#pragma once

#include <stdbool.h>

#include <cglm/struct.h>

typedef struct World World;
typedef struct GLFWwindow GLFWwindow;

typedef struct UIDragState {
  bool dragging;
  size_t ui_elements_index;
  vec2s drag_offset;
  vec2s original_position;
} UIDragState;

typedef struct Controller {
  vec2s mouse;
  vec2s mouse_screen;
  bool bake_pending;
  bool layout_pending;
  bool debug;
  UIDragState drag_state;
} Controller;

void controller_update(GLFWwindow *window, World *world, float dt);

void controller_on_framebuffer_resize(GLFWwindow *window, int width,
                                      int height);

void controller_on_key(GLFWwindow *window, int key, int scancode, int action,
                       int mods);

void controller_on_cursor_position(GLFWwindow *window, double x, double y);

void controller_on_mouse_click(GLFWwindow *window, int code, int state,
                               int mods);
