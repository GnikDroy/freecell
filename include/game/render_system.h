#pragma once
#include "rendering/sprite.h"

typedef struct World World;
typedef struct Vector Vector;
typedef struct Mesh Mesh;
typedef struct UIElement UIElement;
typedef struct AnimationSystem AnimationSystem;

void mesh_push_sprite(Mesh* mesh, Sprite sprite);

void mesh_push_text(Mesh* mesh, World* world, UIElement* ui_element);

void mesh_push_ui_element(Mesh* mesh, World* world, UIElement* ui_element);

void mesh_push_ui_elements(Mesh* mesh, World* world, Vector* vec);

void render_world(World* world);
