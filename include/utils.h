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
