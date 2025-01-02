#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>
#include <string.h>

typedef struct Arena {
  char *region;
  size_t index;
  size_t capacity;
} Arena;

/**
 * Creates a new memory arena with a specified initial capacity.
 *
 * @param initial_capacity The size of the memory region to allocate for the
 * arena in bytes.
 *
 * @return A pointer to the newly created Arena structure, or NULL if allocation
 * fails or if initial_capacity is 0.
 */
Arena *arena_create(size_t initial_capacity);

/**
 * Allocates a block of memory from the arena with the specified size and
 * alignment.
 *
 * @param arena Pointer to the Arena from which memory will be allocated.
 * @param size The size of the memory block to allocate in bytes.
 * @param alignment The required memory alignment for the allocation.
 *
 * @return A pointer to the allocated memory block, or NULL if the allocation
 * cannot be satisfied.
 *
 * @note Alignment ensures that the returned pointer adheres to specific memory
 * alignment constraints.
 */
void *arena_alloc(Arena *arena, size_t size, size_t alignment);

/**
 * Resets the arena, making all memory available for allocation again without
 * freeing the allocated memory block.
 *
 * @param arena Pointer to the Arena to clear.
 *
 * @note This operation does not free or reallocate the memory; it simply resets
 * the allocation index.
 */
void arena_clear(Arena *arena);

/**
 * Frees the memory associated with the arena, including its memory region and
 * the arena structure itself.
 *
 * @param arena Pointer to the Arena to destroy.
 */
void arena_destroy(Arena *arena);

/**
 * Expands the arena’s memory region to a new size if it exceeds the current
 * capacity.
 *
 * @param arena Pointer to the Arena to expand.
 * @param size The new size of the memory region in bytes.
 *
 * @return A pointer to the updated Arena, or NULL if the expansion fails or if
 the requested size is less than or equal to the current capacity.
 */
Arena *arena_expand(Arena *arena, size_t size);

/**
 * Copies data from one arena to another.
 *
 * @param src Pointer to the source Arena.
 * @param dest Pointer to the destination Arena.
 *
 * @return The number of bytes copied.
 *
 * @note If the destination arena’s capacity is smaller than the source arena’s
 * current usage, only part of the data is copied.
 * @note The destination arena’s index is updated to reflect the copied data.
 */
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
