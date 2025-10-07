#pragma once
#include "rendering/sprite.h"
#include "rendering/primitives.h"

typedef struct World World;
typedef struct Vector Vector;
typedef struct Mesh Mesh;
typedef struct UIElement UIElement;
typedef struct AnimationSystem AnimationSystem;

void mesh_push_line(Mesh* mesh, Line line);

void mesh_push_triangle(Mesh* mesh, Triangle triangle);

void mesh_push_quad(Mesh* mesh, Quad quad);

void mesh_push_ellipse(Mesh* mesh, Ellipse ellipse);

void mesh_push_circle(Mesh* mesh, Circle circle);

void mesh_push_sprite(Mesh* mesh, Sprite sprite);

void mesh_push_text(Mesh* mesh, World* world, UIElement* ui_element);

void mesh_push_ui_element(Mesh* mesh, World* world, UIElement* ui_element);

void mesh_push_ui_elements(Mesh* mesh, World* world, Vector* vec);

void mesh_push_animations(Mesh* mesh, World* world);

void render_world(World* world);
