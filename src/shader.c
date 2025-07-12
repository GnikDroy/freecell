#include <glad/glad.h>
#include "log.h"

uint32_t shader_compile(const char *vertex_shader_source,
                        const char *fragment_shader_source) {
  uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertex_shader_source, NULL);
  glCompileShader(vertexShader);

  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, sizeof(infoLog), NULL, infoLog);
    log_error("%s", infoLog);
    glDeleteShader(vertexShader);
    return 0;
  }

  uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragment_shader_source, NULL);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, sizeof(infoLog), NULL, infoLog);
    log_error("%s", infoLog);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return 0;
  }

  uint32_t shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, sizeof(infoLog), NULL, infoLog);
    log_error("%s", infoLog);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteProgram(shaderProgram);
    return 0;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return shaderProgram;
}

void shader_set_int(uint32_t shader, const char *name, int value) {
  glUniform1i(glGetUniformLocation(shader, name), value);
}

void shader_set_mat4(uint32_t shader, const char *name, const float *value) {
  glUniformMatrix4fv(glGetUniformLocation(shader, name), 1, GL_FALSE, value);
}
