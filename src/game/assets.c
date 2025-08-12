#include "game/assets.h"

#include "rendering/image.h"
#include "rendering/shader.h"
#include "rendering/texture.h"

#include "game/constants.h"
#include "game/ui_sprites.h"

Assets assets_init() {
    Assets assets;

    Image spritesheet
        = image_load_from_memory((const uint8_t*)MAIN_SPRITESHEET, MAIN_SPRITESHEET_SIZE, 4);
    assets.spritesheet_texture = texture_init(&spritesheet);
    image_free(&spritesheet);

    assets.main_shader = shader_init(MAIN_VERTEX_SHADER_SOURCE, MAIN_FRAGMENT_SHADER_SOURCE);

    if (assets.main_shader == 0) {
        exit(EXIT_FAILURE);
    }

    return assets;
}

void assets_free(Assets* assets) {
    texture_free(&assets->spritesheet_texture);
    shader_free(&assets->main_shader);
}
