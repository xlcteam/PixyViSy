#include <PixyViSy.h>

PixyViSy::PixyViSy(uint8_t sig, uint16_t calibration_dis_pix, uint8_t _goal_height)
{
  pixy.init();
  signature = sig;
  calib_const = calibration_dis_pix;
  left_pixels = 0, right_pixels = 0, blocks_count = 0;
  goal_height = _goal_height;
  action = 'N';
}
void PixyViSy::update(void)
{
  blocks_count = 0;
  uint8_t all_blocks_count = pixy.getBlocks();
  left_pixels = 0;
  right_pixels = 0;
  action = 'N';
  distance = ~0;
  uint16_t goal_pix_height = 0;
  uint8_t max[3] = { all_blocks_count, all_blocks_count, all_blocks_count };

  if (all_blocks_count) {
    for (uint8_t i = 0; i < all_blocks_count; i++) {
      if (pixy.blocks[i].signature == signature && pixy.blocks[i].height * pixy.blocks[i].width >= MIN_BLOCK_AREA) {
        // finding three max values
        if (max[2] == all_blocks_count || BLOCK_SIZE(pixy.blocks[i]) > BLOCK_SIZE(pixy.blocks[max[2]])) {
          if (max[1] == all_blocks_count || BLOCK_SIZE(pixy.blocks[i]) > BLOCK_SIZE(pixy.blocks[max[1]])) {
            if (max[0] == all_blocks_count || BLOCK_SIZE(pixy.blocks[i]) > BLOCK_SIZE(pixy.blocks[max[0]])) {
              max[2] = max[1];
              max[1] = max[0];
              max[0] = i;
            }
            else {
              max[2] = max[1];
              max[0] = i;
            }
          }
          else {
            max[2] = i;
          }
        }
      }
    }

    for (uint8_t i = 0; i < 3; i++) {
      if (max[i] != all_blocks_count) {
        blocks_count++;
        goal_pix_height += pixy.blocks[max[i]].height;
        uint16_t left = pixy.blocks[max[i]].x - pixy.blocks[max[i]].width / 2;
        uint16_t right = pixy.blocks[max[i]].x + pixy.blocks[max[i]].width / 2;
        
        if (right < 160) {
          left_pixels += pixy.blocks[max[i]].width;
        } else if (left > 160) {
          right_pixels += pixy.blocks[max[i]].width;
        } else {
          left_pixels += 160 - left;
          right_pixels += right - 160;
        }
      }
    }

    if (blocks_count) {
      distance = calib_const * blocks_count / goal_pix_height;
      uint8_t kicked_ball_size = goal_pix_height * 12 / (goal_height * blocks_count);
      for (uint8_t i = 0; i < blocks_count; i++) {
        if (pixy.blocks[max[i]].signature == signature) {
          uint16_t finish = pixy.blocks[max[i]].x + pixy.blocks[max[i]].width / 2;
          uint16_t start = pixy.blocks[max[i]].x - pixy.blocks[max[i]].width / 2;
          uint16_t ball_start = 160 - kicked_ball_size / 2;
          uint16_t ball_finish = 160 + kicked_ball_size / 2;
          
          if (start <= ball_start && finish >= ball_finish) {
            action = 'K';
            break;
          }
        }
      }
      if (action == 'N') {
        if (left_pixels > right_pixels) {
          action = 'L';
        } else {
          action = 'R';
        }
      }
    }
  }
}