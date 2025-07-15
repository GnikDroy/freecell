#include <math.h>
#include <stdint.h>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "game/input_action.h"
#include "game/controller.h"

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
            ia.type = INPUT_ACTION_COMPLETE_GAME;
        } else if (key == GLFW_KEY_F) {
            ia.type = INPUT_ACTION_TOGGLE_FULLSCREEN;
        } else if (key == GLFW_KEY_D) {
            ia.type = INPUT_ACTION_TOGGLE_DEBUG;
        } else if (key == GLFW_KEY_C) {
            ia.type = INPUT_ACTION_FILL_CASCADES;
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
            ia.type = INPUT_ACTION_START_DRAG;
        } else {
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
