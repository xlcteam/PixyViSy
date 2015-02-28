PixyViSy::PixyViSy(uint8_t sig, uint16_t calibration_dis_pix, uint8_t _goal_height)
{
  pixy.init();
  signature = sig;
  calib_const = calibration_dis_pix;
  left_pixels = 0, right_pixels = 0, blocks_count = 0;
  goal_height = _goal_height;
  action = '\0';
}
void PixyViSy::update(void)
{
  blocks_count = pixy.getBlocks();
  left_pixels = 0;
  right_pixels = 0;
  action = '\0';
  float goal_pix_height = 0.0;
  if (blocks_count) {
    for (uint8_t i = 0; i < blocks_count; i++) {
      goal_pix_height += pixy.blocks[i].height;
      uint16_t left = pixy.blocks[i].x - pixy.blocks[i].width / 2;
      uint16_t right = pixy.blocks[i].x + pixy.blocks[i].width / 2;
      
      if (right < 160) {
        left_pixels += pixy.blocks[i].width;
      } else if (left > 160) {
        right_pixels += pixy.blocks[i].width;
      } else {
        left_pixels += 160 - left;
        right_pixels += right - 160;
      }
    }
    goal_pix_height /= blocks_count;
    distance = calib_const / goal_pix_height;
    uint8_t kicked_ball_size = goal_pix_height * 10 / goal_height;
    for (uint8_t i = 0; i < blocks_count; i++) {
      uint16_t finish = pixy.blocks[i].x + pixy.blocks[i].width / 2;
      uint16_t start = pixy.blocks[i].x - pixy.blocks[i].width / 2;
      uint16_t ball_start = 160 - kicked_ball_size / 2;
      uint16_t ball_finish = 160 + kicked_ball_size / 2;
      if (start <= ball_start && finish >= ball_finish) {
        action = 'K';
      } else {
        action = 'N';
        break;
      }
    }
    if (action == 'N') {
      if (left_pixels > right_pixels) {
        action = 'L';
      } else {
        action = 'R';
      }
    }
  } else {
    distance = ~0;
  }
}