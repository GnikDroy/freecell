#pragma once

#include "game/input_action.h"
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

void controller_update(GLFWwindow* window, World* world, double dt);

void controller_handle_input(InputAction action);

void controller_on_framebuffer_resize(GLFWwindow* window, int width, int height);

void controller_on_cursor_position(GLFWwindow* window, double x, double y);

void controller_undo(GLFWwindow* window, World* world);

void controller_new_game(GLFWwindow* window, World* world);

void controller_toggle_fullscreen(GLFWwindow* window, World* world);

void controller_toggle_debug(GLFWwindow* window, World* world);

void controller_complete_game(GLFWwindow* window, World* world);

void controller_fill_cascades(GLFWwindow* window, World* world);
