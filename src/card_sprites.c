#include "sprite.h"

const int ROW = 4;
const int COL = 13;

const int CARD_WIDTH = 127;
const int CARD_HEIGHT = 180;

const int CARD_GAP_X = 1;
const int CARD_GAP_Y = 1;

const int WIDTH = 1792;
const int HEIGHT = 724;

const float SCALE = 0.7f;

const char MAIN_SPRITESHEET[] = {
#embed "assets/spritesheet.png"
};

const size_t MAIN_SPRITESHEET_SIZE = sizeof(MAIN_SPRITESHEET);

static Sprite get_card_sprite_at_idx(int row, int col) {
  Sprite sprite = {0};
  sprite.color.r = 1.0f;
  sprite.color.g = 1.0f;
  sprite.color.b = 1.0f;
  sprite.color.a = 1.0f;

  sprite.width = (float)CARD_WIDTH * SCALE;
  sprite.height = (float)CARD_HEIGHT * SCALE;

  sprite.uv_top = 1.0f - row * (CARD_HEIGHT + CARD_GAP_Y) / (float)HEIGHT;
  sprite.uv_bottom = sprite.uv_top - (float)CARD_HEIGHT / (float)HEIGHT;
  sprite.uv_left = col * (CARD_WIDTH + CARD_GAP_X) / (float)WIDTH;
  sprite.uv_right = sprite.uv_left + (float)CARD_WIDTH / (float)WIDTH;

  return sprite;
}

void generate_card_sprites(Sprite cards[]) {
  cards[0] = get_card_sprite_at_idx(0, 13);

  for (int col = 0; col < COL; col++) {
    for (int row = 0; row < ROW; row++) {
      int index = row * COL + col + 1;
      cards[index] = get_card_sprite_at_idx(row, col);
    }
  }
}
