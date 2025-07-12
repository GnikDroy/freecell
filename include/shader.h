#pragma once
#include <stdint.h>

uint32_t shader_compile(const char *vertex_shader_source,
                              const char *fragment_shader_source);
