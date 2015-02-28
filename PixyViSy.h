#ifndef _pixyvisy
  #define _pixyvisy
  
  #include <SPI.h>  
  #include <Pixy.h>
  #include <stdint.h>
  
  class PixyViSy
  {
    public:
      PixyViSy(uint8_t sig, uint16_t calibration_dis_pix, uint8_t _goal_height);
      void update(void);
      float getDistance(void) { return distance; }
      uint8_t getRightPixels(void) { return right_pixels; }
      uint8_t getLeftPixels(void) { return left_pixels; }
      uint8_t getBlocksCount(void) { return blocks_count; }
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
      float distance;
  };
#endif