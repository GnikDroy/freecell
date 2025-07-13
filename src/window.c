#include "window.h"
#include "log.h"
#include <stdlib.h>

void glfwErrCallback(int error_code, const char *msg) {
  log_error("GLFW Error %d : %s", error_code, msg);
}

GLFWwindow *window_init(WindowConfig config) {
  glfwInit();
  glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
  GLFWwindow *window = glfwCreateWindow((int)config.width, (int)config.height,
                                        config.title, NULL, NULL);
  if (!window) {
    glfwTerminate();
  }
  glfwSetErrorCallback(glfwErrCallback);
  glfwSetWindowSizeLimits(window, config.min_width, config.min_height,
                          config.max_width, config.max_height);
  if (config.on_close)
    glfwSetWindowCloseCallback(window, config.on_close);
  if (config.on_window_resize)
    glfwSetWindowSizeCallback(window, config.on_window_resize);
  if (config.on_key)
    glfwSetKeyCallback(window, config.on_key);
  if (config.on_mouse_click)
    glfwSetMouseButtonCallback(window, config.on_mouse_click);
  if (config.on_cursor_position)
    glfwSetCursorPosCallback(window, config.on_cursor_position);

  glfwMakeContextCurrent(window);
  glfwSwapInterval(config.vsync);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    log_error("Failed to initialize GLAD.\n");
    exit(EXIT_FAILURE);
  }

  glViewport(0, 0, (int32_t)config.width, (int32_t)config.height);
  glEnable(GL_MULTISAMPLE);
  return window;
}

void window_free(GLFWwindow *window) {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void window_get_size(GLFWwindow *window, int *width, int *height) {
  glfwGetWindowSize(window, width, height);
}

void window_get_cursor_position(GLFWwindow *window, double *x, double *y) {
  glfwGetCursorPos(window, x, y);
}
