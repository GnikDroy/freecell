#include <math.h>
#include <stdint.h>

#include <glad/glad.h>

#include <RGFW.h>

#include "core/log.h"
#include "game/controller.h"
#include "game/input_action.h"
#include "platform/window.h"

const float CLICK_THRESHOLD = 5.0f;
static int mouse_press_x = 0.0;
static int mouse_press_y = 0.0;

InputAction input_on_key(RGFW_window* window, World* world, RGFW_keyEvent event) {
    InputAction ia = { 0 };
    ia.world = world;

    if (event.type == RGFW_keyPressed && !event.repeat) {
        RGFW_key key = event.value;
        if (key == RGFW_escape || (key == RGFW_z && (event.mod & RGFW_modControl))) {
            ia.type = INPUT_ACTION_UNDO;
        } else if (key == RGFW_F2) {
            ia.type = INPUT_ACTION_NEW_GAME;
        } else if (key == RGFW_F1) {
            ia.type = INPUT_ACTION_TOGGLE_HELP;
        } else if (key == RGFW_F11) {
            ia.type = INPUT_ACTION_TOGGLE_FULLSCREEN;
        } else if (key == RGFW_q) {
            window_queue_close(window);
        } else if (key == RGFW_v && (event.mod & RGFW_modControl)) {
            const char* clipboard_content = window_get_clipboard(window);
            if (!clipboard_content) {
                ia.type = INPUT_ACTION_NONE;
                return ia;
            }

            char* endptr = NULL;
            long seed = strtol(clipboard_content, &endptr, 10);
            if (endptr == clipboard_content) {
                ia.type = INPUT_ACTION_NONE;
                return ia;
            }

            ia.type = INPUT_ACTION_NEW_GAME_WITH_SEED;
            ia.data.new_game_with_seed.seed = seed;
        } else if (key == RGFW_m) {
            ia.type = INPUT_ACTION_AUTOCOMPLETEABLE_GAME;
        } else if (key == RGFW_c) {
            ia.type = INPUT_ACTION_FILL_CASCADES;
        }
    }
    return ia;
}

InputAction input_on_mouse_action(RGFW_window* window, World* world, RGFW_mouseButtonEvent event) {
    InputAction ia = { 0 };
    ia.world = world;

    if (event.value == RGFW_mouseLeft) {
        if (event.type == RGFW_mouseButtonPressed) {
            window_get_cursor_position(window, &mouse_press_x, &mouse_press_y);
            ia.type = INPUT_ACTION_START_DRAG;
        } else if (event.type == RGFW_mouseButtonReleased) {
            int x, y;
            window_get_cursor_position(window, &mouse_press_x, &mouse_press_y);
            int dx = x - mouse_press_x;
            int dy = y - mouse_press_y;
            int dist_sq = dx * dx + dy * dy;

            // Handle click if the distance is within the threshold
            if (dist_sq < CLICK_THRESHOLD * CLICK_THRESHOLD) {
                InputAction press_action = { .world = world, .type = INPUT_ACTION_CLICK };
                controller_handle_input(press_action);
            }

            ia.type = INPUT_ACTION_END_DRAG;
        }
    } else if (event.value == RGFW_mouseRight) {
        if (event.type == RGFW_mouseButtonReleased) {
            ia.type = INPUT_ACTION_SMART_MOVE;
        }
    }

    return ia;
}

InputAction input_on_framebuffer_resize(RGFW_window* window, World* world, RGFW_commonEvent event) {
    InputAction ia = { 0 };
    ia.world = world;

    ia.type = INPUT_ACTION_FRAMEBUFFER_RESIZE;
    window_get_size(
        window,
        (int*)&ia.data.framebuffer_resize.width,
        (int*)&ia.data.framebuffer_resize.height
    );
    return ia;
}

InputAction input_on_cursor_position(RGFW_window* window, World* world, RGFW_mousePosEvent event) {
    InputAction ia = { 0 };
    ia.world = world;

    ia.type = INPUT_ACTION_POINTER_MOVE;
    ia.data.pointer_move.x = event.x;
    ia.data.pointer_move.y = event.y;

    return ia;
}

bool input_get_input_action(RGFW_window* window, World* world, InputAction* ia) {
    RGFW_event event;
    if (window_get_event(window, &event) && event.type != RGFW_eventNone) {
        if (ia != NULL) {
            if (event.type == RGFW_keyPressed || event.type == RGFW_keyReleased) {
                *ia = input_on_key(window, world, event.key);
            } else if (event.type == RGFW_mouseButtonPressed
                       || event.type == RGFW_mouseButtonReleased) {
                *ia = input_on_mouse_action(window, world, event.button);
            } else if (event.type == RGFW_windowResized) {
                *ia = input_on_framebuffer_resize(window, world, event.common);
            } else if (event.type == RGFW_mousePosChanged) {
                *ia = input_on_cursor_position(window, world, event.mouse);
            }
        }
        return true;
    }
    return false;
}
