#pragma once
#include <stdbool.h>

#include <cglm/struct.h>

#include "rendering/rect.h"

#include "game/ui_element.h"

typedef struct World World;
typedef struct Sprite Sprite;
typedef struct Vector Vector;
typedef struct UIElement UIElement;

Rect compute_hitbox(Sprite* sprite);

bool ui_find_in_layout(Vector* vec, SelectionLocation location, uint32_t card_index,
    UIElement* dest, size_t* dest_idx);

void ui_push_freecells(Vector* vec, World* world);

void ui_push_foundation(Vector* vec, World* world);

void ui_push_cascade(Vector* vec, World* world, int cascade_index, int x_offset);

void ui_push_cascades(Vector* vec, World* world);

void ui_push_world(Vector* vec, World* world);

bool ui_get_topmost_hit(Vector* ui_elements, vec2s mouse, UIElement* topmost, size_t* index);
