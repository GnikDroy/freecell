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

  void (*on_mouse_click)(GLFWwindow *, int code, int state, int mods);
} WindowConfig;

GLFWwindow *window_init(WindowConfig config);

void window_free(GLFWwindow *window);

void glfwErrCallback(int error_code, const char *msg);
