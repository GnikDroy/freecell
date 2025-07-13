#pragma once

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <stdbool.h>
#include <stdint.h>

typedef struct WindowConfig_t {
  size_t width;
  size_t height;
  size_t min_width;
  size_t min_height;
  size_t max_width;
  size_t max_height;
  const char *title;
  bool vsync;

  void (*on_close)(GLFWwindow *);
  void (*on_window_resize)(GLFWwindow *, int x, int y);
  void (*on_framebuffer_resize)(GLFWwindow *, int width, int height);
  void (*on_cursor_position)(GLFWwindow *, double x, double y);
  void (*on_mouse_click)(GLFWwindow *, int code, int state, int mods);
  void (*on_key)(GLFWwindow *, int key, int scancode, int action, int mods);
} WindowConfig;

GLFWwindow *window_init(WindowConfig config);

void window_free(GLFWwindow *window);

void window_get_size(GLFWwindow *window, int *width, int *height);

void window_get_cursor_position(GLFWwindow *window, double *x, double *y);
