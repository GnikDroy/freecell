#pragma once
#include <stdbool.h>

#include <cglm/struct.h>

#include "rect.h"
#include "ui_element.h"

typedef struct World World;
typedef struct Sprite Sprite;
typedef struct Vector Vector;
typedef struct UIElement UIElement;

Rect compute_hitbox(Sprite *sprite);

void ui_push_freecells(Vector *vec, World *world);

void ui_push_foundation(Vector *vec, World *world);

void ui_push_cascade(Vector *vec, World *world, int cascade_index,
                     int x_offset);

void ui_push_cascades(Vector *vec, World *world);

void ui_push_world(Vector *vec, World *world);

bool ui_get_topmost_hit(Vector *ui_elements, vec2s mouse, UIElement *topmost,
                        size_t *index);
