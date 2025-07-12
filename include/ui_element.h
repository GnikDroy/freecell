#pragma once
#include "game.h"
#include "sprite.h"

typedef enum {
  UI_CARD,
  UI_BUTTON,
} UIType;

typedef struct {
  Card card;
  SelectionLocation selection_location;
  int card_index;
} CardUIMeta;

typedef struct {
  int id;
} ButtonUIMeta;

typedef struct {
  float x, y, width, height;
} Rect;

typedef struct {
  UIType type;
  Sprite sprite;
  Rect hitbox;
  union {
    CardUIMeta card;
    ButtonUIMeta button;
  } meta;
} UIElement;
