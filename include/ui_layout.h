#pragma once
#include "ui_element.h"
#include "world.h"

Rect compute_hitbox(Sprite *sprite) ;

void ui_push_freecells(Vector *vec, World *world);

void ui_push_foundation(Vector *vec, World *world);

void ui_push_cascade(Vector *vec, World *world, int cascade_index, int x_offset);

void ui_push_cascades(Vector *vec, World *world);

void ui_push_world(Vector *vec, World *world);
