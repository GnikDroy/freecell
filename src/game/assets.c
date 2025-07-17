#include "game/assets.h"

#include "rendering/shader.h"
#include "rendering/texture.h"

#include "game/ui_sprites.h"
#include "game/constants.h"

Assets assets_init() {
    Assets assets;

    assets.spritesheet
        = image_load_from_memory((const uint8_t*)MAIN_SPRITESHEET, MAIN_SPRITESHEET_SIZE, 4);

    assets.spritesheet_texture = texture_init(&assets.spritesheet);

    assets.main_shader = shader_init(MAIN_VERTEX_SHADER_SOURCE, MAIN_FRAGMENT_SHADER_SOURCE);

    if (assets.main_shader == 0) {
        exit(EXIT_FAILURE);
    }

    assets.background_shader
        = shader_init(BACKGROUND_VERTEX_SHADER_SOURCE, BACKGROUND_FRAGMENT_SHADER_SOURCE);

    if (assets.background_shader == 0) {
        exit(EXIT_FAILURE);
    }

    return assets;
}

void assets_free(Assets* assets) {
    image_free(&assets->spritesheet);
    shader_free(&assets->main_shader);
    shader_free(&assets->background_shader);
    texture_free(&assets->spritesheet_texture);
}
