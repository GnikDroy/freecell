#include <stdlib.h>

#include "core/log.h"
#include "platform/window.h"

#define RGFW_EXPORT
#define RGFW_IMPLEMENTATION
#define RGFW_OPENGL
#include <RGFW.h>

RGFW_window* window_init(WindowConfig config) {
    RGFW_window* window = RGFW_createWindow(
        config.title,
        0,
        0,
        config.width,
        config.height,
        RGFW_windowAllowDND | RGFW_windowCenter | RGFW_windowScaleToMonitor | RGFW_windowOpenGL
    );
    if (!window) {
        log_error("Failed to create RGFW window\n");
        exit(EXIT_FAILURE);
    }
    RGFW_window_makeCurrentContext_OpenGL(window);
    if (config.max_width != -1 && config.max_height != -1) {
        RGFW_window_setMaxSize(window, config.max_width, config.max_height);
    }
    RGFW_window_setMinSize(window, config.min_width, config.min_height);
    RGFW_window_swapInterval_OpenGL(window, config.vsync);
    if (!gladLoadGLLoader((GLADloadproc)RGFW_getProcAddress_OpenGL)) {
        log_error("Failed to initialize GLAD.\n");
        exit(EXIT_FAILURE);
    }

    glViewport(0, 0, (int32_t)config.width, (int32_t)config.height);
    return window;
}

void window_free(RGFW_window* window) { RGFW_window_close(window); }

void window_swap_buffers(RGFW_window* window) { RGFW_window_swapBuffers_OpenGL(window); }

void window_get_size(RGFW_window* window, int* width, int* height) {
    RGFW_window_getSize(window, width, height);
}

const char* window_get_clipboard(RGFW_window* window) { return RGFW_readClipboard(NULL); }

void window_toggle_fullscreen(RGFW_window* window) {
    RGFW_window_setFullscreen(window, !RGFW_window_isFullscreen(window));
}

void window_maximize(RGFW_window* window) { RGFW_window_maximize(window); }

void window_get_cursor_position(RGFW_window* window, int* x, int* y) {
    RGFW_window_getMouse(window, x, y);
}

bool window_is_mouse_pressed(RGFW_window* window, uint8_t button) {
    return RGFW_window_isMousePressed(window, button);
}

void window_queue_close(RGFW_window* window) { RGFW_window_setShouldClose(window, RGFW_TRUE); }

bool window_is_queued_to_close(RGFW_window* window) {
    return RGFW_window_shouldClose(window) == RGFW_TRUE;
}

bool window_get_event(RGFW_window* window, RGFW_event* event) {
    return RGFW_window_checkEvent(window, event);
}

void event_wait_timeout(uint32_t waitMS) {
    RGFW_waitForEvent(waitMS);
}
