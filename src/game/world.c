#include "game/world.h"

#include "core/log.h"

#include "game/assets.h"
#include "game/constants.h"
#include "game/ui_sprites.h"
#include "game/ui_element.h"

const Color BACKGROUND_COLOR = (Color) { 20 / 256.0, 63 / 256.0, 23 / 256.0 };

World world_init(RGFW_window* window) {
    World world = { 0 };
    world.window = window;
    world.game = game_init();
    world.assets = assets_init();

    Camera camera = {
        .view = GLM_MAT4_IDENTITY_INIT,
    };

    glm_ortho(
        0.0f,
        (float)VIRTUAL_WIDTH,
        (float)VIRTUAL_HEIGHT,
        0.0f,
        -1000.0f,
        1.0f,
        camera.projection
    );

    world.camera = camera;

    populate_sprites(&world);

    world.ui_elements = vec_init(sizeof(UIElement));

    world.game_mesh = mesh_init();
    world.game_gpu_mesh = gpu_mesh_init();

    world.sound_enabled = true;
    ma_result result = ma_engine_init(NULL, &world.engine);

    if (result != MA_SUCCESS) {
        log_error("Failed to initialize audio engine: %s", ma_result_description(result));
        exit(EXIT_FAILURE);
    }

    ma_decoder_config decoderConfig = ma_decoder_config_init(ma_format_f32, 0, 0);
    result = ma_decoder_init_memory(
        CARD_MOVE_SOUND,
        CARD_MOVE_SOUND_SIZE,
        &decoderConfig,
        &world.card_move_decoder
    );

    if (result != MA_SUCCESS) {
        log_error("Failed to initialize MP3 decoder: %s", ma_result_description(result));
        exit(EXIT_FAILURE);
    }

    result = ma_sound_init_from_data_source(
        &world.engine,
        &world.card_move_decoder,
        0,
        NULL,
        &world.card_move_sound
    );

    if (result != MA_SUCCESS) {
        log_error("Failed to initialize sound from decoder: %s", ma_result_description(result));
        ma_decoder_uninit(&world.card_move_decoder);
        exit(EXIT_FAILURE);
    }

    world.animation_system = animation_system_init();
    world.show_help = true;

    return world;
}

void world_free(World* world) {
    game_free(&world->game);
    assets_free(&world->assets);

    vec_free(&world->ui_elements);

    gpu_mesh_free(&world->game_gpu_mesh);
    mesh_free(&world->game_mesh);

    ma_sound_uninit(&world->card_move_sound);
    ma_decoder_uninit(&world->card_move_decoder);

    ma_engine_uninit(&world->engine);

    animation_system_free(&world->animation_system);
}
