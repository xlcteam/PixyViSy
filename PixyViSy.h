#ifndef _PIXYVISY_H
#define _PIXYVISY_H

#include <SPI.h>
#include <Pixy.h>
#include <XLCPixy.h>
#include <stdint.h>

class PixyViSy
{
    public:
        PixyViSy(uint8_t sig, uint16_t calib_dis_pix, uint8_t _goal_height);
        void update(void);
        uint16_t getDistance(void) { return distance; }
        uint8_t getRightPixels(void) { return right_pixels; }
        uint8_t getLeftPixels(void) { return left_pixels; }
        uint16_t getBlocksCount(void) { return blocks_count; }
        uint8_t getGoalPixHeight(void) { return goal_pix_height; }
        void setSignature(uint8_t sig) { signature = sig; }
        void setGoalHeight(uint8_t _goal_height) { goal_height = _goal_height;}
        void setCalibConst(uint16_t calibration_dis_pix) {
            if (calibration_dis_pix != 0) calib_const = calibration_dis_pix;
        }
        char getAction(void) { return action; }
    private:
        void findNMax(uint8_t sig, uint16_t n, uint16_t *out_blocks);
        XLCPixy<LinkSPI> pixy;
        uint16_t blocks_count;
        uint8_t signature;
        uint8_t left_pixels;
        uint8_t right_pixels;
        uint8_t goal_height;
        uint8_t goal_pix_height;
        uint16_t calib_const;
        char action;
        uint16_t distance;
};

#endif /* _PIXYVISY_H */

/* vim: set tabstop=4 softtabstop=4 shiftwidth=4 expandtab : */
