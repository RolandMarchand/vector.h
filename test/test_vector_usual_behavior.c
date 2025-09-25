#include "unity/unity.h"
#define VECTOR_LONG_JUMP_NO_ABORT
#include "vector_generated.h"

jmp_buf abort_jmp;

void setUp(void)
{
}

void tearDown(void)
{
}

void test_shrink_abort(void)
{
	Vector vec = {0};
	vector_init(&vec, 10);

	if (setjmp(abort_jmp) == 0) {
		vector_grow(&vec, 5);
	} else {
		vector_free(&vec);
		return;
	}

	vector_free(&vec);
	TEST_FAIL();
}

void test_grow_from_zero(void)
{
	Vector vec = {0};

	vector_grow(&vec, 5);

	TEST_ASSERT_EQUAL_INT(5, VECTOR_CAPACITY(&vec));
	TEST_ASSERT_EQUAL_INT(0, VECTOR_SIZE(&vec));
	TEST_ASSERT(vec.begin == vec.end);
	TEST_ASSERT_NOT_NULL(vec.begin);
	TEST_ASSERT_NOT_NULL(vec.end);
	TEST_ASSERT_NOT_NULL(vec.end_of_storage);

	vector_free(&vec);
}

void test_grow_0(void)
{
	Vector vec = {0};
	Vector vec_copy = {0};

	vector_init(&vec, 10);
	memcpy(&vec_copy, &vec, sizeof(Vector));

	vector_grow(&vec, 10);
	
	TEST_ASSERT_EQUAL_MEMORY(&vec_copy, &vec, sizeof(Vector));

	vector_free(&vec);
}

void test_grow(void)
{
	Vector vec = {0};
	int idx = 0;
	vector_init(&vec, 10);

	vec.end += 3; /* Dummy elements */

	for (idx = 11; idx < 1000; idx++) {
		vector_grow(&vec, idx);
		TEST_ASSERT_EQUAL_INT(idx, VECTOR_CAPACITY(&vec));
		TEST_ASSERT_EQUAL_INT(3, VECTOR_SIZE(&vec));
	}

	vector_free(&vec);
}

void test_free(void)
{
	Vector vec = {0};
	Vector vec_zero = {0};
	vector_init(&vec, 10);
	vector_free(&vec);
	TEST_ASSERT_EQUAL_MEMORY(&vec_zero, &vec, sizeof(Vector));
}

void test_free_zero(void)
{
	Vector vec = {0};
	Vector vec_zero = {0};
	vector_free(&vec);
	TEST_ASSERT_EQUAL_MEMORY(&vec_zero, &vec, sizeof(Vector));
}

void test_init_zero(void)
{
	Vector vec = {0};
	Vector vec_copy = {0};

	memcpy(&vec_copy, &vec, sizeof(Vector));

	vector_init(&vec, 0);

	TEST_ASSERT_EQUAL_MEMORY(&vec_copy, &vec, sizeof(Vector));
}

void test_init_garbage(void)
{
	Vector vec = {0};

	vec.begin = (void*)0xDEADBEEF;
	vec.end = (void*)((char*)0xDEADBEEF + 1);
	vec.end_of_storage = (void*)((char*)0xDEADBEEF + 2);

	if (setjmp(abort_jmp) == 0) {
		vector_init(&vec, 5);
	} else {
		return;
	}

	TEST_FAIL();	
}

void test_init(void)
{
	Vector vec = {0};
	vector_init(&vec, 5);

	TEST_ASSERT_EQUAL_INT(5, VECTOR_CAPACITY(&vec));
	TEST_ASSERT_EQUAL_INT(0, VECTOR_SIZE(&vec));
	TEST_ASSERT(vec.begin == vec.end);
	TEST_ASSERT_NOT_NULL(vec.begin);
	TEST_ASSERT_NOT_NULL(vec.end);
	TEST_ASSERT_NOT_NULL(vec.end_of_storage);

	vector_free(&vec);
}

void test_push_from_zero(void)
{
	Vector vec = {0};
	vector_push(&vec, 8);
	TEST_ASSERT_EQUAL_INT(VECTOR_DEFAULT_CAPACITY, VECTOR_CAPACITY(&vec));
	TEST_ASSERT_EQUAL_INT(1, VECTOR_SIZE(&vec));
	TEST_ASSERT_EQUAL_INT(8, vector_get(&vec, 0));
	TEST_ASSERT_NOT_NULL(vec.begin);
	TEST_ASSERT_NOT_NULL(vec.end);
	TEST_ASSERT_NOT_NULL(vec.end_of_storage);
	vector_free(&vec);
}

void test_push(void)
{
	Vector vec = {0};
	int idx = 0;
	vector_init(&vec, VECTOR_DEFAULT_CAPACITY);

	for (idx = 0; idx < VECTOR_DEFAULT_CAPACITY; idx++) {
		vector_push(&vec, idx);
		TEST_ASSERT_EQUAL_INT(idx + 1, VECTOR_SIZE(&vec));
		TEST_ASSERT_EQUAL_INT(VECTOR_DEFAULT_CAPACITY,
				  VECTOR_CAPACITY(&vec));
		TEST_ASSERT_EQUAL_INT(idx, vector_get(&vec, idx));
	}

	vector_push(&vec, -1);

	TEST_ASSERT_EQUAL_INT(VECTOR_DEFAULT_CAPACITY + 1, VECTOR_SIZE(&vec));
	TEST_ASSERT_EQUAL_INT(VECTOR_DEFAULT_CAPACITY * VECTOR_GROWTH_FACTOR,
			  VECTOR_CAPACITY(&vec));
	TEST_ASSERT_EQUAL_INT(-1, vector_get(&vec, VECTOR_DEFAULT_CAPACITY));

	vector_free(&vec);
}

void test_pop_zero(void)
{
	Vector vec = {0};

	if (setjmp(abort_jmp) == 0) {
		vector_pop(&vec);
	} else {
		return;
	}

	TEST_FAIL();
}

void test_pop(void)
{
	Vector vec = {0};
	int idx = 0;
	
	for (idx = 0; idx < 1000; idx++) {
		vector_push(&vec, idx);
		TEST_ASSERT_EQUAL_INT(idx, vector_pop(&vec));
		vector_push(&vec, idx);
	}

	vector_free(&vec);
}

void test_get_out_of_range(void)
{
	Vector vec = {0};
	
	if (setjmp(abort_jmp) == 0) {
		vector_get(&vec, 0);
	} else {
		if (setjmp(abort_jmp) == 0) {
			vector_get(&vec, -1);
		} else {
			if (setjmp(abort_jmp) == 0) {
				vector_get(&vec, 1);
			} else {
				return;
			}
		}
	}

	TEST_FAIL();
}

void test_get(void)
{
	Vector vec = {0};
	int idx = 0;
	int jdx = 0;
	
	for (idx = 0; idx < 1000; idx++) {
		vector_push(&vec, idx);
		for (jdx = 0; jdx <= idx; jdx++) {
			TEST_ASSERT_EQUAL_INT(jdx, vector_get(&vec, jdx));
		}
	}

	vector_free(&vec);
}

void test_set_out_of_range(void)
{
	Vector vec = {0};
	
	if (setjmp(abort_jmp) == 0) {
		vector_set(&vec, 0, 100);
	} else {
		if (setjmp(abort_jmp) == 0) {
			vector_set(&vec, -1, 100);
		} else {
			if (setjmp(abort_jmp) == 0) {
				vector_set(&vec, 1, 100);
			} else {
				return;
			}
		}
	}

	vector_free(&vec);
	TEST_FAIL();
}

void test_set(void)
{
	Vector vec = {0};
	int idx = 0;
	int jdx = 0;

	for (idx = 0; idx < 1000; idx++) {
		vector_push(&vec, idx);
		vector_set(&vec, idx, idx * 2);
		for (jdx = 0; jdx <= idx; jdx++) {
			TEST_ASSERT_EQUAL_INT(jdx * 2, vector_get(&vec, jdx));
		}
	}

	vector_free(&vec);
}

void test_insert_from_zero(void)
{
	Vector vec = {0};
	int idx = 0;
	
	for(; idx < 100; idx++) {
		vector_insert(&vec, idx, idx);

		TEST_ASSERT_EQUAL_INT(idx, vector_get(&vec, idx));
		TEST_ASSERT_EQUAL_INT(idx + 1, VECTOR_SIZE(&vec));
		TEST_ASSERT_NOT_NULL(vec.begin);
	}

	vector_free(&vec);
}

void test_insert_start(void)
{
	Vector vec = {0};
	int idx = 0;
	int jdx = 0;

	for (idx = 0; idx < 100; idx++) {
		vector_push(&vec, idx);
	}
	
	for (idx = 0; idx < 100; idx++) {
		vector_insert(&vec, 0, idx + 100);

		TEST_ASSERT_EQUAL_INT(idx + 100, vector_get(&vec, 0));
		TEST_ASSERT_EQUAL_INT(idx + 101, VECTOR_SIZE(&vec));
		TEST_ASSERT_NOT_NULL(vec.begin);

		/* Test after the insert point */
		for (jdx = 0; jdx < 100; jdx++) {
			TEST_ASSERT_EQUAL_INT(jdx, vector_get(&vec, idx + jdx + 1));
		}
	}

	vector_free(&vec);
}

void test_insert_middle(void)
{
	Vector vec = {0};
	int idx = 0;
	int jdx = 0;

	for (idx = 0; idx < 100; idx++) {
		vector_push(&vec, idx);
	}
	
	for(idx = 0; idx < 100; idx++) {
		vector_insert(&vec, 50, idx + 100);

		TEST_ASSERT_EQUAL_INT(idx + 100, vector_get(&vec, 50));
		TEST_ASSERT_EQUAL_INT(idx + 101, VECTOR_SIZE(&vec));
		TEST_ASSERT_NOT_NULL(vec.begin);

		/* Test before the insert point */
		for (jdx = 0; jdx < 50; jdx++) {
			TEST_ASSERT_EQUAL_INT(jdx, vector_get(&vec, jdx));
		}

		/* Test after the insert point */
		for (jdx = 0; jdx < 50; jdx++) {
			TEST_ASSERT_EQUAL_INT(jdx + 50, vector_get(&vec, jdx + idx + 51));
		}
	}

	vector_free(&vec);
}

void test_insert_end(void)
{
	Vector vec = {0};
	int idx = 0;
	int jdx = 0;

	for (idx = 0; idx < 100; idx++) {
		vector_push(&vec, idx);
	}
	
	for (idx = 0; idx < 100; idx++) {
		vector_insert(&vec, VECTOR_SIZE(&vec), idx + 100);

		TEST_ASSERT_EQUAL_INT(idx + 100, vector_get(&vec, VECTOR_SIZE(&vec) - 1));
		TEST_ASSERT_EQUAL_INT(idx + 101, VECTOR_SIZE(&vec));
		TEST_ASSERT_NOT_NULL(vec.begin);

		/* Test before the insert point */
		for (jdx = 0; jdx < 100; jdx++) {
			TEST_ASSERT_EQUAL_INT(jdx, vector_get(&vec, jdx));
		}
	}

	vector_free(&vec);	
}

void test_insert_out_of_range(void)
{
	Vector vec = {0};
	int idx = 0;

	for (idx = 0; idx < 100; idx++) {
		vector_push(&vec, idx);
	}
	
	if (setjmp(abort_jmp) == 0) {
		vector_insert(&vec, 101, 100);
	} else {
		if (setjmp(abort_jmp) == 0) {
			vector_insert(&vec, -1, 100);
		} else {
			vector_free(&vec);
			return;
		}
	}

	vector_free(&vec);
	TEST_FAIL();
}

void test_insert_out_of_range_from_zero(void)
{
	Vector vec = {0};
	
	if (setjmp(abort_jmp) == 0) {
		vector_insert(&vec, 1, 100);
	} else {
		if (setjmp(abort_jmp) == 0) {
			vector_insert(&vec, -1, 100);
		} else {
			return;
		}
	}

	vector_free(&vec);
	TEST_FAIL();
}

void test_delete(void)
{
	Vector vec = {0};
	int idx = 0;
	int jdx = 0;
	size_t capacity = 0;

	for (idx = 0; idx < 100; idx++) {
		vector_push(&vec, idx);
	}

	capacity = VECTOR_CAPACITY(&vec);

	for (idx = 0; idx < 100; idx++) {
		vector_delete(&vec, 0);

		TEST_ASSERT_EQUAL_INT(99 - idx, VECTOR_SIZE(&vec));
		TEST_ASSERT_EQUAL_INT(capacity, VECTOR_CAPACITY(&vec));
		TEST_ASSERT_NOT_NULL(vec.begin);

		/* Test after the delete point */
		for (jdx = 0; jdx < 99 - idx; jdx++) {
			TEST_ASSERT_EQUAL_INT(jdx + idx + 1, vector_get(&vec, jdx));
		}
	}

	vector_free(&vec);
}

void test_delete_half(void)
{
	Vector vec = {0};
	int idx = 0;
	int jdx = 0;
	size_t capacity = 0;

	for (idx = 0; idx < 100; idx++) {
		vector_push(&vec, idx);
	}

	capacity = VECTOR_CAPACITY(&vec);

	for (idx = 0; idx < 50; idx++) {
		vector_delete(&vec, 49);
		TEST_ASSERT_EQUAL_INT(idx + 50, vector_get(&vec, 49));
		TEST_ASSERT_EQUAL_INT(99 - idx, VECTOR_SIZE(&vec));
		TEST_ASSERT_EQUAL_INT(capacity, VECTOR_CAPACITY(&vec));
		TEST_ASSERT_NOT_NULL(vec.begin);

		/* Test before the delete point */
		for (jdx = 0; jdx < 49; jdx++) {
			TEST_ASSERT_EQUAL_INT(jdx, vector_get(&vec, jdx));
		}

		/* Test after the delete point */
		for (jdx = 50; jdx < 99 - idx; jdx++) {
			TEST_ASSERT_EQUAL_INT(jdx + idx + 1, vector_get(&vec, jdx));
		}
	}

	vector_free(&vec);
}

void test_delete_last(void)
{
	Vector vec = {0};
	int idx = 0;
	int jdx = 0;
	size_t capacity = 0;

	for (idx = 0; idx < 100; idx++) {
		vector_push(&vec, idx);
	}

	capacity = VECTOR_CAPACITY(&vec);

	for (idx = 99; idx >= 0; idx--) {
		vector_delete(&vec, idx);

		TEST_ASSERT_EQUAL_INT(idx, VECTOR_SIZE(&vec));
		TEST_ASSERT_EQUAL_INT(capacity, VECTOR_CAPACITY(&vec));
		TEST_ASSERT_NOT_NULL(vec.begin);

		/* Test before the delete point */
		for (jdx = 0; jdx < idx; jdx++) {
			TEST_ASSERT_EQUAL_INT(jdx, vector_get(&vec, jdx));
		}
	}

	vector_free(&vec);	
}

void test_delete_out_of_range(void)
{
	Vector vec = {0};
	int idx = 0;
	
	for (idx = 0; idx < 100; idx++) {
		vector_push(&vec, idx);
	}
	
	if (setjmp(abort_jmp) == 0) {
		vector_delete(&vec, 101);
	} else {
		if (setjmp(abort_jmp) == 0) {
			vector_delete(&vec, -1);
		} else {
			vector_free(&vec);
			return;
		}
	}

	vector_free(&vec);
	TEST_FAIL();
}

void test_delete_out_of_range_from_zero(void)
{
	Vector vec = {0};
	
	if (setjmp(abort_jmp) == 0) {
		vector_delete(&vec, 0);
	} else {
		if (setjmp(abort_jmp) == 0) {
			vector_delete(&vec, -1);
		} else {
			if (setjmp(abort_jmp) == 0) {
				vector_delete(&vec, 0);
			} else {
				return;
			}
		}
	}

	vector_free(&vec);
	TEST_FAIL();
}

void test_duplicate_from_zero(void)
{
	Vector src = {0};
	Vector dest = {0};
	Vector expected = {0};

	/* Garbage to be overridden */
	dest.begin = (void*)0xBEEF;
	dest.end = (void*)((char*)0xBEEF + 1);
	dest.end_of_storage = (void*)((char*)0xBEEF + 2);
	
	vector_duplicate(&dest, &src);
	TEST_ASSERT_EQUAL_MEMORY(&expected, &src, sizeof(Vector));
	TEST_ASSERT_EQUAL_MEMORY(&expected, &dest, sizeof(Vector));
	
}

void test_duplicate_to_zero(void)
{
	Vector src = {0};
	Vector dest = {0};

	vector_push(&src, 10);
	vector_duplicate(&dest, &src);

	TEST_ASSERT_EQUAL_INT(10, vector_get(&dest, 0));
	TEST_ASSERT_EQUAL_INT(1, VECTOR_SIZE(&dest));
	TEST_ASSERT_EQUAL_INT(VECTOR_DEFAULT_CAPACITY, VECTOR_CAPACITY(&dest));
	TEST_ASSERT(src.begin != dest.begin);

	vector_free(&src);
	vector_free(&dest);
}

void test_duplicate(void)
{
	Vector src = {0};
	Vector dest = {0};

	/* Garbage to be overridden */
	dest.begin = (void*)0xBEEF;
	dest.end = (void*)((char*)0xBEEF + 1);
	dest.end_of_storage = (void*)((char*)0xBEEF + 2);

	vector_push(&src, 10);
	vector_duplicate(&dest, &src);

	TEST_ASSERT_EQUAL_INT(10, vector_get(&dest, 0));
	TEST_ASSERT_EQUAL_INT(1, VECTOR_SIZE(&dest));
	TEST_ASSERT_EQUAL_INT(VECTOR_DEFAULT_CAPACITY, VECTOR_CAPACITY(&dest));
	TEST_ASSERT(src.begin != dest.begin);

	vector_free(&src);
	vector_free(&dest);
}

void test_clear_zero(void)
{
	Vector vec = {0};
	vector_clear(&vec);
	TEST_ASSERT_NULL(vec.begin);
	TEST_ASSERT_NULL(vec.end);
	TEST_ASSERT_NULL(vec.end_of_storage);
	TEST_ASSERT_EQUAL_INT(0, VECTOR_SIZE(&vec));
	TEST_ASSERT_EQUAL_INT(0, VECTOR_CAPACITY(&vec));
}

void test_clear(void)
{
	Vector vec = {0};
	int idx = 0;

	for (idx = 0; idx < 100; idx++) {
		vector_push(&vec, idx);
	}

	vector_clear(&vec);
	TEST_ASSERT_NOT_NULL(vec.begin);
	TEST_ASSERT_NOT_NULL(vec.end);
	TEST_ASSERT_NOT_NULL(vec.end_of_storage);
	TEST_ASSERT_EQUAL_INT(0, VECTOR_SIZE(&vec));
	TEST_ASSERT_GREATER_THAN(0, VECTOR_CAPACITY(&vec));

	vector_free(&vec);
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_shrink_abort);
	RUN_TEST(test_grow_from_zero);
	RUN_TEST(test_grow_0);
	RUN_TEST(test_grow);
	RUN_TEST(test_free);
	RUN_TEST(test_free_zero);
	RUN_TEST(test_init_zero);
	RUN_TEST(test_init_garbage);
	RUN_TEST(test_init);
	RUN_TEST(test_push_from_zero);
	RUN_TEST(test_push);
	RUN_TEST(test_pop_zero);
	RUN_TEST(test_pop);
	RUN_TEST(test_get_out_of_range);
	RUN_TEST(test_get);
	RUN_TEST(test_set_out_of_range);
	RUN_TEST(test_set);
	RUN_TEST(test_insert_from_zero);
	RUN_TEST(test_insert_start);
	RUN_TEST(test_insert_middle);
	RUN_TEST(test_insert_end);
	RUN_TEST(test_insert_out_of_range);
	RUN_TEST(test_insert_out_of_range_from_zero);
	RUN_TEST(test_delete);
	RUN_TEST(test_delete_half);
	RUN_TEST(test_delete_last);
	RUN_TEST(test_delete_out_of_range);
	RUN_TEST(test_delete_out_of_range_from_zero);
	RUN_TEST(test_duplicate_from_zero);
	RUN_TEST(test_duplicate_to_zero);
	RUN_TEST(test_duplicate);
	RUN_TEST(test_clear_zero);
	RUN_TEST(test_clear);

	return UNITY_END();
}
