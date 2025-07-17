#include "rendering/sprite.h"

// Spritesheet
const char MAIN_SPRITESHEET[] = {
#embed "assets/spritesheet.png"
};

const size_t MAIN_SPRITESHEET_SIZE = sizeof(MAIN_SPRITESHEET);

const int WIDTH = 1856;
const int HEIGHT = 724;

// Card sprite properties
const int CARD_ROW = 4;
const int CARD_COL = 13;

const int CARD_WIDTH = 127;
const int CARD_HEIGHT = 180;

const int CARD_GAP_X = 1;
const int CARD_GAP_Y = 1;

const float CARD_SCALE = 0.85f;

// Character sprite properties
const int CHAR_OFFSET_X = 13 * 128;
const int CHAR_OFFSET_Y = 0;

const int CHAR_COL = 6;

const int CHAR_WIDTH = 32;
const int CHAR_HEIGHT = 32;
const int CHAR_GAP_X = 0;
const int CHAR_GAP_Y = 0;

const float CHAR_SCALE = 1.0f;

static Sprite get_card_sprite_at_idx(int row, int col) {
    Sprite sprite = { 0 };
    sprite.color.r = 1.0f;
    sprite.color.g = 1.0f;
    sprite.color.b = 1.0f;
    sprite.color.a = 1.0f;

    sprite.width = (float)CARD_WIDTH * CARD_SCALE;
    sprite.height = (float)CARD_HEIGHT * CARD_SCALE;

    sprite.uv_top = 1.0f - row * (CARD_HEIGHT + CARD_GAP_Y) / (float)HEIGHT;
    sprite.uv_bottom = sprite.uv_top - (float)CARD_HEIGHT / (float)HEIGHT;
    sprite.uv_left = col * (CARD_WIDTH + CARD_GAP_X) / (float)WIDTH;
    sprite.uv_right = sprite.uv_left + (float)CARD_WIDTH / (float)WIDTH;

    return sprite;
}

static Sprite get_character_sprite_at_idx(int row, int col) {
    Sprite sprite = { 0 };
    sprite.color.r = 1.0f;
    sprite.color.g = 1.0f;
    sprite.color.b = 1.0f;
    sprite.color.a = 1.0f;

    sprite.width = (float)CHAR_WIDTH * CHAR_SCALE;
    sprite.height = (float)CHAR_HEIGHT * CHAR_SCALE;

    sprite.uv_top = 1.0f - (CHAR_OFFSET_Y + row * (CHAR_HEIGHT + CHAR_GAP_Y)) / (float)HEIGHT;
    sprite.uv_bottom = sprite.uv_top - (float)CHAR_HEIGHT / (float)HEIGHT;
    sprite.uv_left = (CHAR_OFFSET_X + col * (CHAR_WIDTH + CHAR_GAP_X)) / (float)WIDTH;
    sprite.uv_right = sprite.uv_left + (float)CHAR_WIDTH / (float)WIDTH;
    return sprite;
}

void generate_card_sprites(Sprite cards[]) {
    cards[0] = get_card_sprite_at_idx(3, 13); // Placeholder for empty card

    for (int col = 0; col < CARD_COL; col++) {
        for (int row = 0; row < CARD_ROW; row++) {
            int index = row * CARD_COL + col + 1;
            cards[index] = get_card_sprite_at_idx(row, col);
        }
    }
}

void generate_character_sprites(Sprite text[]) {
    char character_set[]
        = " !\"#$%&'()*+,-./"
          "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

    // Sprite for space (empty)
    Sprite sprite = get_character_sprite_at_idx(0, 0);
    sprite.uv_top = sprite.uv_bottom;
    sprite.uv_left = sprite.uv_right;
    text[0] = sprite;

    // Sprites for all the non-extended printable ASCII characters
    const int NUM_COLS = 6;
    for (size_t i = 0; i < sizeof(character_set) - 1; i++) {
        char c = character_set[i];
        int col = i % NUM_COLS;
        int row = i / NUM_COLS;

        text[i + 1] = get_character_sprite_at_idx(row, col);
    }
}
