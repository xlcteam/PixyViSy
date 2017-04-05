#include <PixyViSy.h>
#include <math.h>

const int16_t h_center = (PIXY_MAX_X +  1) / 2;
const int16_t v_center = (PIXY_MAX_Y +  1) / 2;

inline uint16_t block_size(Block& block)
{
    return block.height * block.width;
}

PixyViSy::PixyViSy(uint16_t pixel_Fx, uint16_t pixel_Fy, uint8_t _goal_sig,
    uint8_t _goal_height, uint16_t _min_goal_size, uint8_t _ball_size,
    uint8_t _ball_sig, uint16_t _min_ball_size, uint8_t flag)
{
    pixy.init();

    Fyp = pixel_Fy;
    Fxp = pixel_Fx;
    process_flag = flag;

    goal_sig = _goal_sig;
    goal_height = _goal_height;
    min_goal_size = _min_goal_size;
    ball_size = _ball_size;
    ball_sig = _ball_sig;
    min_ball_size = _min_ball_size;

    setDefValues();
}

void PixyViSy::update(void)
{
    setDefValues();
    blocks_count = pixy.getBlocks();

    if (blocks_count == 0) {
        return;
    }

    if (process_flag & PIXYVISY_GOAL) {
        processGoal();
    }
    if (process_flag & PIXYVISY_BALL) {
        processBall();
    }
}

void PixyViSy::processBall()
{
    uint16_t ball_i;
    if (findNMax(ball_sig, 1, &ball_i, min_ball_size) == 0) {
        return;
    }
    Block& ball = pixy.blocks[ball_i];
    if (ball.width > ball.height) {
        ball_dist = getRealZ(ball_size, ball.width, 'X');
    } else {
        ball_dist = getRealZ(ball_size, ball.height, 'Y');
    }
    ball_angle = getAngleH(ball.x);
}

void PixyViSy::processGoal()
{
    uint16_t max[3];
    uint8_t goal_blocks_count = findNMax(goal_sig, 3, max, min_goal_size);

    if (goal_blocks_count == 0) {
        return;
    }

    // counting left_pixels, right_pixels, goal_pix_height
    for (uint8_t i = 0; i < goal_blocks_count; i++) {
        Block& block = pixy.blocks[max[i]];

        goal_pix_height += block.height;

        // x coordinates of block's borders
        uint16_t left_border = block.x - block.width / 2;
        uint16_t right_border = block.x + block.width / 2;

        if (right_border < h_center) {
            goal_left_pixels += block.width;
        } else if (left_border > h_center) {
            goal_right_pixels += block.width;
        } else {
            goal_left_pixels += h_center - left_border;
            goal_right_pixels += right_border - h_center;
        }
    }
    // counting distance and suggesting action
    goal_pix_height /= goal_blocks_count; // average value
    goal_dist = getRealZ(goal_height, goal_pix_height, 'Y');
    // x coordinates of ball's borders
    // (just prediction where it should be after kick)
    int16_t ball_left = getPixX(-ball_size/2, goal_dist);
    int16_t ball_right = getPixX(ball_size/2, goal_dist);

    for (uint8_t i = 0; i < goal_blocks_count; i++) {
        Block& block = pixy.blocks[max[i]];
        // x coordinates of goal_block's borders
        int16_t goal_right = block.x + block.width / 2;
        int16_t goal_left = block.x - block.width / 2;

        if (goal_left <= ball_left && goal_right >= ball_right) {
            goal_action = 'K'; // as "Kick"
            return;
        }
    }
    if (goal_left_pixels > goal_right_pixels) {
        goal_action = 'L'; // as "move Left"
    } else {
        goal_action = 'R'; // as "move Right"
    }
}

void PixyViSy::setDefValues()
{
    blocks_count = 0;
    goal_left_pixels = 0;
    goal_right_pixels = 0;
    goal_pix_height = 0;
    goal_action = 'N'; // as "Nothing"
    goal_dist = ~0;
    ball_dist = ~0;
    ball_angle = 0;
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

inline int16_t PixyViSy::getAngleH(int16_t Xp)
{
    return RAD_TO_DEG * atan2(Xp - h_center, Fxp);
}

inline int16_t PixyViSy::getAngleV(int16_t Yp)
{
    return RAD_TO_DEG * atan2(Yp - h_center, Fyp);
}

/* vim: set tabstop=4 softtabstop=4 shiftwidth=4 expandtab : */
