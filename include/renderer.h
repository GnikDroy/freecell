#pragma once

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "mesh.h"
#include "sprite.h"

typedef struct {
  char _; // dummy field to ensure this struct is not empty
} Renderer;

Renderer renderer_init(void);

void renderer_free(Renderer *renderer);

void renderer_clear(Color clear_color);

void renderer_set_shader(uint32_t shader);

void renderer_bind_texture(uint32_t slot, GLenum target, uint32_t texture);

void renderer_draw_mesh(GPUMesh *mesh, GLenum primitive);
