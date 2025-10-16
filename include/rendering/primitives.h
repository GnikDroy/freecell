#pragma once
#include "color.h"

typedef struct Line {
    float x1, y1, z1;
    float x2, y2, z2;
    float thickness;
    Color color;
} Line;

typedef struct Triangle {
    float x1, y1, z1;
    float x2, y2, z2;
    float x3, y3, z3;
    Color color;
} Triangle;

typedef struct Quad {
    float x, y, z;
    float width, height;
    float rotation;
    Color color;
} Quad;

typedef struct Ellipse_ {
    float x, y, z;
    float radius_x, radius_y;
    int segments;
    float rotation;
    Color color;
} Ellipse_;

typedef struct Circle {
    float x, y, z;
    float radius;
    int segments;
    Color color;
} Circle;
