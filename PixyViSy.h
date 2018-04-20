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
        /* "pixel_Fx"/"Fy" are intrinsic parameters and "flag" determines
           what will be processed in "update" function - there are three
           options: PIXYVISY_GOAL, PIXYVISY_BALL, for both at the same time
           use PIXYVISY_GOAL | PIXYVISY_BALL. Further information in README.
        */
        PixyViSy(uint16_t pixel_Fx, uint16_t pixel_Fy, uint8_t flag);
        void update(void);

        /* Getters to obtain data computed in last "update" function call */
        uint16_t getGoalDist(void) { return goal_dist; }
        uint8_t getGoalRightPixels(void) { return goal_right_pixels; }
        uint8_t getGoalLeftPixels(void) { return goal_left_pixels; }
        uint16_t getBlocksCount(void) { return blocks_count; }
        uint8_t getGoalPixHeight(void) { return goal_pix_height; }
        char getGoalAction(void) { return goal_action; }
        uint16_t getBallDist(void) { return ball_dist; }
        int16_t getBallAngle(void) { return ball_angle; }

        /* Functions used to set/change all parameters */
        void setGoalSig(uint8_t sig) { goal_sig = sig; }
        void setGoalHeight(uint8_t _goal_height) { goal_height = _goal_height;}
        void setPixelFy(uint16_t pixel_Fy) {
            if (pixel_Fy != 0) Fyp = pixel_Fy; }
        void setPixelFx(uint16_t pixel_Fx) {
            if (pixel_Fx != 0) Fxp = pixel_Fx; }
        void setMinGoalArea(uint16_t _min_goal_area) {
            min_goal_area = _min_goal_area; }
        void setBallSize(uint8_t _ball_size) { ball_size = _ball_size; }
        void setBallSig(uint8_t _ball_sig) { ball_sig = _ball_sig; }
        void setMinBallArea(uint16_t _min_ball_area)
            { min_ball_area = _min_ball_area; }
        void setFlag(uint8_t flag) { process_flag = flag; }

        /* Prints parameters using Serial */
        void printParams();

    private:
        /* puts "n" indexes of blocks (of signature "sig") from the biggest
           to the smallest (but at least of area "thresh") to out_blocks */
        uint16_t findNMax(uint8_t sig, uint16_t n, uint16_t *out_blocks,
            uint16_t thresh);
        void setDefValues();
        void processGoal();
        void processBall();

        /* Attention: X, Y, D, Z should not exceed cca 100 (2**15 / Fp) */
        inline int16_t getRealX(int16_t Xp, int16_t Z);
        inline int16_t getRealY(int16_t Yp, int16_t Z);
        /* "D", "Dp" - horizontal/vertical ("axis") distance between two points
           (in a plane parallel to image sensor) in reality, image respectively
           "axis" - X, x, H, h, Y, y, V, v */
        inline int16_t getRealZ(int16_t D, int16_t Dp, char axis);
        inline int16_t getPixX(int16_t X, int16_t Z);
        inline int16_t getPixY(int16_t Y, int16_t Z);
        inline int16_t getAngleH(int16_t Xp);
        inline int16_t getAngleV(int16_t Yp);

        XLCPixy<LinkSPI> pixy;
        int16_t Fyp; /* vertical focal length in pixels */
        int16_t Fxp; /* horizontal focal length in pixels */
        uint8_t process_flag;
        uint8_t goal_sig;
        uint8_t goal_height;
        uint16_t min_goal_area;
        uint8_t ball_sig;
        uint8_t ball_size;
        uint16_t min_ball_area;

        uint16_t blocks_count;
        uint8_t goal_left_pixels;
        uint8_t goal_right_pixels;
        uint8_t goal_pix_height;
        char goal_action;
        uint16_t goal_dist;
        uint16_t ball_dist;
        int16_t ball_angle;
};

#endif /* _PIXYVISY_H */

/* vim: set tabstop=4 softtabstop=4 shiftwidth=4 expandtab : */
