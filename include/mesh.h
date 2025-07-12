#pragma once
#include "vector.h"
#include <stdint.h>

#pragma pack(push, 1)
typedef struct {
  float x, y, z;
  float r, g, b, a;
  float u, v;
} Vertex;
#pragma pack(pop)

typedef struct {
  Vector vertices;
  Vector indices;
} Mesh;

typedef struct {
  uint32_t VAO;
  uint32_t VBO;
  uint32_t EBO;
  size_t index_count;
} GPUMesh;

Mesh mesh_init();

void mesh_free(Mesh *mesh);

GPUMesh gpu_mesh_init();

void gpu_mesh_free(GPUMesh *mesh);

void upload_mesh(GPUMesh *gpu_mesh, Mesh *mesh);
