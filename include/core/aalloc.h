#pragma once
#include <stddef.h>
#include <stdint.h>

typedef ptrdiff_t APtr;

extern void* arena_buffer;
extern size_t arena_size;
extern size_t arena_capacity;

APtr aalloc(size_t size);
void* aptr(APtr ptr);
void aclear();
void afree();
