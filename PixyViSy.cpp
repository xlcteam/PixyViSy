#include <PixyViSy.h>

const uint16_t min_block_area = 100;
const uint16_t ball_size = 12;
const int16_t h_center = (PIXY_MAX_X +  1) / 2;
const int16_t v_center = (PIXY_MAX_Y +  1) / 2;

inline uint16_t block_size(Block& block)
{
    return block.height * block.width;
}

PixyViSy::PixyViSy(uint8_t sig, uint16_t pixel_Fy, uint16_t pixel_Fx,
    uint8_t _goal_height)
{
    pixy.init();
    signature = sig;
    Fyp = pixel_Fy;
    Fxp = pixel_Fx;
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

    if (blocks_count == 0) {
        return;
    }

    uint8_t goal_blocks_count = findNMax(signature, 3, max, min_block_area);

    // counting left_pixels, right_pixels, goal_pix_height
    for (uint8_t i = 0; i < 3; i++) {
        if (max[i] != blocks_count) {
            Block& block = pixy.blocks[max[i]];

            goal_pix_height += block.height;

            // x coordinates of block's borders
            uint16_t left_border = block.x - block.width / 2;
            uint16_t right_border = block.x + block.width / 2;

            if (right_border < h_center) {
                left_pixels += block.width;
            } else if (left_border > h_center) {
                right_pixels += block.width;
            } else {
                left_pixels += h_center - left_border;
                right_pixels += right_border - h_center;
            }
        }
    }
    // counting distance and suggesting action
    if (goal_blocks_count) {
        goal_pix_height /= goal_blocks_count; // average value
        distance = getRealZ(goal_height, goal_pix_height, 'Y');
        // x coordinates of ball's borders
        // (just prediction where it should be after kick)
        int16_t ball_left = getPixX(-ball_size/2, distance);
        int16_t ball_right = getPixX(ball_size/2, distance);

        for (uint8_t i = 0; i < goal_blocks_count; i++) {
            Block& block = pixy.blocks[max[i]];
            // x coordinates of goal_block's borders
            int16_t goal_right = block.x + block.width / 2;
            int16_t goal_left = block.x - block.width / 2;

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
uint16_t PixyViSy::findNMax(uint8_t sig, uint16_t n, uint16_t *out_blocks,
    uint16_t thresh)
{
    uint16_t i, cnt;
    // pixy sends blocks in order by size from the biggest
    for (i = 0, cnt = 0; i < blocks_count && cnt < n; i++) {
        if (pixy.blocks[i].signature == sig) {
            if (block_size(pixy.blocks[i]) < thresh) {
                break;
            }
            out_blocks[cnt] = i;
            cnt++;
        }
    }
    return cnt;
}

inline int16_t PixyViSy::getRealX(int16_t Xp, int16_t Z)
{
    return (Xp - h_center) * Z / Fxp;
}

inline int16_t PixyViSy::getRealY(int16_t Yp, int16_t Z)
{
    return (v_center - Yp) * Z / Fyp;
}

// D - delta between two points in reality (Dp in image)
inline int16_t PixyViSy::getRealZ(int16_t D, int16_t Dp, char axis)
{
    if (axis == 'x' || axis == 'X' || axis == 'h' || axis == 'H') {
        return Fxp * D / Dp;
    } else if (axis == 'y' || axis == 'Y' || axis == 'v' || axis == 'V') {
        return Fyp * D / Dp;
    }
}

inline int16_t PixyViSy::getPixX(int16_t X, int16_t Z)
{
    return h_center + X * Fxp / Z;
}

inline int16_t PixyViSy::getPixY(int16_t Y, int16_t Z)
{
    return v_center - Y * Fyp / Z;
}

/* vim: set tabstop=4 softtabstop=4 shiftwidth=4 expandtab : */
