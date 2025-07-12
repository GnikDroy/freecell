#pragma once

#include "color.h"

typedef struct {
  float uv_top, uv_bottom, uv_left, uv_right;
  float x, y;
  float width, height;
  Color color;
} Sprite;
