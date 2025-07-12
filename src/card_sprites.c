#include "sprite.h"

const int ROW = 4;
const int COL = 13;

const int CARD_WIDTH = 88;
const int CARD_HEIGHT = 119;

const int CARD_GAP_X = 5;
const int CARD_GAP_Y = 5;

const int WIDTH = COL * (CARD_WIDTH + CARD_GAP_X) - CARD_GAP_X;
const int HEIGHT = (ROW + 1) * (CARD_HEIGHT + CARD_GAP_Y) - CARD_GAP_Y;

static Sprite get_card_sprite_at_idx(int row, int col) {
  Sprite sprite = {0};
  sprite.color.r = 1.0f;
  sprite.color.g = 1.0f;
  sprite.color.b = 1.0f;
  sprite.color.a = 1.0f;

  sprite.width = (float)CARD_WIDTH;
  sprite.height = (float)CARD_HEIGHT;

  sprite.uv_top = 1.0f - row * (CARD_HEIGHT + CARD_GAP_Y) / (float)HEIGHT;
  sprite.uv_bottom = sprite.uv_top - (float)CARD_HEIGHT / (float)HEIGHT;
  sprite.uv_left = col * (CARD_WIDTH + CARD_GAP_X) / (float)WIDTH;
  sprite.uv_right = sprite.uv_left + (float)CARD_WIDTH / (float)WIDTH;

  return sprite;
}

void generate_card_sprites(Sprite cards[]) {
  // Empty card at index 0
  cards[0] = get_card_sprite_at_idx(4, 1);

  for (int col = 0; col < COL; col++) {
    for (int row = 0; row < ROW; row++) {
      int index = row * COL + COL - col;
      cards[index] = get_card_sprite_at_idx(row, col);
    }
  }
}
