#include "unity/unity.h"
#include "vector_generated.h"

jmp_buf abort_jmp;

void setUp(void)
{
}

void tearDown(void)
{
}

void test_get(void)
{
	Vector vec = { 0 };
	int gotten = 0;

	vector_init(&vec, 10);

	if (setjmp(abort_jmp) == 0) {
		gotten = vector_get(&vec, 10000);
	} else {
		TEST_FAIL();
	}

	TEST_ASSERT_EQUAL_INT(0, gotten);

	if (setjmp(abort_jmp) == 0) {
		gotten = vector_get(&vec, 10);
	} else {
		TEST_FAIL();
	}

	TEST_ASSERT_EQUAL_INT(0, gotten);

	if (setjmp(abort_jmp) == 0) {
		gotten = vector_get(&vec, -1);
	} else {
		TEST_FAIL();
	}

	TEST_ASSERT_EQUAL_INT(0, gotten);

	vector_free(&vec);
}

void test_set(void)
{
	Vector vec = { 0 };

	vector_init(&vec, 10);

	if (setjmp(abort_jmp) == 0) {
		vector_set(&vec, 10000, 100);
	} else {
		TEST_FAIL();
	}

	if (setjmp(abort_jmp) == 0) {
		vector_set(&vec, 10, 100);
	} else {
		TEST_FAIL();
	}

	if (setjmp(abort_jmp) == 0) {
		vector_set(&vec, -1, 100);
	} else {
		TEST_FAIL();
	}

	vector_free(&vec);
}

void test_insert(void)
{
	Vector vec = { 0 };
	int idx = 0;

	for (idx = 0; idx < 10; idx++) {
		vector_push(&vec, idx);
	}

	if (setjmp(abort_jmp) == 0) {
		vector_insert(&vec, 10000, 100);
	} else {
		TEST_FAIL();
	}

	if (setjmp(abort_jmp) == 0) {
		vector_insert(&vec, 10, 100);
	} else {
		TEST_FAIL();
	}

	if (setjmp(abort_jmp) == 0) {
		vector_insert(&vec, -1, 100);
	} else {
		TEST_FAIL();
	}

	for (idx = 0; idx < 10; idx++) {
		TEST_ASSERT_EQUAL_INT(idx, vector_get(&vec, idx));
	}

	vector_free(&vec);
}

void test_delete(void)
{
	Vector vec = { 0 };
	int idx = 0;

	vector_init(&vec, 10);

	for (idx = 0; idx < 10; idx++) {
		vector_push(&vec, idx);
	}

	if (setjmp(abort_jmp) == 0) {
		vector_delete(&vec, 10000);
	} else {
		TEST_FAIL();
	}

	if (setjmp(abort_jmp) == 0) {
		vector_delete(&vec, 10);
	} else {
		TEST_FAIL();
	}

	if (setjmp(abort_jmp) == 0) {
		vector_delete(&vec, -1);
	} else {
		TEST_FAIL();
	}

	for (idx = 0; idx < 10; idx++) {
		TEST_ASSERT_EQUAL_INT(idx, vector_get(&vec, idx));
	}

	vector_free(&vec);
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_get);
	RUN_TEST(test_set);
	RUN_TEST(test_insert);
	RUN_TEST(test_delete);

	return UNITY_END();
}
