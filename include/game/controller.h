#pragma once

#include <stdbool.h>

#include "game/game.h"
#include "game/input_action.h"

#include <cglm/struct.h>

typedef struct World World;
typedef struct GLFWwindow GLFWwindow;
typedef struct UIElement UIElement;

typedef struct UIDragState {
    bool dragging;
    vec2s drag_offset;

    SelectionLocation card_location;
    uint8_t card_index;
} UIDragState;

typedef struct Controller {
    vec2s mouse;
    vec2s mouse_screen;
    UIDragState drag_state;
} Controller;

void controller_update(World* world, double dt);

bool controller_handle_card_drop(
    UIElement* dest, SelectionLocation location, uint8_t card_index, World* world);

void controller_undo(World* world);

void controller_new_game(World* world);

void controller_new_game_with_seed(World* world, uint32_t seed);

void controller_handle_input(InputAction action);

void controller_start_drag(World* window);

void controller_update_drag(World* window);

void controller_end_drag(World* window);

void controller_toggle_fullscreen(World* world);

void controller_toggle_help(World* world);

void controller_on_framebuffer_resize(World* world, int width, int height);

void controller_on_cursor_position(World* world, double x, double y);
