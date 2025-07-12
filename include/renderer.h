#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "vector.h"
#include "world.h"

typedef struct {
  GLFWwindow *window;
  uint32_t shader;
  uint32_t VBO;
  uint32_t VAO;
  uint32_t EBO;
  uint32_t textures;
} Renderer;

#pragma pack(push, 1)
typedef struct {
  float x, y, z;
  float r, g, b, a;
  float u, v;
} Vertex;
#pragma pack(pop)

Renderer renderer_init(GLFWwindow *window, World *world);

void renderer_free(Renderer *renderer);

void renderer_clear(Renderer renderer);

void renderer_draw_mesh(Renderer renderer, Vector *vertices, Vector *indices,
                        GLenum primitive);
