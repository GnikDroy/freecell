#include "platform/window.h"
#include <stdint.h>

#include <RGFW.h>

#include "rendering/renderer.h"

#include "game/constants.h"
#include "game/render_system.h"
#include "game/world.h"
#include "utils.h"

void gameloop(RGFW_window* window) {
    // Clear screen immediately to avoid white flash of blindness
    renderer_init();
    renderer_clear(BACKGROUND_COLOR);
    window_swap_buffers(window);

    // Maximize window and resize framebuffer
    // window_maximize(window);
    renderer_clear(BACKGROUND_COLOR);
    window_swap_buffers(window);

    World world = world_init(window);

    int width, height;
    window_get_size(window, &width, &height);
    controller_on_framebuffer_resize(&world, width, height);

    double time = time_millis_from_start() / 1000.0;
    while (!window_is_queued_to_close(window)) {
        window_swap_buffers(window);
        if (!world.controller.screen_needs_update) {
            event_wait_timeout(1000 / 5.0);
        }
        double dt = time_millis_from_start() / 1000.0 - time;
        controller_update(&world, dt);
        time = time_millis_from_start() / 1000.0;

        aclear(); // Clear the arena allocator for the next frame
    }
    afree(); // Free the arena allocator at the end
    world_free(&world);
}

int main(void) {
    RGFW_window* window = window_init((WindowConfig) {
        .width = VIRTUAL_WIDTH,
        .height = VIRTUAL_HEIGHT,
        .min_width = GAME_MIN_WIDTH,
        .min_height = GAME_MIN_HEIGHT,
        .max_width = -1,
        .max_height = -1,
        .title = GAME_TITLE,
        .vsync = true,
    });

    gameloop(window);

    window_free(window);
    return 0;
}

#ifdef _WIN32
typedef wchar_t WCHAR;
typedef WCHAR* LPWSTR;
typedef void* HINSTANCE;
typedef int INT;
#define APIENTRY __stdcall

// wWinMain prototype
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, INT nCmdShow);

int APIENTRY
wWinMain(_In_ HINSTANCE hinstance, _In_opt_ HINSTANCE opt, _In_ LPWSTR cmd, _In_ int i) {
    (void)hinstance;
    (void)opt;
    (void)cmd;
    (void)i;
    return main();
}
#endif
