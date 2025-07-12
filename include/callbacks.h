#pragma once
#include <math.h>
#include <stdbool.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "constants.h"
#include "world.h"

struct GLFWwindow;

inline static void on_close(GLFWwindow *window) {
  (void)window;
  World* world = glfwGetWindowUserPointer(window);
  world->running = false;
}

inline static void on_resize(GLFWwindow *window, int width, int height) {
  (void)window;
  float aspectVirtual = (float)VIRTUAL_WIDTH / VIRTUAL_HEIGHT;
  float aspectWindow = (float)width / height;

  int viewportWidth, viewportHeight;
  int viewportX, viewportY;

  if (aspectWindow > aspectVirtual) {
    viewportHeight = height;
    viewportWidth = (int)(viewportHeight * aspectVirtual);
  } else {
    viewportWidth = width;
    viewportHeight = (int)(viewportWidth / aspectVirtual);
  }

  viewportX = (width - viewportWidth) / 2;
  viewportY = (height - viewportHeight) / 2;

  glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
}
