#pragma once
#include <stdint.h>

uint32_t shader_compile(const char *vertex_shader_source,
                              const char *fragment_shader_source);

void shader_set_int(uint32_t shader, const char *name, int value);
void shader_set_mat4(uint32_t shader, const char *name,
                            const float *value);
