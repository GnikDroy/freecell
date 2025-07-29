#pragma once

typedef struct GLFWwindow GLFWwindow;

typedef enum InputActionType {
    INPUT_ACTION_NONE,
    INPUT_ACTION_NEW_GAME,
    INPUT_ACTION_UNDO,
    INPUT_ACTION_TOGGLE_FULLSCREEN,
    INPUT_ACTION_CLICK,
    INPUT_ACTION_START_DRAG,
    INPUT_ACTION_END_DRAG,
    INPUT_ACTION_SMART_MOVE,
    INPUT_ACTION_POINTER_MOVE,
    INPUT_ACTION_FRAMEBUFFER_RESIZE,

    INPUT_ACTION_AUTOCOMPLETEABLE_GAME,
    INPUT_ACTION_FILL_CASCADES,
} InputActionType;

typedef struct InputAction {
    InputActionType type;
    GLFWwindow* window;
    union {
        struct {
            double x;
            double y;
        } pointer_move;

        struct {
            int width;
            int height;
        } framebuffer_resize;
    } data;
} InputAction;

