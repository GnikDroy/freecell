#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


typedef struct {
    void *data;
    size_t capacity;
    size_t size;
    size_t elem_size;
} Vector;


inline Vector vec_init(size_t elem_size) {
    Vector vector = {
            .elem_size = elem_size,
            .data = NULL,
            .capacity = 0,
            .size = 0,
    };
    return vector;
}

inline void vec_free(Vector *vector) {
    free(vector->data);
    vector->data = NULL;
    vector->capacity = vector->size = 0;
}

inline Vector vec_ensure_capacity(Vector *vector, size_t capacity) {
    if (vector->capacity < capacity) {
        vector->data = realloc(vector->data, capacity * vector->elem_size);
        vector->capacity = vector->capacity;
    }
    return *vector;
}

inline Vector vec_push_back(Vector *vector, const void *const data) {
    vec_ensure_capacity(vector, vector->size + 1);
    memcpy((uint8_t *) vector->data + vector->elem_size * vector->size, data, vector->elem_size);
    vector->size += 1;
    return *vector;
}

inline Vector vec_push_front(Vector *vector, const void *const data) {
    vec_ensure_capacity(vector, vector->size + 1);
    memmove((uint8_t *) vector->data + vector->elem_size, vector->data, vector->elem_size * vector->size);
    memcpy(vector->data, data, vector->elem_size);
    vector->size += 1;
    return *vector;
}

inline void vec_pop_back(Vector *vector) {
    vector->size = vector->size ? vector->size - 1 : 0;
}

inline void vec_delete(Vector *vector, size_t i) {
    if (i > vector->size - 1) {
        return;
    }
    if (i != vector->size - 1) {
        memmove((uint8_t *) vector->data + i * vector->elem_size,
                (uint8_t *) vector->data + (i + 1) * vector->elem_size,
                (vector->size - i) * vector->elem_size);
    }
    vector->size -= 1;
}

