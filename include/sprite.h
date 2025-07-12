#pragma once

typedef struct {
  float r, g, b, a;
} Color;

typedef struct {
  float x, y, width, height;
} Rect;

typedef struct {
  float uv_top, uv_bottom, uv_left, uv_right;
  float x, y;
  float width, height;
  Color color;
} Sprite;
