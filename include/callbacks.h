#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>

struct GLFWwindow;

bool running = true;

inline static void on_close(GLFWwindow *window) {
  (void)window;
  running = false;
}

inline static void on_resize(GLFWwindow* window, int width, int height) {
    (void) window;
    glViewport(0, 0, width, height);
}
