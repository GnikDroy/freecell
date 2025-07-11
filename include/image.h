#pragma once
#include <stb_image.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  int width;
  int height;
  int channels;
  uint8_t *data;
} Image;

inline Image image_load(const char *path, int desired_channels) {
  Image image;
  stbi_set_flip_vertically_on_load(true);
  image.data = stbi_load(path, &image.width, &image.height, &image.channels,
                         desired_channels);
  return image;
}

inline Image image_load_from_memory(const uint8_t *data, int size,
                                    int desired_channels) {
  Image image;
  stbi_set_flip_vertically_on_load(true);
  image.data = stbi_load_from_memory(data, size, &image.width, &image.height,
                                     &image.channels, desired_channels);
  return image;
}

inline void image_free(Image *image) {
  stbi_image_free(image->data);
  image->data = NULL;
}

inline const char *image_get_error_msg(void) { return stbi_failure_reason(); }
