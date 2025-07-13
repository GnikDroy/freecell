#pragma once

#include <stdbool.h>
#include <cglm/struct.h>

typedef struct World World;
typedef struct GLFWwindow GLFWwindow;

typedef struct Controller {
  vec2s mouse;
  vec2s mouse_screen;
  bool bake_pending;
  bool layout_pending;
  bool debug;
} Controller;

void controller_update(GLFWwindow *window, World *world, float dt);

void controller_on_close(GLFWwindow *window);

void controller_on_resize(GLFWwindow *window, int width, int height);

void controller_on_key(GLFWwindow* window, int key, int scancode, int action, int mods);

void controller_on_cursor_position(GLFWwindow* window, double x, double y);
