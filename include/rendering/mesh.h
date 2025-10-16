#pragma once
#include "core/vector.h"
#include "rendering/primitives.h"
#include "rendering/sprite.h"

#include <stdint.h>

#pragma pack(push, 1)
typedef struct Vertex {
    float x, y, z;
    float r, g, b, a;
    float u, v;
} Vertex;
#pragma pack(pop)

typedef struct Mesh {
    Vector vertices;
    Vector indices;
} Mesh;

typedef struct GPUMesh {
    uint32_t VAO;
    uint32_t VBO;
    uint32_t EBO;
    size_t index_count;
} GPUMesh;

Mesh mesh_init();

void mesh_clear(Mesh* mesh);

void mesh_free(Mesh* mesh);

void mesh_push_line(Mesh* mesh, Line line);

void mesh_push_triangle(Mesh* mesh, Triangle triangle);

void mesh_push_quad(Mesh* mesh, Quad quad);

void mesh_push_ellipse(Mesh* mesh, Ellipse_ ellipse);

void mesh_push_circle(Mesh* mesh, Circle circle);

void mesh_push_sprite(Mesh* mesh, Sprite sprite);

GPUMesh gpu_mesh_init();

void gpu_mesh_free(GPUMesh* mesh);

void gpu_mesh_upload(GPUMesh* gpu_mesh, Mesh* mesh);
