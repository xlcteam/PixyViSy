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
  action = 'N'; // as "Nothing"
  distance = ~0;
  goal_pix_height = 0;
  uint8_t max[3] = { all_blocks_count, all_blocks_count, all_blocks_count };

  if (all_blocks_count) {
    /* finding three max blocks */
    for (uint8_t i = 0; i < all_blocks_count; i++) {
      if (pixy.blocks[i].signature == signature && BLOCK_SIZE(pixy.blocks[i]) >= MIN_BLOCK_AREA) {
        if (max[2] == all_blocks_count || BLOCK_SIZE(pixy.blocks[i]) > BLOCK_SIZE(pixy.blocks[max[2]])) {
          if (max[1] == all_blocks_count || BLOCK_SIZE(pixy.blocks[i]) > BLOCK_SIZE(pixy.blocks[max[1]])) {
            if (max[0] == all_blocks_count || BLOCK_SIZE(pixy.blocks[i]) > BLOCK_SIZE(pixy.blocks[max[0]])) {
              max[2] = max[1];
              max[1] = max[0];
              max[0] = i;
            } else {
              max[2] = max[1];
              max[0] = i;
            }
          } else {
            max[2] = i;
          }
        }
      }
    }
    /* counting left_pixels, right_pixels, blocks_count and goal_pix_height */
    for (uint8_t i = 0; i < 3; i++) {
      if (max[i] != all_blocks_count) {
        blocks_count++;
        goal_pix_height += pixy.blocks[max[i]].height;
        /* x coordinates of block's borders */
        uint16_t left_border = pixy.blocks[max[i]].x - pixy.blocks[max[i]].width / 2;
        uint16_t right_border = pixy.blocks[max[i]].x + pixy.blocks[max[i]].width / 2;
        
        if (right_border < PIXY_SCREEN_HALF) {
          left_pixels += pixy.blocks[max[i]].width;
        } else if (left_border > PIXY_SCREEN_HALF) {
          right_pixels += pixy.blocks[max[i]].width;
        } else {
          left_pixels += PIXY_SCREEN_HALF - left_border;
          right_pixels += right_border - PIXY_SCREEN_HALF;
        }
      }
    }
    goal_pix_height /= blocks_count; // average value
    /* counting distance and suggesting action */
    if (blocks_count) {
      distance = calib_const / goal_pix_height; // inverse proportion
      uint8_t ball_pix_size = goal_pix_height * BALL_SIZE / goal_height; // direct proportion

      for (uint8_t i = 0; i < blocks_count; i++) {
        /* x coordinates of goal_block's borders */
        uint16_t goal_right = pixy.blocks[max[i]].x + pixy.blocks[max[i]].width / 2;
        uint16_t goal_left = pixy.blocks[max[i]].x - pixy.blocks[max[i]].width / 2;
        /* x coordinates of ball's borders (just prediction where it shuld be after kick) */
        uint16_t ball_left = PIXY_SCREEN_HALF - ball_pix_size / 2;
        uint16_t ball_right = PIXY_SCREEN_HALF + ball_pix_size / 2;
          
        if (goal_left <= ball_left && goal_right >= ball_right) {
          action = 'K'; // as "Kick"
          break;
        }
      }
      /* in case action wasn't set to "Kick" */
      if (action == 'N') {
        if (left_pixels > right_pixels) {
          action = 'L'; // as "move Left"
        } else {
          action = 'R'; // as "move Right"
        }
      }
    }
  }
}