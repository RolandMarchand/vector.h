#ifndef VECTOR_H
#define VECTOR_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Library to generate type-safe vector types.
 * 
 * This library is portable (tested on GCC/Clang/MSVC/ICX, x86_64/ARM64, all
 * warnings and pedantic) and is C89 compatible.
 *
 * To generate vectors, use the macros VECTOR_DECLARE() to generate the header,
 * and VECTOR_DEFINE() to generate the source. It is recommended to place them
 * in their respective files. Generate as many different types of vectors as
 * you want.
 *
 * This library is not thread safe.
 *
 * This library follows a 2x capacity growing policy.
 * 
 * Vector is optimized for iteration, here is an example:
 * for (int *i = v.begin; i != v.end; i++)
 * 
 * To configure this library #define the symbols before including the library.
 * This is usually done in the header file where VECTOR_DECLARE() is called.
 * 
 * Configuration options:
 * 
 * - VECTOR_NO_PANIC_ON_NULL (default 0): if true (1), does not panic
 *   upon passing NULL to vector functions. Otherwise, panic.
 * 
 * - VECTOR_REALLOC (default realloc(3)): specify the allocator. If using
 *   a custom allocator, must also specify VECTOR_FREE.
 * 
 * - VECTOR_FREE (default free(3)): specify the deallocator. If using a
 *   custom deallocator, must also specify VECTOR_REALLOC.
 * 
 * - VECTOR_LONG_JUMP_NO_ABORT (default undefined): for testing only. Jump to
 *   externally defined "jmp_buf abort_jmp" instead of panicking. Unlike other
 *   configuration options, must be defined before including the library.
 *
 * 
 * API Functions:
 *
 * The following documentation takes this generated vector for instance:
 * VECTOR_DECLARE(Vector, vector, SampleType)
 * 
 * All functions panic if vec is NULL (unless VECTOR_NO_PANIC_ON_NULL).
 *
 * VECTOR_SIZE(Vector *vec)
 *   Macro that returns the current element count as a size_t.
 *
 * VECTOR_CAPACITY(Vector *vec)
 *   Macro that returns the total capacity allocated in element count as a
 *   size_t.
 *
 * void vector_init(Vector *vec, size_t capacity)
 *   Initialize empty vector. vec must be zero-initialized. capacity is in
 *   element count. Panics if vec contains garbage data or on memory failure.
 *
 * void vector_free(Vector *vec)
 *   Deallocate vector memory. Safe to call on already-freed vectors.
 *
 * void vector_grow(Vector *vec, size_t desired)
 *   Increase capacity to desired element count. Panics if shrinking attempted.
 *
 * void vector_push(Vector *vec, SampleType value)
 *   Append element, growing capacity if needed. Auto-initializes empty vectors.
 *   O(1) amortized complexity.
 *
 * SampleType vector_pop(Vector *vec)
 *   Remove and return last element. Panics if empty.
 *
 * SampleType vector_get(const Vector *vec, size_t idx)
 *   Get element at 0-based index. Panics if idx out of bounds.
 *
 * void vector_set(Vector *vec, size_t idx, SampleType value)
 *   Set element at 0-based index. Panics if idx out of bounds.
 *
 * void vector_insert(Vector *vec, size_t idx, SampleType value)
 *   Insert element at 0-based index, shifting later elements right.
 *   idx equal to size appends to end. Panics if idx > size.
 *   O(n) worst-case complexity.
 *
 * void vector_delete(Vector *vec, size_t idx)
 *   Remove element at 0-based index, shifting later elements left.
 *   Panics if idx out of bounds. O(n) worst-case complexity.
 *
 * void vector_duplicate(Vector *RESTRICT dest, const Vector *RESTRICT src)
 *   Copy src vector to dest. dest must be uninitialized. Overwrites existing
 *   dest data without freeing it.
 *
 * void vector_clear(Vector *vec)
 *   Remove all elements without deallocating capacity.
 *
 *
 * Example:
 *  int main(void)
 *  {
 *     Vector numbers = {0};
 *     int first_element, last_element, popped_element;
 *     int *ptr;
 *     
 *     vector_push(&numbers, 10);
 *     vector_push(&numbers, 20);
 *     vector_push(&numbers, 30);
 *     vector_push(&numbers, 40);
 *     vector_push(&numbers, 50);
 *     
 *     first_element = vector_get(&numbers, 0);
 *     last_element = vector_get(&numbers, VECTOR_SIZE(&numbers) - 1);
 *     
 *     vector_insert(&numbers, 2, 25);
 *     
 *     for (ptr = numbers.begin; ptr != numbers.end; ptr++) {
 *     }
 *     
 *     popped_element = vector_pop(&numbers);
 *     
 *     vector_delete(&numbers, 1);
 *     
 *     vector_set(&numbers, 0, 99);
 *     
 *     vector_clear(&numbers);
 *     
 *     vector_free(&numbers);
 *     
 *     return 0;
 *  }
 */

#if defined(VECTOR_REALLOC) && !defined(VECTOR_FREE) || \
	!defined(VECTOR_REALLOC) && defined(VECTOR_FREE)
#error "You must define both VECTOR_REALLOC and VECTOR_FREE, or neither."
#endif
#if !defined(VECTOR_REALLOC) && !defined(VECTOR_FREE)
#define VECTOR_REALLOC(p, s) (realloc((p), (s)))
#define VECTOR_FREE(p) (free((p)))
#endif

#ifndef VECTOR_NO_PANIC_ON_NULL
#define VECTOR_NO_PANIC_ON_NULL 0
#endif

#ifdef VECTOR_LONG_JUMP_NO_ABORT
#include <setjmp.h>
extern jmp_buf abort_jmp;
#endif

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
#define VECTOR_NORETURN [[noreturn]]
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define VECTOR_NORETURN _Noreturn
#elif defined(__GNUC__) || defined(__clang__)
#define VECTOR_NORETURN __attribute__((noreturn))
#else
#define VECTOR_NORETURN
#endif

#ifndef __STDC_VERSION__
#define VECTOR_INLINE
#elif _MSC_VER
#define VECTOR_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define VECTOR_INLINE __attribute__((always_inline)) inline
#else
#define VECTOR_INLINE inline
#endif

#ifndef __STDC_VERSION__
#define RESTRICT
#else
#define RESTRICT restrict
#endif

#define VECTOR_SIZE(vec) (size_t)((vec)->end - (vec)->begin)
#define VECTOR_IS_SIZE_ZERO(vec) ((vec)->end == (vec)->begin)
#define VECTOR_CAPACITY(vec) (size_t)((vec)->end_of_storage - (vec)->begin)

enum { VECTOR_DEFAULT_CAPACITY = 8, VECTOR_GROWTH_FACTOR = 2 };
typedef int SampleType;

/* Declarations start here */

typedef struct Vector {
	SampleType *begin;
	SampleType *end;
	SampleType *end_of_storage;
} Vector;

VECTOR_NORETURN void vector_panic(const char *message);
void vector_assert(const Vector *vec);
void vector_grow(Vector *vec, size_t desired);
void vector_free(Vector *vec);
void vector_init(Vector *vec, size_t capacity);
void vector_push(Vector *vec, SampleType value);
SampleType vector_pop(Vector *vec);
SampleType vector_get(const Vector *vec, size_t idx);
void vector_set(Vector *vec, size_t idx, SampleType value);
void vector_insert(Vector *vec, size_t idx, SampleType value);
void vector_delete(Vector *vec, size_t idx);
void vector_duplicate(Vector *RESTRICT dest, const Vector *RESTRICT src);
void vector_clear(Vector *vec);
/* Declarations stop here */

#ifdef VECTOR_LONG_JUMP_NO_ABORT
#define VECTOR_DEFINE_PANIC(Function_Prefix_)                              \
	VECTOR_NORETURN void Function_Prefix_##_panic(const char *message) \
	{                                                                  \
		assert(message);                                           \
		longjmp(abort_jmp, 1);                                     \
	}
#else
#define VECTOR_DEFINE_PANIC(Function_Prefix_)                              \
	VECTOR_NORETURN void Function_Prefix_##_panic(const char *message) \
	{                                                                  \
		assert(message);                                           \
		(void)fprintf(stderr, "%s\n", message);                    \
		abort();                                                   \
	}
#endif

/* Definitions start here */
struct Vector;
VECTOR_DEFINE_PANIC(vector)

VECTOR_INLINE void vector_assert(const struct Vector *vec)
{
	if (vec->begin == NULL) {
		assert(vec->end == NULL && vec->end_of_storage == NULL);
		return;
	}

	assert(vec->end && vec->end_of_storage);
	assert(vec->begin <= vec->end && vec->end <= vec->end_of_storage);
}

void vector_grow(struct Vector *vec, size_t desired)
{
	size_t old_size = 0;
	SampleType *new_begin = NULL;

	if (vec == NULL) {
		if (VECTOR_NO_PANIC_ON_NULL) {
			return;
		}
		vector_panic(
			"Null passed to vector_grow but non-null argument expected.");
	}
	vector_assert(vec);

	if (vec->begin) {
		if (VECTOR_CAPACITY(vec) == desired) {
			return;
		}
		if (VECTOR_CAPACITY(vec) > desired) {
			vector_panic("Vector shrinking not supported.");
		}
	}

	old_size = VECTOR_SIZE(vec);

	new_begin = VECTOR_REALLOC(vec->begin, desired * sizeof(SampleType));
	if (new_begin == NULL) {
		vector_panic("Out of memory. Panic.");
	}

	vec->begin = new_begin;
	vec->end = new_begin + old_size;
	vec->end_of_storage = new_begin + desired;
}

void vector_free(struct Vector *vec)
{
	if (vec == NULL) {
		if (VECTOR_NO_PANIC_ON_NULL) {
			return;
		}
		vector_panic(
			"Null passed to vector_free but non-null argument expected.");
	}

	vector_assert(vec);

	VECTOR_FREE(vec->begin);
	vec->begin = NULL;
	vec->end = NULL;
	vec->end_of_storage = NULL;
}

void vector_init(struct Vector *vec, size_t capacity)
{
	if (vec == NULL) {
		if (VECTOR_NO_PANIC_ON_NULL) {
			return;
		}
		vector_panic(
			"Null passed to vector_init but non-null argument expected.");
	}

	if (vec->begin || vec->end || vec->end_of_storage) {
		vector_panic(
			"Uninitialized garbage memory, cannot initialize.");
	}

	if (capacity == 0) {
		return;
	}

	vec->begin = VECTOR_REALLOC(NULL, capacity * sizeof(SampleType));
	if (vec->begin == NULL) {
		vector_panic("Out of memory. Panic.");
	}

	vec->end = vec->begin;
	vec->end_of_storage = vec->begin + capacity;

	vector_assert(vec);
}

void vector_push(struct Vector *vec, SampleType value)
{
	if (vec == NULL) {
		if (VECTOR_NO_PANIC_ON_NULL) {
			return;
		}
		vector_panic(
			"Null passed to vector_push but non-null argument expected.");
	}

	vector_assert(vec);

	if (vec->begin == NULL) {
		vector_init(vec, VECTOR_DEFAULT_CAPACITY);
	}

	if (VECTOR_SIZE(vec) >= VECTOR_CAPACITY(vec)) {
		vector_grow(vec, VECTOR_CAPACITY(vec) * VECTOR_GROWTH_FACTOR);
	}

	vec->end[0] = value;
	vec->end++;
}

SampleType vector_pop(struct Vector *vec)
{
	SampleType nothing = { 0 };
	SampleType ret = { 0 };

	if (vec == NULL) {
		if (VECTOR_NO_PANIC_ON_NULL) {
			return nothing;
		}
		vector_panic(
			"Null passed to vector_pop but non-null argument expected.");
	}
	vector_assert(vec);

	if (VECTOR_IS_SIZE_ZERO(vec)) {
		vector_panic("Cannot pop from empty vector.");
	}

	ret = vec->end[-1];
	vec->end--;

	return ret;
}

SampleType vector_get(const struct Vector *vec, size_t idx)
{
	SampleType nothing = { 0 };

	if (vec == NULL) {
		if (VECTOR_NO_PANIC_ON_NULL) {
			return nothing;
		}
		vector_panic(
			"Null passed to vector_get but non-null argument expected.");
	}
	vector_assert(vec);

	if (idx >= VECTOR_SIZE(vec)) {
		vector_panic("Out of range.");
	}

	return vec->begin[idx];
}

void vector_set(struct Vector *vec, size_t idx, SampleType value)
{
	if (vec == NULL) {
		if (VECTOR_NO_PANIC_ON_NULL) {
			return;
		}
		vector_panic(
			"Null passed to vector_set but non-null argument expected.");
	}
	vector_assert(vec);

	if (idx >= VECTOR_SIZE(vec)) {
		vector_panic("Out of range.");
	}

	vec->begin[idx] = value;
}

void vector_insert(struct Vector *vec, size_t idx, SampleType value)
{
	SampleType *middle = NULL;
	size_t delete_size = 0;
	size_t capacity = 0;

	if (vec == NULL) {
		if (VECTOR_NO_PANIC_ON_NULL) {
			return;
		}
		vector_panic(
			"Null passed to vector_insert but non-null argument expected.");
	}
	vector_assert(vec);

	if (idx > VECTOR_SIZE(vec)) {
		vector_panic("Out of range.");
	}

	capacity = VECTOR_CAPACITY(vec);
	if (VECTOR_SIZE(vec) >= capacity) {
		/* Set a minimum multiplicand of 1 */
		vector_grow(vec, (capacity | (capacity == 0)) *
					 VECTOR_GROWTH_FACTOR);
	}

	if (vec->begin + idx == vec->end) {
		vec->end[0] = value;
		vec->end++;
		return;
	}

	middle = vec->begin + idx;
	delete_size = (vec->end - middle) * sizeof(SampleType);
	memmove(middle + 1, middle, delete_size);
	vec->end++;
	middle[0] = value;
}

void vector_delete(struct Vector *vec, size_t idx)
{
	SampleType *middle = NULL;
	size_t delete_size = 0;

	if (vec == NULL) {
		if (VECTOR_NO_PANIC_ON_NULL) {
			return;
		}
		vector_panic(
			"Null passed to vector_delete but non-null argument expected.");
	}
	vector_assert(vec);

	if (idx >= VECTOR_SIZE(vec)) {
		vector_panic("Out of range.");
	}

	/* Delete last element */
	if (idx == VECTOR_SIZE(vec) - 1) {
		vec->end--;
		return;
	}

	middle = vec->begin + idx;
	delete_size = (vec->end - middle - 1) * sizeof(SampleType);
	memmove(middle, middle + 1, delete_size);
	vec->end--;
}

void vector_duplicate(struct Vector *RESTRICT dest, const struct Vector *RESTRICT src)
{
	if (dest == NULL || src == NULL) {
		if (VECTOR_NO_PANIC_ON_NULL) {
			return;
		}
		vector_panic(
			"Null passed to vector_duplicate but non-null argument expected.");
	}
	vector_assert(src);

	if (VECTOR_CAPACITY(src) == 0) {
		dest->begin = NULL;
		dest->end = NULL;
		dest->end_of_storage = NULL;
		return;
	}

	dest->begin =
		VECTOR_REALLOC(NULL, VECTOR_CAPACITY(src) * sizeof(SampleType));
	if (dest->begin == NULL) {
		vector_panic("Out of memory.");
	}

	dest->end = dest->begin + VECTOR_SIZE(src);
	dest->end_of_storage = dest->begin + VECTOR_CAPACITY(src);

	memcpy(dest->begin, src->begin, VECTOR_SIZE(src) * sizeof(SampleType));

	vector_assert(dest);
}

void vector_clear(struct Vector *vec)
{
	if (vec == NULL) {
		if (VECTOR_NO_PANIC_ON_NULL) {
			return;
		}
		vector_panic(
			"Null passed to vector_clear but non-null argument expected.");
	}
	vector_assert(vec);

	vec->end = vec->begin;
}
/* Definitions stop here */

/****************************************************************************
 * Copyright (C) 2025 by Roland Marchand <roland.marchand@protonmail.com>   *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED “AS IS” AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

#endif /* VECTOR_H */
