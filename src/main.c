#include <stdint.h>

#include "platform/window.h"

#include "rendering/renderer.h"

#include "game/constants.h"
#include "game/input.h"
#include "game/render_system.h"
#include "game/world.h"

void gameloop(GLFWwindow* window) {
    World world = world_init(window);
    Renderer renderer = renderer_init();

    glfwSetWindowUserPointer(window, &world);

    double time = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        glfwSwapBuffers(window);
        glfwWaitEventsTimeout(0.2f); // only 5fps if no events are received

        double dt = glfwGetTime() - time;

        controller_update(&world, dt);

        time = glfwGetTime();

        aclear(); // Clear the arena allocator for the next frame
    }
    afree(); // Free the arena allocator at the end

    renderer_free(&renderer);
    world_free(&world);
}

int main(void) {
    GLFWwindow* window = window_init((WindowConfig) {
        .width = VIRTUAL_WIDTH,
        .height = VIRTUAL_HEIGHT,
        .min_width = GAME_MIN_WIDTH,
        .min_height = GAME_MIN_HEIGHT,
        .max_width = GLFW_DONT_CARE,
        .max_height = GLFW_DONT_CARE,
        .title = GAME_TITLE,
        .vsync = true,
        .fullscreen_monitor = NULL,
        .on_close = NULL,
        .on_window_resize = NULL,
        .on_framebuffer_resize = input_on_framebuffer_resize,
        .on_key = input_on_key,
        .on_mouse_click = input_on_mouse_click,
        .on_cursor_position = input_on_cursor_position,
    });

    gameloop(window);

    window_free(window);
    return 0;
}

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int APIENTRY
wWinMain(_In_ HINSTANCE hinstance, _In_opt_ HINSTANCE opt, _In_ LPWSTR cmd, _In_ int i) {
    (void)hinstance;
    (void)opt;
    (void)cmd;
    (void)i;
    return main();
}
#endif
