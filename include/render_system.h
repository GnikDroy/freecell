#pragma once
#include "world.h"

struct World;

void mesh_push_sprite(Mesh *mesh, Sprite sprite);

void mesh_push_ui_elements(Mesh *mesh, Vector *vec);

void renderer_draw(World *world);
