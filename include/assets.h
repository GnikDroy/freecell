#pragma once

#include "image.h"
#include "mesh.h"
#include "shader.h"
#include "sprite.h"
#include "texture.h"

typedef struct {
  Image spritesheet;
  Sprite deck[53];
  Shader main_shader;
  Texture spritesheet_texture;
  GPUMesh game_mesh;
} Assets;

Assets assets_init();

void assets_free(Assets *assets);
