#pragma once
#include <stdint.h>

typedef struct Vector {
  void *data;
  size_t capacity;
  size_t size;
  size_t elem_size;
} Vector;

Vector vec_init(size_t elem_size);

void vec_free(Vector *vector);

Vector vec_ensure_capacity(Vector *vector, size_t capacity);

Vector vec_push_back(Vector *vector, const void *const data);

Vector vec_push_front(Vector *vector, const void *const data);

void vec_pop_back(Vector *vector);

void *vec_get(Vector *vector, size_t index);

void *vec_set(Vector *vector, size_t index, const void *const data);

void vec_delete(Vector *vector, size_t i);
