#pragma once
#include "rendering/sprite.h"

typedef struct World World;
typedef struct Vector Vector;
typedef struct Mesh Mesh;

void mesh_push_sprite(Mesh* mesh, Sprite sprite);

void mesh_push_ui_elements(Mesh* mesh, Vector* vec);

void layout_world(World* world);

void bake_world(World* world);

void render_world(World* world);
