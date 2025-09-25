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
 * Vector is optimized for iteration, here is an example:
 * for (int *i = v.begin; i != v.end; i++)
 * 
 * To configure this library, either #define the symbols before including
 * the library.
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
 *   configuration options, must be defined before including the library. */

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

#if defined(__GNUC__) || defined(__clang__)
#define VECTOR_AUTO __attribute__((cleanup(vector_free)))
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

typedef int Vector_Custom_Type_;

struct Vector;
typedef struct Vector Vector;

VECTOR_NORETURN void vector_panic(const char *message);
void vector_assert(const Vector *vec);
/* Desired is in element count */
void vector_grow(Vector *vec, size_t desired);
void vector_free(Vector *vec);
/* Capacity is in element count */
void vector_init(Vector *vec, size_t capacity);
void vector_push(Vector *vec, Vector_Custom_Type_ value);
Vector_Custom_Type_ vector_pop(Vector *vec);
Vector_Custom_Type_ vector_get(const Vector *vec, size_t idx);
void vector_set(Vector *vec, size_t idx, Vector_Custom_Type_ value);
void vector_insert(Vector *vec, size_t idx, Vector_Custom_Type_ value);
void vector_delete(Vector *vec, size_t idx);
void vector_duplicate(Vector *RESTRICT dest, const Vector *RESTRICT src);
void vector_clear(Vector *vec);

struct Vector {
	Vector_Custom_Type_ *begin;
	Vector_Custom_Type_ *end;
	Vector_Custom_Type_ *end_of_storage;
};

enum { VECTOR_DEFAULT_CAPACITY = 8, VECTOR_GROWTH_FACTOR = 2 };

VECTOR_NORETURN void vector_panic(const char *message)
{
	assert(message);
#ifdef VECTOR_LONG_JUMP_NO_ABORT
	longjmp(abort_jmp, 1);
#endif
	(void)fprintf(stderr, "%s\n", message);
	abort();
}

VECTOR_INLINE void vector_assert(const Vector *vec)
{
	if (vec->begin == NULL) {
		assert(vec->end == NULL);
		assert(vec->end_of_storage == NULL);
	} else {
		assert(vec->end);
		assert(vec->end_of_storage);
		assert(vec->begin <= vec->end);
		assert(vec->end <= vec->end_of_storage);
	}
}

void vector_grow(Vector *vec, size_t desired)
{
	size_t old_size = 0;
	Vector_Custom_Type_ *new_begin = NULL;

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

	new_begin = VECTOR_REALLOC(vec->begin, desired * sizeof(Vector_Custom_Type_));
	if (new_begin == NULL) {
		vector_panic("Out of memory. Panic.");
	}

	vec->begin = new_begin;
	vec->end = new_begin + old_size;
	vec->end_of_storage = new_begin + desired;
}

void vector_free(Vector *vec)
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

void vector_init(Vector *vec, size_t capacity)
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

	vec->begin = VECTOR_REALLOC(NULL, capacity * sizeof(Vector_Custom_Type_));
	if (vec->begin == NULL) {
		vector_panic("Out of memory. Panic.");
	}

	vec->end = vec->begin;
	vec->end_of_storage = vec->begin + capacity;

	vector_assert(vec);
}

void vector_push(Vector *vec, Vector_Custom_Type_ value)
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

Vector_Custom_Type_ vector_pop(Vector *vec)
{
	Vector_Custom_Type_ nothing = { 0 };
	Vector_Custom_Type_ ret = { 0 };

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

Vector_Custom_Type_ vector_get(const Vector *vec, size_t idx)
{
	Vector_Custom_Type_ nothing = { 0 };

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

void vector_set(Vector *vec, size_t idx, Vector_Custom_Type_ value)
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

void vector_insert(Vector *vec, size_t idx, Vector_Custom_Type_ value)
{
	Vector_Custom_Type_ *middle = NULL;
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
		vector_grow(vec, (capacity | (capacity == 0))
			    * VECTOR_GROWTH_FACTOR);
	}

	if (vec->begin + idx == vec->end) {
		vec->end[0] = value;
		vec->end++;
		return;
	}

	middle = vec->begin + idx;
	delete_size = (vec->end - middle) * sizeof(Vector_Custom_Type_);
	memmove(middle + 1, middle, delete_size);
	vec->end++;
	middle[0] = value;
}

void vector_delete(Vector *vec, size_t idx)
{
	Vector_Custom_Type_ *middle = NULL;
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
	delete_size = (vec->end - middle - 1) * sizeof(Vector_Custom_Type_);
	memmove(middle, middle + 1, delete_size);
	vec->end--;
}

void vector_duplicate(Vector *RESTRICT dest, const Vector *RESTRICT src)
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

	dest->begin = VECTOR_REALLOC(NULL, VECTOR_CAPACITY(src) * sizeof(Vector_Custom_Type_));
	if (dest->begin == NULL) {
		vector_panic("Out of memory.");
	}

	dest->end = dest->begin + VECTOR_SIZE(src);
	dest->end_of_storage = dest->begin + VECTOR_CAPACITY(src);

	memcpy(dest->begin, src->begin, VECTOR_SIZE(src) * sizeof(Vector_Custom_Type_));

	vector_assert(dest);
}

void vector_clear(Vector *vec)
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

/****************************************************************************
 * Copyright (C) 2025 by Roland Marchand <roland.marchand@protonmail.com>   *
 * 									    *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.			    *
 * 									    *
 * THE SOFTWARE IS PROVIDED “AS IS” AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF	    *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.	    *
 ****************************************************************************/

#endif /* VECTOR_H */
