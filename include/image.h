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

Image image_load(const char *path, int desired_channels);

Image image_load_from_memory(const uint8_t *data, int size,
                                    int desired_channels);

void image_free(Image *image);

const char *image_get_error_msg(void);
