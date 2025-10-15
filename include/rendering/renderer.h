#pragma once

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "rendering/color.h"
#include "rendering/mesh.h"

void renderer_init(void);

void renderer_clear(Color clear_color);

void renderer_set_shader(uint32_t shader);

void renderer_bind_texture(uint32_t slot, GLenum target, uint32_t texture);

void renderer_draw_mesh(GPUMesh* mesh, GLenum primitive);

void APIENTRY openglDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar* message, const void* userParam);
