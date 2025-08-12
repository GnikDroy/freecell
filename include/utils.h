#pragma once
#include <stdbool.h>

#include <cglm/struct.h>

#include "rendering/camera.h"
#include "rendering/rect.h"

uint64_t time_millis();

bool point_in_rect(float px, float py, Rect rect);

vec2s screen_to_world(
    double mouse_x,
    double mouse_y,
    int screen_width,
    int screen_height,
    Camera* cam
);

float lerp(float a, float b, float t);

float clamp(float value, float min, float max);

float ease_in_out_cubic(float t);
