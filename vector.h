#ifndef VECTOR_H
#define VECTOR_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Library to generate type-safe vector types.
 *
 * This library is portable (tested on GCC/Clang/MSVC/ICX, x86/x86_64/ARM64,
 * all warnings and pedantic) and is C89 compatible.
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
 * - VECTOR_NO_PANIC_ON_NULL (default 0): if true (1), does not panic upon
 *   passing NULL to vector functions. Otherwise, panic.
 *
 * - VECTOR_NO_PANIC_ON_OOB (default 0): if true (1), does not panic upon
 *   getting, setting, inserting, or deleting out of bounds. Those operations
 *   become no-ops. Otherwise, panic.
 *
 * - VECTOR_NO_PANIC_ON_OVERFLOW (default 0): if true(1), does not panic on
 *   setting capacities that would cause an unsigned integer overflow. Those
 *   operations become no-ops. Otherwise, panic.
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

#ifndef VECTOR_NO_PANIC_ON_OOB
#define VECTOR_NO_PANIC_ON_OOB 0
#endif

#ifndef VECTOR_NO_PANIC_ON_OVERFLOW
#define VECTOR_NO_PANIC_ON_OVERFLOW 0
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
#elif defined(_MSC_VER)
#define VECTOR_NORETURN __declspec(noreturn)
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

#define VECTOR_DECLARE(Struct_Name_, Functions_Prefix_, Custom_Type_)\
\
typedef struct Struct_Name_ {\
	Custom_Type_ *begin;\
	Custom_Type_ *end;\
	Custom_Type_ *end_of_storage;\
} Struct_Name_;\
\
VECTOR_NORETURN void Functions_Prefix_##_panic(const char *message);\
void Functions_Prefix_##_assert(const Struct_Name_ *vec);\
void Functions_Prefix_##_grow(Struct_Name_ *vec, size_t desired);\
void Functions_Prefix_##_free(Struct_Name_ *vec);\
void Functions_Prefix_##_init(Struct_Name_ *vec, size_t capacity);\
void Functions_Prefix_##_push(Struct_Name_ *vec, Custom_Type_ value);\
Custom_Type_ Functions_Prefix_##_pop(Struct_Name_ *vec);\
Custom_Type_ Functions_Prefix_##_get(const Struct_Name_ *vec, size_t idx);\
void Functions_Prefix_##_set(Struct_Name_ *vec, size_t idx, Custom_Type_ value);\
void Functions_Prefix_##_insert(Struct_Name_ *vec, size_t idx, Custom_Type_ value);\
void Functions_Prefix_##_delete(Struct_Name_ *vec, size_t idx);\
void Functions_Prefix_##_duplicate(Struct_Name_ *RESTRICT dest, const Struct_Name_ *RESTRICT src);\
void Functions_Prefix_##_clear(Struct_Name_ *vec);

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

#define VECTOR_DEFINE(Struct_Name_, Functions_Prefix_, Custom_Type_)\
struct Struct_Name_;\
VECTOR_DEFINE_PANIC(Functions_Prefix_)\
\
VECTOR_INLINE void Functions_Prefix_##_assert(const struct Struct_Name_ *vec)\
{\
	if (vec->begin == NULL) {\
		assert(vec->end == NULL && vec->end_of_storage == NULL);\
		return;\
	}\
\
	assert(vec->end && vec->end_of_storage);\
	assert(vec->begin <= vec->end && vec->end <= vec->end_of_storage);\
}\
\
void Functions_Prefix_##_grow(struct Struct_Name_ *vec, size_t desired)\
{\
	size_t old_size = 0;\
	Custom_Type_ *new_begin = NULL;\
\
	if (vec == NULL) {\
		if (VECTOR_NO_PANIC_ON_NULL) {\
			return;\
		}\
		Functions_Prefix_##_panic(\
			"Null passed to "#Functions_Prefix_"_grow but non-null argument expected.");\
	}\
	Functions_Prefix_##_assert(vec);\
\
	if (desired != 0 && sizeof(Custom_Type_) > ((size_t)-1) / desired) {\
		if (VECTOR_NO_PANIC_ON_OVERFLOW) {\
			return;\
		}\
		Functions_Prefix_##_panic("Requested capacity would cause size overflow.");\
	}\
\
	if (vec->begin) {\
		if (VECTOR_CAPACITY(vec) == desired) {\
			return;\
		}\
		if (VECTOR_CAPACITY(vec) > desired) {\
			Functions_Prefix_##_panic(""#Struct_Name_" shrinking not supported.");\
		}\
	}\
\
	old_size = VECTOR_SIZE(vec);\
\
	new_begin = VECTOR_REALLOC(vec->begin, desired * sizeof(Custom_Type_));\
	if (new_begin == NULL) {\
		Functions_Prefix_##_panic("Out of memory. Panic.");\
	}\
\
	vec->begin = new_begin;\
	vec->end = new_begin + old_size;\
	vec->end_of_storage = new_begin + desired;\
}\
\
void Functions_Prefix_##_free(struct Struct_Name_ *vec)\
{\
	if (vec == NULL) {\
		if (VECTOR_NO_PANIC_ON_NULL) {\
			return;\
		}\
		Functions_Prefix_##_panic(\
			"Null passed to "#Functions_Prefix_"_free but non-null argument expected.");\
	}\
\
	Functions_Prefix_##_assert(vec);\
\
	VECTOR_FREE(vec->begin);\
	vec->begin = NULL;\
	vec->end = NULL;\
	vec->end_of_storage = NULL;\
}\
\
void Functions_Prefix_##_init(struct Struct_Name_ *vec, size_t capacity)\
{\
	if (vec == NULL) {\
		if (VECTOR_NO_PANIC_ON_NULL) {\
			return;\
		}\
		Functions_Prefix_##_panic(\
			"Null passed to "#Functions_Prefix_"_init but non-null argument expected.");\
	}\
\
	if (capacity == 0) {\
		return;\
	}\
\
	if (sizeof(Custom_Type_) > ((size_t)-1) / capacity) {\
		if (VECTOR_NO_PANIC_ON_OVERFLOW) {\
			return;\
		}\
		Functions_Prefix_##_panic("Requested capacity would cause size overflow.");\
	}\
\
	vec->begin = VECTOR_REALLOC(NULL, capacity * sizeof(Custom_Type_));\
	if (vec->begin == NULL) {\
		Functions_Prefix_##_panic("Out of memory. Panic.");\
	}\
\
	vec->end = vec->begin;\
	vec->end_of_storage = vec->begin + capacity;\
\
	Functions_Prefix_##_assert(vec);\
}\
\
void Functions_Prefix_##_push(struct Struct_Name_ *vec, Custom_Type_ value)\
{\
	if (vec == NULL) {\
		if (VECTOR_NO_PANIC_ON_NULL) {\
			return;\
		}\
		Functions_Prefix_##_panic(\
			"Null passed to "#Functions_Prefix_"_push but non-null argument expected.");\
	}\
\
	Functions_Prefix_##_assert(vec);\
\
	if (vec->begin == NULL) {\
		Functions_Prefix_##_init(vec, VECTOR_DEFAULT_CAPACITY);\
	}\
\
	if (VECTOR_SIZE(vec) >= VECTOR_CAPACITY(vec)) {\
		Functions_Prefix_##_grow(vec, VECTOR_CAPACITY(vec) * VECTOR_GROWTH_FACTOR);\
	}\
\
	vec->end[0] = value;\
	vec->end++;\
}\
\
Custom_Type_ Functions_Prefix_##_pop(struct Struct_Name_ *vec)\
{\
	Custom_Type_ nothing = { 0 };\
	Custom_Type_ ret = { 0 };\
\
	if (vec == NULL) {\
		if (VECTOR_NO_PANIC_ON_NULL) {\
			return nothing;\
		}\
		Functions_Prefix_##_panic(\
			"Null passed to "#Functions_Prefix_"_pop but non-null argument expected.");\
	}\
	Functions_Prefix_##_assert(vec);\
\
	if (VECTOR_IS_SIZE_ZERO(vec)) {\
		Functions_Prefix_##_panic("Cannot pop from empty "#Functions_Prefix_".");\
	}\
\
	ret = vec->end[-1];\
	vec->end--;\
\
	return ret;\
}\
\
Custom_Type_ Functions_Prefix_##_get(const struct Struct_Name_ *vec, size_t idx)\
{\
	Custom_Type_ nothing = { 0 };\
\
	if (vec == NULL) {\
		if (VECTOR_NO_PANIC_ON_NULL) {\
			return nothing;\
		}\
		Functions_Prefix_##_panic(\
			"Null passed to "#Functions_Prefix_"_get but non-null argument expected.");\
	}\
	Functions_Prefix_##_assert(vec);\
\
	if (idx >= VECTOR_SIZE(vec)) {\
		if (VECTOR_NO_PANIC_ON_OOB) {\
			return nothing;\
		}\
		Functions_Prefix_##_panic("Out of range.");\
	}\
\
	return vec->begin[idx];\
}\
\
void Functions_Prefix_##_set(struct Struct_Name_ *vec, size_t idx, Custom_Type_ value)\
{\
	if (vec == NULL) {\
		if (VECTOR_NO_PANIC_ON_NULL) {\
			return;\
		}\
		Functions_Prefix_##_panic(\
			"Null passed to "#Functions_Prefix_"_set but non-null argument expected.");\
	}\
	Functions_Prefix_##_assert(vec);\
\
	if (idx >= VECTOR_SIZE(vec)) {\
		if (VECTOR_NO_PANIC_ON_OOB) {\
			return;\
		}\
		Functions_Prefix_##_panic("Out of range.");\
	}\
\
	vec->begin[idx] = value;\
}\
\
void Functions_Prefix_##_insert(struct Struct_Name_ *vec, size_t idx, Custom_Type_ value)\
{\
	Custom_Type_ *middle = NULL;\
	size_t delete_size = 0;\
	size_t capacity = 0;\
\
	if (vec == NULL) {\
		if (VECTOR_NO_PANIC_ON_NULL) {\
			return;\
		}\
		Functions_Prefix_##_panic(\
			"Null passed to "#Functions_Prefix_"_insert but non-null argument expected.");\
	}\
	Functions_Prefix_##_assert(vec);\
\
	if (idx > VECTOR_SIZE(vec)) {\
		if (VECTOR_NO_PANIC_ON_OOB) {\
			return;\
		}\
		Functions_Prefix_##_panic("Out of range.");\
	}\
\
	capacity = VECTOR_CAPACITY(vec);\
	if (VECTOR_SIZE(vec) >= capacity) {\
		/* Set a minimum multiplicand of 1 */\
		Functions_Prefix_##_grow(vec, (capacity | (capacity == 0)) *\
					 VECTOR_GROWTH_FACTOR);\
	}\
\
	if (vec->begin + idx == vec->end) {\
		vec->end[0] = value;\
		vec->end++;\
		return;\
	}\
\
	middle = vec->begin + idx;\
	delete_size = (vec->end - middle) * sizeof(Custom_Type_);\
	memmove(middle + 1, middle, delete_size);\
	vec->end++;\
	middle[0] = value;\
}\
\
void Functions_Prefix_##_delete(struct Struct_Name_ *vec, size_t idx)\
{\
	Custom_Type_ *middle = NULL;\
	size_t delete_size = 0;\
\
	if (vec == NULL) {\
		if (VECTOR_NO_PANIC_ON_NULL) {\
			return;\
		}\
		Functions_Prefix_##_panic(\
			"Null passed to "#Functions_Prefix_"_delete but non-null argument expected.");\
	}\
	Functions_Prefix_##_assert(vec);\
\
	if (idx >= VECTOR_SIZE(vec)) {\
		if (VECTOR_NO_PANIC_ON_OOB) {\
			return;\
		}\
		Functions_Prefix_##_panic("Out of range.");\
	}\
\
	/* Delete last element */\
	if (idx == VECTOR_SIZE(vec) - 1) {\
		vec->end--;\
		return;\
	}\
\
	middle = vec->begin + idx;\
	delete_size = (vec->end - middle - 1) * sizeof(Custom_Type_);\
	memmove(middle, middle + 1, delete_size);\
	vec->end--;\
}\
\
void Functions_Prefix_##_duplicate(struct Struct_Name_ *RESTRICT dest,\
		      const struct Struct_Name_ *RESTRICT src)\
{\
	if (dest == NULL || src == NULL) {\
		if (VECTOR_NO_PANIC_ON_NULL) {\
			return;\
		}\
		Functions_Prefix_##_panic(\
			"Null passed to "#Functions_Prefix_"_duplicate but non-null argument expected.");\
	}\
	Functions_Prefix_##_assert(src);\
\
	if (VECTOR_CAPACITY(src) == 0) {\
		dest->begin = NULL;\
		dest->end = NULL;\
		dest->end_of_storage = NULL;\
		return;\
	}\
\
	dest->begin =\
		VECTOR_REALLOC(NULL, VECTOR_CAPACITY(src) * sizeof(Custom_Type_));\
	if (dest->begin == NULL) {\
		Functions_Prefix_##_panic("Out of memory.");\
	}\
\
	dest->end = dest->begin + VECTOR_SIZE(src);\
	dest->end_of_storage = dest->begin + VECTOR_CAPACITY(src);\
\
	memcpy(dest->begin, src->begin, VECTOR_SIZE(src) * sizeof(Custom_Type_));\
\
	Functions_Prefix_##_assert(dest);\
}\
\
void Functions_Prefix_##_clear(struct Struct_Name_ *vec)\
{\
	if (vec == NULL) {\
		if (VECTOR_NO_PANIC_ON_NULL) {\
			return;\
		}\
		Functions_Prefix_##_panic(\
			"Null passed to "#Functions_Prefix_"_clear but non-null argument expected.");\
	}\
	Functions_Prefix_##_assert(vec);\
\
	vec->end = vec->begin;\
}

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
