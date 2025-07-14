#pragma once

#include "rendering/image.h"
#include "rendering/shader.h"
#include "rendering/texture.h"

typedef struct Assets {
    Image spritesheet;
    Shader main_shader;
    Shader background_shader;
    Texture spritesheet_texture;
} Assets;

Assets assets_init();

void assets_free(Assets* assets);
