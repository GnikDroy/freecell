#pragma once
#include <stdint.h>

typedef struct {
    void* base;
    size_t offset;
} APtr;

extern void* arena_buffer;
extern size_t arena_size;
extern size_t arena_capacity;

APtr aalloc(size_t size);
void* aptr(APtr ptr);
void aclear();
void afree();
