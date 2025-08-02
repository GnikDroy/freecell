#include "core/aalloc.h"
#include <stdlib.h>

void* arena_buffer = NULL;
size_t arena_size = 0;
size_t arena_capacity = 0;

static void arena_ensure_capacity(size_t new_capacity) {
    if (arena_capacity < new_capacity) {
        void* new_buffer = realloc(arena_buffer, new_capacity);
        if (!new_buffer) {
            exit(EXIT_FAILURE);
        }
        arena_buffer = new_buffer;
        arena_capacity = new_capacity;
    }
}

APtr aalloc(size_t size) {
    arena_ensure_capacity(arena_size + size);
    APtr result = arena_size;
    arena_size += size;
    return result;
}

void* aptr(APtr ptr) { return (char*)arena_buffer + ptr; }

void aclear() { arena_size = 0; }

void afree() {
    free(arena_buffer);
    arena_buffer = NULL;
    arena_size = 0;
    arena_capacity = 0;
}
