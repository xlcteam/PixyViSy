#ifndef _PIXYVISY_H
#define _PIXYVISY_H

#include <SPI.h>
#include <Pixy.h>
#include <stdint.h>

class PixyViSy
{
    public:
        PixyViSy(uint8_t sig, uint16_t pixel_Fy, uint16_t pixel_Fx,
            uint8_t _goal_height);
        void update(void);
        uint16_t getDistance(void) { return distance; }
        uint8_t getRightPixels(void) { return right_pixels; }
        uint8_t getLeftPixels(void) { return left_pixels; }
        uint16_t getBlocksCount(void) { return blocks_count; }
        uint8_t getGoalPixHeight(void) { return goal_pix_height; }
        void setSignature(uint8_t sig) { signature = sig; }
        void setGoalHeight(uint8_t _goal_height) { goal_height = _goal_height;}
        void setPixelFy(uint16_t pixel_Fy) {
            if (pixel_Fy != 0) Fyp = pixel_Fy;
        }
        void setPixelFx(uint16_t pixel_Fx) {
            if (pixel_Fx != 0) Fxp = pixel_Fx;
        }
        char getAction(void) { return action; }
    private:
        void findNMax(uint8_t sig, uint16_t n, uint16_t *out_blocks,
            uint16_t thresh);
        inline int16_t getRealX(int16_t Xp, int16_t Z);
        inline int16_t getRealY(int16_t Yp, int16_t Z);
        inline int16_t getRealZ(int16_t D, int16_t Dp, char axis);
        inline int16_t getPixX(int16_t X, int16_t Z);
        inline int16_t getPixY(int16_t Y, int16_t Z);

        Pixy pixy;
        int16_t Fyp; // vertical focal length (multiplied by 100) in pixels
        int16_t Fxp; // horizontal focal length (multiplied by 100) in pixels

        uint16_t blocks_count;
        uint8_t signature;
        uint8_t left_pixels;
        uint8_t right_pixels;
        uint8_t goal_height;
        uint8_t goal_pix_height;
        char action;
        uint16_t distance;
};

#endif /* _PIXYVISY_H */

/* vim: set tabstop=4 softtabstop=4 shiftwidth=4 expandtab : */
