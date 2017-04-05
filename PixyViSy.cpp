#include <PixyViSy.h>

const uint16_t min_block_area = 100;
const uint16_t ball_size = 12;
const uint16_t pixy_screen_half = (PIXY_MAX_X +  1) / 2;

inline uint16_t block_size(Block& block)
{
    return block.height * block.width;
}

PixyViSy::PixyViSy(uint8_t sig, uint16_t calib_dis_pix, uint8_t _goal_height)
{
    pixy.init();
    signature = sig;
    calib_const = calib_dis_pix;
    left_pixels = 0, right_pixels = 0, blocks_count = 0;
    goal_height = _goal_height;
    action = 'N';
}

void PixyViSy::update(void)
{
    // set default values
    left_pixels = 0;
    right_pixels = 0;
    action = 'N'; // as "Nothing"
    distance = ~0;
    goal_pix_height = 0;

    blocks_count = pixy.getBlocks();
    uint16_t max[3] = { blocks_count, blocks_count, blocks_count };
    uint8_t goal_blocks_count = 0;

    if (blocks_count == 0) {
        return;
    }

    findNMax(signature, 3, max);

    // counting left_pixels, right_pixels, goal_pix_height
    for (uint8_t i = 0; i < 3; i++) {
        if (max[i] != blocks_count) {
            Block& block = pixy.blocks[max[i]];
            goal_blocks_count++;

            goal_pix_height += block.height;

            // x coordinates of block's borders
            uint16_t left_border = block.x - block.width / 2;
            uint16_t right_border = block.x + block.width / 2;

            if (right_border < pixy_screen_half) {
                left_pixels += block.width;
            } else if (left_border > pixy_screen_half) {
                right_pixels += block.width;
            } else {
                left_pixels += pixy_screen_half - left_border;
                right_pixels += right_border - pixy_screen_half;
            }
        }
    }
    // counting distance and suggesting action
    if (goal_blocks_count) {
        goal_pix_height /= goal_blocks_count; // average value
        distance = calib_const / goal_pix_height; // inverse proportion
        // direct proportion
        uint8_t ball_pix_size = goal_pix_height * ball_size / goal_height;

        for (uint8_t i = 0; i < goal_blocks_count; i++) {
            Block& block = pixy.blocks[max[i]];
            // x coordinates of goal_block's borders
            uint16_t goal_right = block.x + block.width / 2;
            uint16_t goal_left = block.x - block.width / 2;
            // x coordinates of ball's borders
            // (just prediction where it should be after kick)
            uint16_t ball_left = pixy_screen_half - ball_pix_size / 2;
            uint16_t ball_right = pixy_screen_half + ball_pix_size / 2;

            if (goal_left <= ball_left && goal_right >= ball_right) {
                action = 'K'; // as "Kick"
                break;
            }
        }
        // in case action wasn't set to "Kick"
        if (action == 'N') {
            if (left_pixels > right_pixels) {
                action = 'L'; // as "move Left"
            } else {
                action = 'R'; // as "move Right"
            }
        }
    }
}

// puts indexes of blocks from the biggest to the smallest to out_blocks
void PixyViSy::findNMax(uint8_t sig, uint16_t n, uint16_t *out_blocks)
{
    // pixy sends blocks in order by size from the biggest
    for (uint16_t i = 0, j = 0; i < blocks_count && j < n; i++) {
        if (pixy.blocks[i].signature == sig) {
            out_blocks[j] = i;
            j++;
        }
    }
}

/* vim: set tabstop=4 softtabstop=4 shiftwidth=4 expandtab : */
