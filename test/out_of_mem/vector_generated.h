#ifndef VECTOR_GENERATED_H
#define VECTOR_GENERATED_H

#include <stdlib.h>

void *return_null(void *ignored1, size_t ignored2);

#define VECTOR_REALLOC(p, s) (return_null((p), (s)))
#define VECTOR_FREE(p) (free((p)))
#define VECTOR_LONG_JUMP_NO_ABORT

#include "vector.h"

VECTOR_DECLARE(Vector, vector, int)

#endif /* VECTOR_GENERATED_H */
