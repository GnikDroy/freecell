#pragma once

#ifndef __EMSCRIPTEN__
#include <glad/glad.h>
#else
#include <GLES3/gl3.h>
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct RGFW_window RGFW_window;
typedef union RGFW_event RGFW_event;

typedef struct WindowConfig {
    size_t width;
    size_t height;
    int32_t min_width;
    int32_t min_height;
    int32_t max_width;
    int32_t max_height;
    const char* title;
    bool vsync;
} WindowConfig;

RGFW_window* window_init(WindowConfig config);

void window_free(RGFW_window* window);

void window_swap_buffers(RGFW_window* window);

void window_get_size(RGFW_window* window, int* width, int* height);

const char* window_get_clipboard(RGFW_window* window);

void window_toggle_fullscreen(RGFW_window* window);

void window_maximize(RGFW_window* window);

void window_get_cursor_position(RGFW_window* window, int* x, int* y);

bool window_is_mouse_pressed(RGFW_window* window, uint8_t button);

void window_queue_close(RGFW_window*);

bool window_is_queued_to_close(RGFW_window* window);

bool window_get_event(RGFW_window* window, RGFW_event* event);

void event_wait_timeout(uint32_t waitMS);
