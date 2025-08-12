#pragma once

#include "rendering/shader.h"
#include "rendering/texture.h"

typedef struct Assets {
    Shader main_shader;
    Texture spritesheet_texture;
} Assets;

Assets assets_init();

void assets_free(Assets* assets);
