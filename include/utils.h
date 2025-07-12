#pragma once
#include <string.h>

inline void mat4_ortho(float left, float right, float bottom, float top,
                       float near, float far, float *out) {
  memset(out, 0, sizeof(float) * 16);
  out[0] = 2.0f / (right - left);
  out[5] = 2.0f / (top - bottom);
  out[10] = -2.0f / (far - near);
  out[12] = -(right + left) / (right - left);
  out[13] = -(top + bottom) / (top - bottom);
  out[14] = -(far + near) / (far - near);
  out[15] = 1.0f;
}
