#pragma once
#include "world.h"

void mesh_push_sprite(Mesh *mesh, Sprite sprite);

void mesh_push_ui_elements(Mesh *mesh, Vector *vec);

void draw_world(World *world);

void render_world(World *world);
