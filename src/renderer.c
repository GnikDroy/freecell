#include "renderer.h"
#include "constants.h"
#include "debug.h"
#include "log.h"
#include "mesh.h"
#include "shader.h"

Renderer renderer_init(GLFWwindow *window, World *world) {
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(openglDebugCallback, NULL);

  // Enable blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  uint32_t shaderProgram =
      shader_compile(MAIN_VERTEX_SHADER_SOURCE, MAIN_FRAGMENT_SHADER_SOURCE);

  if (shaderProgram == 0) {
    exit(EXIT_FAILURE);
  }

  uint32_t texture =
      renderer_create_texture_from_image(&world->assets.spritesheet);

  Renderer renderer = {
      .window = window,
      .shader = shaderProgram,
      .mesh = gpu_mesh_init(),
      .textures = texture,
  };

  return renderer;
}

uint32_t renderer_create_texture_from_image(Image *image) {
  uint32_t texture;

  GLenum internalFormat = GL_RGBA8;
  GLenum format = GL_RGBA;

  if (image->channels == 1) {
    internalFormat = GL_R8;
    format = GL_RED;
  } else if (image->channels == 3) {
    internalFormat = GL_RGB8;
    format = GL_RGB;
  } else if (image->channels == 4) {
    internalFormat = GL_RGBA8;
    format = GL_RGBA;
  } else {
    log_error("Unsupported image channel count: %d", image->channels);
    exit(EXIT_FAILURE);
  }

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, image->width, image->height);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image->width, image->height, format,
                  GL_UNSIGNED_BYTE, image->data);
  return texture;
}

void renderer_free(Renderer *renderer) {
  gpu_mesh_free(&renderer->mesh);
  glDeleteTextures(1, &renderer->textures);
  glDeleteProgram(renderer->shader);
}

void renderer_clear() {
  glClearColor(0.2f, 0.4f, 0.2f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

void renderer_set_shader(uint32_t shader) { glUseProgram(shader); }

void renderer_bind_texture(uint32_t slot, GLenum target, uint32_t texture) {
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(target, texture);
}

void renderer_draw_mesh(GPUMesh *mesh, GLenum primitive) {
  glBindVertexArray(mesh->VAO);
  glDrawElements(primitive, (GLsizei)mesh->index_count, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
