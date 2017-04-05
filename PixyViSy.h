#ifndef _PIXYVISY_H
#define _PIXYVISY_H

#include <SPI.h>
#include <Pixy.h>
#include <XLCPixy.h>
#include <stdint.h>

#define PIXYVISY_GOAL B00000001
#define PIXYVISY_BALL B00000010

class PixyViSy
{
    public:
        PixyViSy(uint16_t pixel_Fx, uint16_t pixel_Fy, uint8_t _goal_sig,
            uint8_t _goal_height, uint16_t _min_goal_size, uint8_t _ball_size,
            uint8_t _ball_sig, uint16_t _min_ball_size, uint8_t flag);
        void update(void);

        uint16_t getGoalDist(void) { return goal_dist; }
        uint8_t getGoalRightPixels(void) { return goal_right_pixels; }
        uint8_t getGoalLeftPixels(void) { return goal_left_pixels; }
        uint16_t getBlocksCount(void) { return blocks_count; }
        uint8_t getGoalPixHeight(void) { return goal_pix_height; }
        char getGoalAction(void) { return goal_action; }
        uint16_t getBallDist(void) { return ball_dist; }
        uint16_t getBallAngle(void) { return ball_angle; }

        void setGoalSig(uint8_t sig) { goal_sig = sig; }
        void setGoalHeight(uint8_t _goal_height) { goal_height = _goal_height;}
        void setPixelFy(uint16_t pixel_Fy) {
            if (pixel_Fy != 0) Fyp = pixel_Fy; }
        void setPixelFx(uint16_t pixel_Fx) {
            if (pixel_Fx != 0) Fxp = pixel_Fx; }
        void setMinGoalSize(uint16_t _min_goal_size) {
            min_goal_size = _min_goal_size; }
        void setBallSize(uint8_t _ball_size) { ball_size = _ball_size; }
        void setBallSig(uint8_t _ball_sig) { ball_sig = _ball_sig; }
        void setMinBallSize(uint16_t _min_ball_size)
            { min_ball_size = _min_ball_size; }

    private:
        uint16_t findNMax(uint8_t sig, uint16_t n, uint16_t *out_blocks,
            uint16_t thresh);
        void setDefValues();
        void processGoal();
        void processBall();
        inline int16_t getRealX(int16_t Xp, int16_t Z);
        inline int16_t getRealY(int16_t Yp, int16_t Z);
        inline int16_t getRealZ(int16_t D, int16_t Dp, char axis);
        inline int16_t getPixX(int16_t X, int16_t Z);
        inline int16_t getPixY(int16_t Y, int16_t Z);
        inline int16_t getAngleH(int16_t Xp);
        inline int16_t getAngleV(int16_t Yp);

        XLCPixy<LinkSPI> pixy;
        int16_t Fyp; // vertical focal length in pixels
        int16_t Fxp; // horizontal focal length in pixels
        uint8_t process_flag;
        uint8_t goal_sig;
        uint8_t goal_height;
        uint8_t ball_size;
        uint16_t min_goal_size;
        uint8_t ball_sig;
        uint16_t min_ball_size;

        uint16_t blocks_count;
        uint8_t goal_left_pixels;
        uint8_t goal_right_pixels;
        uint8_t goal_pix_height;
        char goal_action;
        uint16_t goal_dist;
        uint16_t ball_dist;
        int8_t ball_angle;
};

#endif /* _PIXYVISY_H */

/* vim: set tabstop=4 softtabstop=4 shiftwidth=4 expandtab : */
