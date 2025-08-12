#pragma once

#include "rendering/color.h"
#include "rendering/rect.h"

typedef struct Sprite {
    float uv_top, uv_bottom, uv_left, uv_right;
    float x, y, z;
    float width, height;
    float rotation;
    Color color;
} Sprite;

inline Rect compute_hitbox(Sprite* sprite) {
    float height_grace = 1.0f;
    float width_grace = 1.2f;

    Rect hitbox = {
        .height = sprite->height * height_grace,
        .width = sprite->width * width_grace,
        .x = sprite->x,
        .y = sprite->y,
    };

    return hitbox;
}
