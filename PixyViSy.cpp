#include <PixyViSy.h>
#include <math.h>

const int16_t h_center = (PIXY_MAX_X +  1) / 2;
const int16_t v_center = (PIXY_MAX_Y +  1) / 2;
const uint8_t def_goal_height = 12;
const uint8_t def_orange_ball_size = 7;
const uint8_t def_IR_ball_size = 8;

inline uint16_t block_area(Block& block)
{
    return block.height * block.width;
}

PixyViSy::PixyViSy(uint16_t pixel_Fx, uint16_t pixel_Fy)
{
    pixy.init();

    Fyp = pixel_Fy;
    Fxp = pixel_Fx;

    goal_height = def_goal_height;
    ball_size = def_IR_ball_size;

    ball_sig = goal_sig = 0;
    min_ball_area = min_goal_area = 0;

    setDefValues();
}

void PixyViSy::printParams()
{
    Serial.print("Fyp: ");           Serial.println(Fyp);
    Serial.print("Fxp: ");           Serial.println(Fxp);
    Serial.print("goal_sig: ");      Serial.println(goal_sig);
    Serial.print("goal_height: ");   Serial.println(goal_height);
    Serial.print("goal_angle: ");    Serial.println(goal_angle);
    Serial.print("min_goal_area: "); Serial.println(min_goal_area);
    Serial.print("ball_sig: ");      Serial.println(ball_sig);
    Serial.print("ball_size: ");     Serial.println(ball_size);
    Serial.print("min_ball_area: "); Serial.println(min_ball_area);
    Serial.print("teammate_sig: ");  Serial.println(team_sig);
    Serial.print("teammate_angle: ");Serial.println(team_angle);
}

void PixyViSy::update(uint8_t flag)
{
    setDefValues();
    blocks_count = pixy.getBlocks();

    if (blocks_count == 0) {
        return;
    }

    if (flag & PIXYVISY_GOAL) {
        processGoal(flag & PIXYVISY_ANGG);
    }
    if (flag & PIXYVISY_BALL) {
        processBall();
    }
    if (flag & PIXYVISY_TEAM) {
        processTeam();
    }
}

void PixyViSy::processBall()
{
    uint16_t ball_i;
    if (findNMax(ball_sig, 1, &ball_i, min_ball_area) == 0) {
        return;
    }
    Block& ball = pixy.blocks[ball_i];
    /* prevent covered ball (e.g. by robot) */
    if (ball.width > ball.height) {
        ball_dist = getRealZ(ball_size, ball.width, 'X');
    } else {
        ball_dist = getRealZ(ball_size, ball.height, 'Y');
    }
    ball_angle = getAngleH(ball.x);
}

uint16_t PixyViSy::valueGoalBlock(Block& block, int16_t Z)
{
    uint16_t value = 0;
    if (block.x > h_center) {
        value = 100 * block.width / (block.x - h_center);
    } else if (block.x < h_center) {
        value = 100 * block.width / (h_center - block.x);
    } else {
        value = 100 * block.width * 2;
    }
    if (getRealX(block.width, Z) > ball_size) {
        value *= 4;
    }
    return value;
}

void PixyViSy::processGoal(bool set_angle)
{
    uint16_t max[3];
    uint8_t goal_blocks_count = findNMax(goal_sig, 3, max, min_goal_area);

    if (goal_blocks_count == 0) {
        return;
    }

    /* counting goal_pix_height */
    for (uint8_t i = 0; i < goal_blocks_count; i++) {
        Block& block = pixy.blocks[max[i]];
        goal_pix_height += block.height;
    }
    // counting distance and suggesting action
    goal_pix_height /= goal_blocks_count; // average value
    goal_dist = getRealZ(goal_height, goal_pix_height, 'Y');
    // x coordinates of ball's borders
    // (just prediction where it should be after kick)
    int16_t ball_left = getPixX(-ball_size/2, goal_dist);
    int16_t ball_right = getPixX(ball_size/2, goal_dist);

    Block& best = pixy.blocks[max[0]];
    uint16_t best_val = valueGoalBlock(best, goal_dist);
    for (uint8_t i = 0; i < goal_blocks_count; i++) {
        Block& block = pixy.blocks[max[i]];
        // x coordinates of goal_block's borders
        int16_t goal_right = block.x + block.width / 2;
        int16_t goal_left = block.x - block.width / 2;

        if (goal_left <= ball_left && goal_right >= ball_right) {
            goal_action = 'K'; // as "Kick"
            if (set_angle) {
                goal_angle = getAngleH(block.x);
            }
            return;
        }

        uint16_t block_val = valueGoalBlock(block, goal_dist);
        if (block_val > best_val) {
            best = block;
            best_val = block_val;
        }
    }
    if (best.x < h_center) {
        goal_action = 'L'; // as "move Left"
    } else {
        goal_action = 'R'; // as "move Right"
    }
    if (set_angle) {
        goal_angle = getAngleH(best.x);
    }
}

void PixyViSy::processTeam()
{
    uint16_t team_i;
    if (findNMax(team_sig, 1, &team_i, min_team_area) == 0) {
        return;
    }
    Block& team = pixy.blocks[team_i];
    team_dist = getRealZ(team_size, team.height, 'Y');
    team_angle = getAngleH(team.x);
    team_dx = getRealX(team.x, team_dist);
}

void PixyViSy::setDefValues()
{
    blocks_count = 0;
    goal_pix_height = 0;
    goal_action = 'N'; // as "Nothing"
    goal_dist = PIXYVISY_NOBLOCK;
    goal_angle = 0;
    ball_dist = PIXYVISY_NOBLOCK;
    ball_angle = 0;
    team_dist = PIXYVISY_NOBLOCK;
    team_angle = 0;
    team_dx = 0;
}

uint16_t PixyViSy::findNMax(uint8_t sig, uint16_t n, uint16_t *out_blocks,
    uint16_t thresh)
{
    uint16_t i, cnt;
    // pixy sends blocks in order by size from the biggest
    for (i = 0, cnt = 0; i < blocks_count && cnt < n; i++) {
        if (pixy.blocks[i].signature == sig) {
            if (block_area(pixy.blocks[i]) < thresh) {
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

/* Note that atan is not as slow as other goniometric functions */
inline int16_t PixyViSy::getAngleH(int16_t Xp)
{
    return RAD_TO_DEG * atan2(Xp - h_center, Fxp);
}

inline int16_t PixyViSy::getAngleV(int16_t Yp)
{
    return RAD_TO_DEG * atan2(Yp - h_center, Fyp);
}

/* vim: set tabstop=4 softtabstop=4 shiftwidth=4 expandtab : */
