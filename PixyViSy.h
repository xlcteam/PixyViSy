#ifndef _PIXYVISY_H
#define _PIXYVISY_H

#include <SPI.h>  
#include <Pixy.h>
#include <stdint.h>

#define MIN_BLOCK_AREA 100
#define BALL_SIZE 12
#define PIXY_SCREEN_HALF 160
#define BLOCK_SIZE(block) ((block).height * (block).width)

class PixyViSy
{
  public:
    PixyViSy(uint8_t sig, uint16_t calibration_dis_pix, uint8_t _goal_height);
    void update(void);
    uint16_t getDistance(void) { return distance; }
    uint8_t getRightPixels(void) { return right_pixels; }
    uint8_t getLeftPixels(void) { return left_pixels; }
    uint8_t getBlocksCount(void) { return blocks_count; }
    void setSignature(uint8_t sig) { signature = sig; }
    void setGoalHeight(uint8_t _goal_height) { goal_height = _goal_height; }
    void setCalibConst(uint16_t calibration_dis_pix) 
        { if (calibration_dis_pix != 0) calib_const = calibration_dis_pix; }
    char getAction(void) { return action; }
  private:
    Pixy pixy;
    uint8_t blocks_count;
    uint8_t signature;
    uint8_t left_pixels;
    uint8_t right_pixels;
    uint8_t goal_height;
    uint16_t calib_const;
    char action;
    uint16_t distance;
};
#endif /*_PIXYVISY_H*/