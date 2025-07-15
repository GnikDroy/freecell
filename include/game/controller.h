#pragma once

#include <stdbool.h>

#include "core/game.h"
#include "game/input_action.h"

#include <cglm/struct.h>

typedef struct World World;
typedef struct GLFWwindow GLFWwindow;
typedef struct UIElement UIElement;

typedef struct UIDragState {
    bool dragging;
    vec2s drag_offset;
    vec2s original_position;

    SelectionLocation card_location;
    uint8_t card_index;
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

void controller_start_drag(GLFWwindow* window, World* world);

void controller_end_drag(GLFWwindow*, World* world);

bool controller_handle_card_drop(
    UIElement* dest, SelectionLocation location, uint8_t card_index, World* world);

void controller_handle_input(InputAction action);

void controller_on_framebuffer_resize(GLFWwindow* window, int width, int height);

void controller_on_cursor_position(GLFWwindow* window, double x, double y);

void controller_undo(GLFWwindow* window, World* world);

void controller_new_game(GLFWwindow* window, World* world);

void controller_toggle_fullscreen(GLFWwindow* window, World* world);

void controller_toggle_debug(GLFWwindow* window, World* world);

void controller_complete_game(GLFWwindow* window, World* world);

void controller_fill_cascades(GLFWwindow* window, World* world);
