#pragma once

#include "rendering/color.h"

typedef struct Sprite {
    float uv_top, uv_bottom, uv_left, uv_right;
    float x, y, z;
    float width, height;
    Color color;
} Sprite;
