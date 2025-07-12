#pragma once

#include "renderer.h"
#include "sprite.h"

struct World;

void renderer_render_sprite(Sprite sprite, Mesh* mesh);

void renderer_draw(Renderer renderer, World *world);
