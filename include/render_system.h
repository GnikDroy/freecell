#pragma once

#include "renderer.h"
#include "sprite.h"

struct Vector;
struct World;

void renderer_render_sprite(Sprite sprite, Vector *vertices_vec,
                            Vector *indices_vec);

void renderer_draw(Renderer renderer, World *world);
