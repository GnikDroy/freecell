#pragma once

#include "sprite.h"
#include "world.h"

struct World;

void mesh_push_sprite(Mesh *mesh, Sprite sprite);

void renderer_draw(World *world);
