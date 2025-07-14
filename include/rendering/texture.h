#pragma once
#include <stdint.h>

#include "rendering/image.h"

typedef uint32_t Texture;

uint32_t texture_init(Image* image);

void texture_free(uint32_t* texture);
