#include "renderer.h"
#include "constants.h"
#include "debug.h"
#include "log.h"
#include "shader.h"

Renderer renderer_init(GLFWwindow *window, World *world) {
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(openglDebugCallback, NULL);

  uint32_t shaderProgram =
      shader_compile(MAIN_VERTEX_SHADER_SOURCE, MAIN_FRAGMENT_SHADER_SOURCE);

  if (shaderProgram == 0) {
    exit(EXIT_FAILURE);
  }

  // Enable blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  uint32_t VAO;
  glGenVertexArrays(1, &VAO);

  uint32_t VBO;
  glGenBuffers(1, &VBO);

  uint32_t EBO;
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  size_t stride = sizeof(Vertex);

  // Position (location = 0)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                        (void *)offsetof(Vertex, x));
  glEnableVertexAttribArray(0);

  // Color (location = 1)
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride,
                        (void *)offsetof(Vertex, r));
  glEnableVertexAttribArray(1);

  // TexCoords (location = 2)
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                        (void *)offsetof(Vertex, u));
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBindVertexArray(0);

  uint32_t texture =
      renderer_create_texture_from_image(&world->assets.spritesheet);

  Renderer renderer = {
      .window = window,
      .shader = shaderProgram,
      .VBO = VBO,
      .VAO = VAO,
      .EBO = EBO,
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
  glDeleteBuffers(1, &renderer->VBO);
  glDeleteBuffers(1, &renderer->EBO);
  glDeleteTextures(1, &renderer->textures);
  glDeleteVertexArrays(1, &renderer->VAO);
  glDeleteProgram(renderer->shader);
}

void renderer_clear(Renderer renderer) {
  (void)renderer;
  glClearColor(0.2f, 0.4f, 0.2f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

void renderer_set_shader(uint32_t shader) { glUseProgram(shader); }

void renderer_bind_texture(uint32_t slot, GLenum target, uint32_t texture) {
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(target, texture);
}

void renderer_draw_mesh(Renderer renderer, Vector *vertices, Vector *indices,
                        GLenum primitive) {
  glBindBuffer(GL_ARRAY_BUFFER, renderer.VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices->size * vertices->elem_size,
               vertices->data, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer.EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size * indices->elem_size,
               indices->data, GL_STATIC_DRAW);

  glBindVertexArray(renderer.VAO);
  glDrawElements(primitive, (GLsizei)indices->size, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
