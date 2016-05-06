#include <PixyViSy.h>

const uint16_t min_block_area = 100;
const uint16_t ball_size = 12;
const uint16_t pixy_screen_half = (PIXY_MAX_X +  1) / 2;

inline uint16_t block_size(Block &block)
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
    blocks_count = 0;
    left_pixels = 0;
    right_pixels = 0;
    action = 'N'; // as "Nothing"
    distance = ~0;
    goal_pix_height = 0;
    uint8_t all_blocks_count = pixy.getBlocks();
    uint8_t max[3] = { all_blocks_count, all_blocks_count, all_blocks_count };

    if (all_blocks_count) {
        // finding three max blocks
        for (uint8_t i = 0; i < all_blocks_count; i++) {
            if (pixy.blocks[i].signature == signature
            && block_size(pixy.blocks[i]) >= min_block_area) {
                if (max[2] == all_blocks_count || block_size(pixy.blocks[i])
                > block_size(pixy.blocks[max[2]])) {
                    if (max[1] == all_blocks_count ||
                    block_size(pixy.blocks[i]) >
                    block_size(pixy.blocks[max[1]])) {
                        if (max[0] == all_blocks_count ||
                        block_size(pixy.blocks[i]) >
                        block_size(pixy.blocks[max[0]])) {
                            max[2] = max[1];
                            max[1] = max[0];
                            max[0] = i;
                        } else {
                            max[2] = max[1];
                            max[0] = i;
                        }
                    } else {
                        max[2] = i;
                    }
                }
            }
        }
        // counting left_pixels, right_pixels, blocks_count, goal_pix_height
        for (uint8_t i = 0; i < 3; i++) {
            if (max[i] != all_blocks_count) {
                blocks_count++;
                goal_pix_height += pixy.blocks[max[i]].height;
                // x coordinates of block's borders
                uint16_t left_border = pixy.blocks[max[i]].x -
                                       pixy.blocks[max[i]].width / 2;
                uint16_t right_border = pixy.blocks[max[i]].x +
                                        pixy.blocks[max[i]].width / 2;

                if (right_border < pixy_screen_half) {
                    left_pixels += pixy.blocks[max[i]].width;
                } else if (left_border > pixy_screen_half) {
                    right_pixels += pixy.blocks[max[i]].width;
                } else {
                    left_pixels += pixy_screen_half - left_border;
                    right_pixels += right_border - pixy_screen_half;
                }
            }
        }
        goal_pix_height /= blocks_count; // average value
        // counting distance and suggesting action
        if (blocks_count) {
            distance = calib_const / goal_pix_height; // inverse proportion
            // direct proportion
            uint8_t ball_pix_size = goal_pix_height * ball_size / goal_height;

            for (uint8_t i = 0; i < blocks_count; i++) {
                // x coordinates of goal_block's borders
                uint16_t goal_right = pixy.blocks[max[i]].x +
                                      pixy.blocks[max[i]].width / 2;
                uint16_t goal_left = pixy.blocks[max[i]].x -
                                     pixy.blocks[max[i]].width / 2;
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
}

/* vim: set tabstop=4 softtabstop=4 shiftwidth=4 expandtab : */
