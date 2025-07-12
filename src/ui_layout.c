#include "ui_layout.h"
#include "constants.h"

Rect compute_hitbox(Sprite *sprite) {
  Rect hitbox = {
      .height = sprite->height,
      .width = sprite->width,
      .x = sprite->x,
      .y = sprite->y,
  };

  return hitbox;
}

void ui_push_freecells(Vector *vec, World *world) {
  Sprite *deck = world->deck;
  Freecell *freecell = &world->game.freecell;

  const int MARGIN_Y = 30;
  const int MARGIN_X = 30;
  const int GAP = 15;

  for (int i = 0; i < 4; i++) {
    Card card = freecell->reserve[i];
    Sprite sprite = deck[card];

    if (card == NONE) {
      sprite.color.a = 0.5f;
    }

    sprite.x = (sprite.width + GAP) * i + sprite.width / 2.f + MARGIN_X;
    sprite.y = sprite.height / 2.f + MARGIN_Y;

    UIElement ui_element = {
        .type = UI_CARD,
        .sprite = sprite,
        .hitbox = compute_hitbox(&sprite),
        .meta.card =
            {
                .card = card,
                .selection_location = RESERVE_1 + i,
                .card_index = i,
            },
    };

    vec_push_back(vec, &ui_element);
  }
}

void ui_push_foundation(Vector *vec, World *world) {
  Sprite *deck = world->deck;
  Freecell *freecell = &world->game.freecell;

  const int MARGIN_Y = 30;
  const int MARGIN_X = 30;
  const int GAP = 15;

  for (int i = 0; i < 4; i++) {
    Card card = freecell->foundation[i];
    Sprite sprite = deck[card];
    if (card == NONE) {
      sprite = deck[ACE_SPADES + 13 * i];
      sprite.color.a = 0.5f;
    }
    sprite.x = (sprite.width + GAP) * i + sprite.width / 2.f + MARGIN_X;
    sprite.x = VIRTUAL_WIDTH - sprite.x;
    sprite.y = sprite.height / 2.f + MARGIN_Y;

    UIElement ui_element = {
        .type = UI_CARD,
        .sprite = sprite,
        .hitbox = compute_hitbox(&sprite),
        .meta.card =
            {
                .card = card,
                .selection_location = FOUNDATION_SPADES + i,
                .card_index = i,
            },
    };

    vec_push_back(vec, &ui_element);
  }
}

void ui_push_cascade(Vector *vec, World *world, int cascade_index,
                     int x_offset) {
  Freecell *freecell = &world->game.freecell;
  Cascade *cascade = &freecell->cascade[cascade_index];
  Sprite *deck = world->deck;

  const int MARGIN_Y = deck[NONE].height * 2;
  const int OVERLAP = 20;

  for (int j = 0; j < cascade->size; j++) {
    Card card = cascade->cards[j];
    Sprite sprite = deck[card];
    sprite.x = x_offset;
    sprite.y = sprite.height / 2.f + MARGIN_Y + OVERLAP * j;

    UIElement ui_element = {
        .type = UI_CARD,
        .sprite = sprite,
        .hitbox = compute_hitbox(&sprite),
        .meta.card =
            {
                .card = card,
                .selection_location = CASCADE_1 + cascade_index,
                .card_index = j,
            },
    };

    vec_push_back(vec, &ui_element);
  }
}

void ui_push_cascades(Vector *vec, World *world) {
  Sprite *deck = world->deck;

  const int CASCADE_COUNT = 8;
  const int GAP = 15;

  const float total_width =
      CASCADE_COUNT * deck[NONE].width + (CASCADE_COUNT - 1) * GAP;
  for (int i = 0; i < CASCADE_COUNT; i++) {
    int x_offset = (VIRTUAL_WIDTH - total_width) / 2.0f +
                   i * (deck[NONE].width + GAP) + deck[NONE].width / 2.f;
    ui_push_cascade(vec, world, i, x_offset);
  }
}

void ui_push_world(Vector *vec, World *world) {
  ui_push_freecells(vec, world);
  ui_push_foundation(vec, world);
  ui_push_cascades(vec, world);
}
