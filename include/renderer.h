#pragma once

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "mesh.h"
#include "world.h"

typedef struct {
  GLFWwindow *window;
  uint32_t shader;
  GPUMesh mesh;
  uint32_t textures;
} Renderer;

Renderer renderer_init(GLFWwindow *window, World *world);

void renderer_free(Renderer *renderer);

void renderer_clear();

uint32_t renderer_create_texture_from_image(Image *image);

void renderer_set_shader(uint32_t shader);

void renderer_bind_texture(uint32_t slot, GLenum target, uint32_t texture);

void renderer_draw_mesh(GPUMesh *mesh, GLenum primitive);
