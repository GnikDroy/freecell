#pragma once
typedef struct World World;
typedef struct Vector Vector;
typedef struct Mesh Mesh;
typedef struct UIElement UIElement;
typedef struct AnimationSystem AnimationSystem;

void mesh_push_text(Mesh* mesh, World* world, UIElement* ui_element);

void mesh_push_ui_element(Mesh* mesh, World* world, UIElement* ui_element);

void mesh_push_ui_elements(Mesh* mesh, World* world, Vector* vec);

void mesh_push_animations(Mesh* mesh, World* world);

void render_world(World* world);
