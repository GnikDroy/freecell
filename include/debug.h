#pragma once
#include <glad/glad.h>

typedef struct World World;

void APIENTRY openglDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar* message, const void* userParam);

void debug_render_mouse(World* world);

void debug_render_hit_hitbox(World* world);
