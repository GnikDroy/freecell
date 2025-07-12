#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "callbacks.h"
#include "constants.h"
#include "renderer.h"
#include "render_system.h"
#include "window.h"
#include "world.h"

void handle_events(GLFWwindow *window, World *world) {
  (void)window;
  (void)world;

  // if (glfwGetKey(window, GLFW_KEY_UP)) {
  // } else if (glfwGetKey(window, GLFW_KEY_DOWN)) {
  // } else if (glfwGetKey(window, GLFW_KEY_LEFT)) {
  // } else if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
  // }
}

void gameloop(GLFWwindow *window) {
  World world = world_init();
  Renderer renderer = renderer_init(window);

  double start_time = glfwGetTime();

  while (running) {
    glfwSwapBuffers(window);
    glfwPollEvents();

    double dt = glfwGetTime() - start_time;
    (void)dt;

    renderer_clear();
    renderer_draw(&world);
    handle_events(window, &world);
    start_time = glfwGetTime();
  }

  renderer_free(&renderer);
  world_free(&world);
}

int main(void) {
  srand((uint32_t)time(NULL));

  Game game = game_init();
  game_free(&game);

  GLFWwindow *window = window_init((WindowConfig){
      .width = VIRTUAL_WIDTH,
      .height = VIRTUAL_HEIGHT,
      .min_width = GAME_MIN_WIDTH,
      .min_height = GAME_MIN_HEIGHT,
      .max_width = GLFW_DONT_CARE,
      .max_height = GLFW_DONT_CARE,
      .title = GAME_TITLE,
      .vsync = true,
      .on_close = on_close,
      .on_window_resize = on_resize,
      .on_mouse_click = NULL,
  });

  gameloop(window);

  window_free(window);
  return 0;
}

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int APIENTRY wWinMain(_In_ HINSTANCE hinstance, _In_opt_ HINSTANCE opt,
                      _In_ LPWSTR cmd, _In_ int i) {
  (void)hinstance;
  (void)opt;
  (void)cmd;
  (void)i;
  return main();
}
#endif
