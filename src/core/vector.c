#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "core/vector.h"

Vector vec_init(size_t elem_size) {
    Vector vector = {
        .elem_size = elem_size,
        .data = NULL,
        .capacity = 0,
        .size = 0,
    };
    return vector;
}

void vec_free(Vector* vector) {
    free(vector->data);
    vector->data = NULL;
    vector->capacity = vector->size = 0;
}

Vector vec_ensure_capacity(Vector* vector, size_t capacity) {
    if (vector->capacity < capacity) {
        void* new_data = realloc(vector->data, capacity * vector->elem_size);
        if (!new_data) {
            exit(EXIT_FAILURE);
        }
        vector->data = new_data;
        vector->capacity = vector->capacity;
    }
    return *vector;
}

Vector vec_push_back(Vector* vector, const void* const data) {
    vec_ensure_capacity(vector, vector->size + 1);
    memcpy((uint8_t*)vector->data + vector->elem_size * vector->size, data, vector->elem_size);
    vector->size += 1;
    return *vector;
}

Vector vec_push_front(Vector* vector, const void* const data) {
    vec_ensure_capacity(vector, vector->size + 1);
    memmove(
        (uint8_t*)vector->data + vector->elem_size, vector->data, vector->elem_size * vector->size);
    memcpy(vector->data, data, vector->elem_size);
    vector->size += 1;
    return *vector;
}

void vec_pop_back(Vector* vector) { vector->size = vector->size ? vector->size - 1 : 0; }

void* vec_get(Vector* vector, size_t index) {
    return (uint8_t*)vector->data + index * vector->elem_size;
}

void* vec_set(Vector* vector, size_t index, const void* const data) {
    void* ptr = vec_get(vector, index);
    memcpy(ptr, data, vector->elem_size);
    return ptr;
}

void vec_delete(Vector* vector, size_t i) {
    if (i > vector->size - 1) {
        return;
    }
    if (i != vector->size - 1) {
        memmove((uint8_t*)vector->data + i * vector->elem_size,
            (uint8_t*)vector->data + (i + 1) * vector->elem_size,
            (vector->size - i) * vector->elem_size);
    }
    vector->size -= 1;
}
