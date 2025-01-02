#include <setjmp.h>
#include <stdalign.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "./arena.h"
#include <cmocka.h>
#include <stdio.h>

void test_arena_create(void **state) {
  (void)state;

  Arena *arena = arena_create(10);

  assert_non_null(arena);
  assert_int_equal(10, arena->capacity);
  assert_int_equal(0, arena->index);
  assert_non_null(arena->region);

  arena_destroy(arena);
}

void test_arena_alloc(void **state) {
  (void)state;

  Arena *arena = arena_create(5 * sizeof(int));

  int *array = arena_alloc(arena, 5 * sizeof(int), alignof(int));

  assert_non_null(array);

  for (int i = 0; i < 5; i++) {
    array[i] = i * i;
  }

  for (int i = 0; i < 5; i++) {
    assert_int_equal(array[i], i * i);
  }

  int *new_int = arena_alloc(arena, sizeof(int), alignof(int));

  assert_null(new_int);

  arena_destroy(arena);
}

typedef struct A {
  int b;
  int c;
  char *d;
} A;
void test_arena_alloc_multiple_types(void **state) {
  (void)state;

  Arena *arena = arena_create(5 * sizeof(int) + sizeof(A) + 10);

  int *array = arena_alloc(arena, 5 * sizeof(int), alignof(int));
  A *a = arena_alloc(arena, sizeof(A), alignof(A));

  assert_non_null(array);
  assert_non_null(a);

  *a = (A){.b = 267, .c = 821, .d = "Cmocka is great"};

  for (int i = 0; i < 5; i++) {
    array[i] = i * i;
  }

  for (int i = 0; i < 5; i++) {
    assert_int_equal(array[i], i * i);
  }

  assert_int_equal(267, a->b);
  assert_int_equal(821, a->c);
  assert_string_equal(a->d, "Cmocka is great");

  arena_destroy(arena);
}

void test_arena_clear(void **state) {
  (void)state;

  Arena *arena = arena_create(5 * sizeof(int));

  int *array = arena_alloc(arena, 5 * sizeof(int), alignof(int));

  for (int i = 0; i < 5; i++) {
    array[i] = i * i;
  }

  assert_int_equal(5 * sizeof(int), arena->index);
  arena_clear(arena);
  assert_int_equal(0, arena->index);

  arena_destroy(arena);
}

void test_arena_expand(void **state) {
  (void)state;

  Arena *arena = arena_create(5 * sizeof(int));

  int *array = arena_alloc(arena, 5 * sizeof(int), alignof(int));

  assert_non_null(array);

  for (int i = 0; i < 5; i++) {
    array[i] = i * i;
  }

  for (int i = 0; i < 5; i++) {
    assert_int_equal(array[i], i * i);
  }

  int *new_int = arena_alloc(arena, sizeof(int), alignof(int));

  assert_null(new_int);

  arena = arena_expand(arena, 6 * sizeof(int));

  assert_non_null(arena);
  assert_non_null(arena->region);

  assert_int_equal(arena->capacity, 6 * sizeof(int));

  new_int = arena_alloc(arena, sizeof(int), alignof(int));

  assert_non_null(new_int);

  arena_destroy(arena);
}

void test_arena_copy(void **state) {
  (void)state;
  Arena *arena = arena_create(5 * sizeof(int));
  Arena *arena2 = arena_create(6 * sizeof(int));
  Arena *arena3 = arena_create(3 * sizeof(int));

  int *array = arena_alloc(arena, 5 * sizeof(int), alignof(int));

  for (int i = 0; i < 5; i++) {
    array[i] = i * i;
  }

  for (int i = 0; i < 5; i++) {
    assert_int_equal(array[i], i * i);
  }
  assert_int_equal(0, arena->capacity - arena->index);

  size_t result = arena_copy(arena, arena2);
  assert_int_not_equal(0, result);
  assert_int_equal(sizeof(int), arena2->capacity - arena2->index);

  size_t result2 = arena_copy(arena, arena3);
  assert_int_not_equal(0, result);
  assert_int_equal(result2, 3 * sizeof(int));
  assert_int_equal(0, arena3->capacity - arena3->index);

  arena_destroy(arena);
  arena_destroy(arena2);
  arena_destroy(arena3);
}

int main() {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_arena_create),
      cmocka_unit_test(test_arena_alloc),
      cmocka_unit_test(test_arena_alloc_multiple_types),
      cmocka_unit_test(test_arena_clear),
      cmocka_unit_test(test_arena_expand),
      cmocka_unit_test(test_arena_copy),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
