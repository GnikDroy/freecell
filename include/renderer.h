#pragma once

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "mesh.h"

typedef struct {
  GLFWwindow *window;
} Renderer;

Renderer renderer_init(GLFWwindow *window);

void renderer_free(Renderer *renderer);

void renderer_clear();

void renderer_set_shader(uint32_t shader);

void renderer_bind_texture(uint32_t slot, GLenum target, uint32_t texture);

void renderer_draw_mesh(GPUMesh *mesh, GLenum primitive);
