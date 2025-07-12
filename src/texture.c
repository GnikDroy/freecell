#include <glad/glad.h>

#include "texture.h"
#include "log.h"

uint32_t texture_init(Image *image) {
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

void texture_free(uint32_t* texture) {
  glDeleteTextures(1, texture);
  texture = NULL;
}
