#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <string.h>

#include "constants.h"
#include "controller.h"
#include "render_system.h"
#include "ui_layout.h"
#include "utils.h"
#include "window.h"
#include "world.h"

UIElement compute_state(UIElement *element, bool hovered, bool clicked,
                        bool disabled) {
  UIElement new_element = *element;

  if (element->type == UI_CARD) {
    if (clicked) {
      new_element.meta.card.state = CARD_UI_STATE_SELECTED;
    } else if (hovered) {
      new_element.meta.card.state = CARD_UI_STATE_HOVERED;
    } else {
      new_element.meta.card.state = CARD_UI_STATE_NORMAL;
    }
  } else if (element->type == UI_BUTTON) {
    if (disabled) {
      new_element.meta.button.state = BUTTON_UI_STATE_DISABLED;
    } else if (clicked) {
      new_element.meta.button.state = BUTTON_UI_STATE_SELECTED;
    } else if (hovered) {
      new_element.meta.button.state = BUTTON_UI_STATE_HOVERED;
    } else {
      new_element.meta.button.state = BUTTON_UI_STATE_NORMAL;
    }
  }

  return new_element;
}

static void controller_update_ui_state(GLFWwindow *window, World *world) {
  Controller *controller = &world->controller;

  size_t hit_index = -1;
  ui_get_topmost_hit(&world->ui_elements, controller->mouse, NULL, &hit_index);

  for (size_t i = 0; i < world->ui_elements.size; ++i) {
    UIElement element;
    memcpy(&element, vec_get(&world->ui_elements, i), sizeof(UIElement));

    bool hovered = hit_index == i;
    bool pressed =
        glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool clicked = hovered && pressed;
    bool disabled = false;
    UIElement expected_state =
        compute_state(&element, hovered, clicked, disabled);

    if (memcmp(&element, &expected_state, sizeof(UIElement)) != 0) {
      vec_set(&world->ui_elements, i, &expected_state);
      controller->bake_pending = true;
    }
  }
}

void controller_update(GLFWwindow *window, World *world, float dt) {
  (void)dt;
  Controller *controller = &world->controller;

  if (controller->layout_pending) {
    layout_world(world);
    controller->layout_pending = false;
  }

  if (controller->bake_pending) {
    bake_world(world);
    controller->bake_pending = false;
  }

  controller_update_ui_state(window, world);
  // if (glfwGetKey(window, GLFW_KEY_UP)) {
  // } else if (glfwGetKey(window, GLFW_KEY_DOWN)) {
  // } else if (glfwGetKey(window, GLFW_KEY_LEFT)) {
  // } else if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
  // }
}

void controller_on_close(GLFWwindow *window) {
  World *world = glfwGetWindowUserPointer(window);
  world->running = false;
}

void controller_on_resize(GLFWwindow *window, int width, int height) {
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

void controller_on_key(GLFWwindow *window, int key, int scancode, int action,
                       int mods) {
  (void)scancode;
  (void)mods;

  World *world = glfwGetWindowUserPointer(window);
  Controller *controller = &world->controller;
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_SPACE) {
      controller->debug = !controller->debug;
    }
  }
}

void controller_on_cursor_position(GLFWwindow *window, double x, double y) {
  World *world = glfwGetWindowUserPointer(window);
  Controller *controller = &world->controller;

  controller->mouse_screen.x = (float)x;
  controller->mouse_screen.y = (float)y;

  int width, height;
  window_get_size(window, &width, &height);
  controller->mouse =
      screen_to_world(x, y, width, height, &world->camera);
}
