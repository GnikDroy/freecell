#include <math.h>
#include <stdint.h>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "game/controller.h"
#include "game/input_action.h"
#include "platform/window.h"

const float CLICK_THRESHOLD = 5.0f;
static double mouse_press_x = 0.0;
static double mouse_press_y = 0.0;

void input_on_key(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)scancode;
    (void)mods;

    InputAction ia = { 0 };
    ia.window = window;

    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) {
            ia.type = INPUT_ACTION_UNDO;
        } else if (key == GLFW_KEY_N) {
            ia.type = INPUT_ACTION_NEW_GAME;
        } else if (key == GLFW_KEY_M) {
            ia.type = INPUT_ACTION_AUTOCOMPLETEABLE_GAME;
        } else if (key == GLFW_KEY_F) {
            ia.type = INPUT_ACTION_TOGGLE_FULLSCREEN;
        } else if (key == GLFW_KEY_C) {
            ia.type = INPUT_ACTION_FILL_CASCADES;
        } else if (key == GLFW_KEY_Q) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        } else if (key == GLFW_KEY_V && (mods & GLFW_MOD_CONTROL)) {
            const char* clipboard_content = window_get_clipboard(window);
            if (!clipboard_content) {
                return;
            }

            char* endptr = NULL;
            long seed = strtol(clipboard_content, &endptr, 10);
            if (endptr == clipboard_content) {
                return;
            }

            ia.type = INPUT_ACTION_NEW_GAME_WITH_SEED;
            ia.data.new_game_with_seed.seed = seed;
        }
    }
    controller_handle_input(ia);
}

void input_on_mouse_click(GLFWwindow* window, int code, int state, int mods) {
    (void)mods;
    InputAction ia = { 0 };
    ia.window = window;

    if (code == GLFW_MOUSE_BUTTON_LEFT) {
        if (state == GLFW_PRESS) {
            glfwGetCursorPos(window, &mouse_press_x, &mouse_press_y);
            ia.type = INPUT_ACTION_START_DRAG;
        } else {
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            double dx = x - mouse_press_x;
            double dy = y - mouse_press_y;
            double dist_sq = dx * dx + dy * dy;

            // Handle click if the distance is within the threshold
            if (dist_sq < CLICK_THRESHOLD * CLICK_THRESHOLD) {
                InputAction press_action = { .window = window, .type = INPUT_ACTION_CLICK };
                controller_handle_input(press_action);
            }

            ia.type = INPUT_ACTION_END_DRAG;
        }
    } else if (code == GLFW_MOUSE_BUTTON_RIGHT) {
        if (state == GLFW_RELEASE) {
            ia.type = INPUT_ACTION_SMART_MOVE;
        }
    }

    controller_handle_input(ia);
}

void input_on_framebuffer_resize(GLFWwindow* window, int width, int height) {
    InputAction ia = { 0 };
    ia.window = window;

    ia.type = INPUT_ACTION_FRAMEBUFFER_RESIZE;
    ia.data.framebuffer_resize.width = width;
    ia.data.framebuffer_resize.height = height;

    controller_handle_input(ia);
}

void input_on_cursor_position(GLFWwindow* window, double x, double y) {
    InputAction ia = { 0 };
    ia.window = window;

    ia.type = INPUT_ACTION_POINTER_MOVE;
    ia.data.pointer_move.x = x;
    ia.data.pointer_move.y = y;

    controller_handle_input(ia);
}
