# Vector - Type-Safe Dynamic Arrays for C

A production-ready, macro-based vector library to generate type-safe dynamic arrays.

```c
#include "vector.h"

VECTOR_DECLARE(IntVector, int_vector, int)
VECTOR_DEFINE(IntVector, int_vector, int)

int main(void) {
	IntVector nums = {0};

	int_vector_push(&nums, 42);
	int_vector_push(&nums, 13);

	printf("First: %d\n", int_vector_get(&nums, 0));  /* 42 */
	printf("Size: %zu\n", VECTOR_SIZE(&nums));        /* 2 */

	int_vector_free(&nums);
	return 0;
}
```

## Features

- **Type-safe**: Generate vectors for any type
- **Portable**: C89 compatible, tested on GCC/Clang/MSVC/ICX across x86_64/ARM64
- **Robust**: Comprehensive bounds checking and memory management
- **Configurable**: Custom allocators, null-pointer policies
- **Zero dependencies**: Just standard C library

## Quick Start

1. Include the header and declare your vector type:
```c
#include "vector.h"
VECTOR_DECLARE(MyVector, my_vector, float)
```

2. Define the implementation (usually in a .c file):
```c
VECTOR_DEFINE(MyVector, my_vector, float)
```

3. Use it:
```c
MyVector v = {0};
my_vector_push(&v, 3.14f);
my_vector_push(&v, 2.71f);

/* Fast iteration */
for (float *f = v.begin; f != v.end; f++) {
	printf("%.2f ", *f);
}

my_vector_free(&v);
```

## API Overview

- `VECTOR_SIZE(vec)` - Get element count
- `VECTOR_CAPACITY(vec)` - Get allocated capacity
- `vector_push(vec, value)` - Append element (O(1) amortized)
- `vector_pop(vec)` - Remove and return last element
- `vector_get(vec, idx)` / `vector_set(vec, idx, value)` - Random access
- `vector_insert(vec, idx, value)` / `vector_delete(vec, idx)` - Insert/remove at index
- `vector_clear(vec)` - Remove all elements
- `vector_free(vec)` - Deallocate memory

## Configuration

Define before including the library:

```c
#define VECTOR_NO_PANIC_ON_NULL 1       /* Return silently on NULL instead of panic */
#define VECTOR_REALLOC my_realloc       /* Custom allocator */
#define VECTOR_FREE my_free             /* Custom deallocator */
```

## Testing

```bash
mkdir build
cmake -S . -B build/ -DCMAKE_BUILD_TYPE=Debug
cd build
make test
```

Tests cover normal operation, edge cases, out-of-memory conditions, and null pointer handling.

## Why `vector.h` Over [stb_ds.h](https://github.com/nothings/stb/blob/master/stb_ds.h)

- **Just as convenient**: both are single-header libraries
- **Better type safety**: `stb_ds.h` includes no compile-time type checking, unlike `vector.h`
- **Faster iteration**: `stb_ds.h` relies on index calculation, while `vector.h` follows `std::vector`'s approach of direct pointer access
- **More reliable memory**: `stb_ds.h` relies on undefined behavior to hide its header behind the data, not `vector.h`
- **Easier to debug**: `stb_ds.h` hides header data in debugging environment, while `vector.h` exposes its simple pointers internal structure
- **No memory corruption**: `stb_ds.h` silently corrupts your memory on out-of-bounds access, while `vector.h` fails fast and lets the developer know by panicking
- **Better licensing**: `stb_ds.h` offers either MIT or public domain, while `vector.h` offers BSD0, which is less restrictive than MIT (no attribution, allows for re-licensing) and more universal than public domain

## Contribution

Contributors and library hackers should work on `vector.in.h` instead of
`vector.h`. It is a version of the library with hardcoded types and function
names. To generate the final library from it, run `libgen.py`.

## License

This library is licensed under the BSD Zero license.
