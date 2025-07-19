#include "game/ui_sprites.h"

#include "rendering/sprite.h"

#include "game/world.h"

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
const int GLYPH_OFFSET_X = 13 * 128;
const int GLYPH_OFFSET_Y = 0;

const int GLYPH_COL = 6;

const int GLYPH_WIDTH = 32;
const int GLYPH_HEIGHT = 32;
const int GLYPH_GAP_X = 0;
const int GLYPH_GAP_Y = 0;

const float GLYPH_SCALE = 1.0f;

// Button sprite properties

const int BUTTON_COL = 1;
const int BUTTON_ROW = 3;

const int BUTTON_WIDTH = 64;
const int BUTTON_HEIGHT = 65;

const int BUTTON_GAP_X = 0;
const int BUTTON_GAP_Y = 0;

const int BUTTON_OFFSET_X = (CARD_COL + 1) * (CARD_WIDTH + CARD_GAP_X);
const int BUTTON_OFFSET_Y = HEIGHT - BUTTON_ROW * (BUTTON_HEIGHT + BUTTON_GAP_Y);

const float BUTTON_SCALE = 0.8f;

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

static Sprite get_glyph_sprite_at_idx(int row, int col) {
    Sprite sprite = { 0 };
    sprite.color.r = 1.0f;
    sprite.color.g = 1.0f;
    sprite.color.b = 1.0f;
    sprite.color.a = 1.0f;

    sprite.width = (float)GLYPH_WIDTH * GLYPH_SCALE;
    sprite.height = (float)GLYPH_HEIGHT * GLYPH_SCALE;

    sprite.uv_top = 1.0f - (GLYPH_OFFSET_Y + row * (GLYPH_HEIGHT + GLYPH_GAP_Y)) / (float)HEIGHT;
    sprite.uv_bottom = sprite.uv_top - (float)GLYPH_HEIGHT / (float)HEIGHT;
    sprite.uv_left = (GLYPH_OFFSET_X + col * (GLYPH_WIDTH + GLYPH_GAP_X)) / (float)WIDTH;
    sprite.uv_right = sprite.uv_left + (float)GLYPH_WIDTH / (float)WIDTH;
    return sprite;
}

static void populate_card_sprites(Sprite cards[]) {
    cards[0] = get_card_sprite_at_idx(3, 13); // Placeholder for empty card

    for (int col = 0; col < CARD_COL; col++) {
        for (int row = 0; row < CARD_ROW; row++) {
            int index = row * CARD_COL + col + 1;
            cards[index] = get_card_sprite_at_idx(row, col);
        }
    }
}

static void populate_glyph_sprites(World* world) {
    char character_set[]
        = " !\"#$%&'()*+,-./"
          "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

    // Sprite for space (empty)
    Sprite sprite = get_glyph_sprite_at_idx(0, 0);
    sprite.uv_top = sprite.uv_bottom;
    sprite.uv_left = sprite.uv_right;
    world->characters[0] = sprite;

    // Sprites for all the non-extended printable ASCII characters
    const int NUM_COLS = 6;
    for (size_t i = 0; i < sizeof(character_set) - 1; i++) {
        char c = character_set[i];
        int col = i % NUM_COLS;
        int row = i / NUM_COLS;

        world->characters[i + 1] = get_glyph_sprite_at_idx(row, col);
    }

    // Sprite for some custom game glyphs
    world->icon_game = get_glyph_sprite_at_idx(15, 4);
    world->icon_clock = get_glyph_sprite_at_idx(15, 5);
}

static Sprite get_button_sprite_at_idx(int row, int col) {
    Sprite sprite = { 0 };
    sprite.color.r = 1.0f;
    sprite.color.g = 1.0f;
    sprite.color.b = 1.0f;
    sprite.color.a = 1.0f;

    sprite.width = (float)BUTTON_WIDTH * BUTTON_SCALE;
    sprite.height = (float)BUTTON_HEIGHT * BUTTON_SCALE;

    sprite.uv_top = 1.0f - (BUTTON_OFFSET_Y + row * (BUTTON_HEIGHT + BUTTON_GAP_Y)) / (float)HEIGHT;
    sprite.uv_bottom = sprite.uv_top - (float)BUTTON_HEIGHT / (float)HEIGHT;
    sprite.uv_left = (BUTTON_OFFSET_X + col * (BUTTON_WIDTH + BUTTON_GAP_X)) / (float)WIDTH;
    sprite.uv_right = sprite.uv_left + (float)BUTTON_WIDTH / (float)WIDTH;
    return sprite;
}

static void populate_button_sprites(World* world) {
    world->button_new_game = get_button_sprite_at_idx(0, 0);
    world->button_new_game.color.r = 0.55f;
    world->button_new_game.color.g = 0.85f;
    world->button_new_game.color.b = 0.55f;

    world->button_undo = get_button_sprite_at_idx(1, 0);
    world->button_undo.color.r = 0.85f;
    world->button_undo.color.g = 0.55f;
    world->button_undo.color.b = 0.55f;

    world->button_sound = get_button_sprite_at_idx(2, 0);
    world->button_sound.color.r = 0.55f;
    world->button_sound.color.g = 0.55;
    world->button_sound.color.b = 0.85f;
}

void populate_sprites(World* world) {
    populate_card_sprites(world->deck);
    populate_glyph_sprites(world);
    populate_button_sprites(world);
}
