#pragma once
#include <stdint.h>

typedef uint32_t Shader;

Shader shader_init(const char *vertex_shader_source,
                     const char *fragment_shader_source);

void shader_free(Shader *shader);

void shader_set_int(Shader shader, const char *name, int value);

void shader_set_mat4(Shader shader, const char *name, const float *value);
