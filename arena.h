#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>
#include <string.h>

typedef struct Arena {
  char *region;
  size_t index;
  size_t capacity;
} Arena;

Arena *arena_create(size_t initial_capacity);
void *arena_alloc(Arena *arena, size_t size, size_t alignment);
void arena_clear(Arena *arena);
void arena_destroy(Arena *arena);
Arena *arena_expand(Arena *arena, size_t size);
size_t arena_copy(Arena *src, Arena *dest);

#endif // ARENA_H

#ifndef ARENA_IMPL
#define ARENA_IMPL

#include <stdlib.h>

Arena *arena_create(size_t initial_capacity) {
  if (initial_capacity == 0) {
    return NULL;
  }

  Arena *arena = (Arena *)malloc(sizeof(Arena));
  if (arena == NULL) {
    return NULL;
  }

  arena->region = (char *)malloc(initial_capacity);
  if (arena->region == NULL) {
    free(arena);
    return NULL;
  }

  arena->capacity = initial_capacity;
  arena->index = 0;

  return arena;
}

void *arena_alloc(Arena *arena, size_t size, size_t alignment) {
  if (arena == NULL || arena->region == NULL || size == 0) {
    return NULL;
  }

  size_t current_address = (size_t)(arena->region + arena->index);
  size_t offset = current_address % alignment;

  if (offset != 0) {
    arena->index += (alignment - offset);
  }

  if (arena->index + size > arena->capacity) {
    return NULL;
  }

  void *ptr = arena->region + arena->index;
  arena->index += size;

  return ptr;
}

void arena_clear(Arena *arena) {
  if (arena == NULL) {
    return;
  }

  arena->index = 0;
}

void arena_destroy(Arena *arena) {
  if (arena == NULL) {
    return;
  }

  if (arena->region != NULL) {
    free(arena->region);
  }

  free(arena);
}

Arena *arena_expand(Arena *arena, size_t size) {
  if (arena == NULL || size <= arena->capacity) {
    return NULL;
  }

  arena->region = (char *)realloc(arena->region, size);
  if (arena->region == NULL) {
    return NULL;
  }

  arena->capacity = size;
  return arena;
}

size_t arena_copy(Arena *src, Arena *dest) {
  if (src == NULL || dest == NULL) {
    return 0;
  }

  size_t copy_size;

  if (dest->capacity > src->capacity) {
    copy_size = src->index;
  } else {
    copy_size = dest->capacity;
  }

  memcpy(dest->region, src->region, copy_size);
  dest->index = copy_size;

  return copy_size;
}

#endif // ARENA_IMPL
