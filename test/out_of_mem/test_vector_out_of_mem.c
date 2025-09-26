#include "unity/unity.h"
#include "vector_generated.h"

jmp_buf abort_jmp;

void setUp(void)
{
}

void tearDown(void)
{
}

void *return_null(void *ignored1, size_t ignored2)
{
	(void)ignored1;
	(void)ignored2;
	return NULL;
}

void test_grow_out_of_mem(void)
{
	Vector vec = { 0 };

	if (setjmp(abort_jmp) == 0) {
		vector_grow(&vec, 10);
	} else {
		return;
	}

	TEST_FAIL();
}

void test_init_out_of_mem(void)
{
	Vector vec = { 0 };

	if (setjmp(abort_jmp) == 0) {
		vector_init(&vec, 10);
	} else {
		vector_free(&vec);
		return;
	}

	TEST_FAIL();
}

void test_duplicate_out_of_mem(void)
{
	int buffer[10] = { 0 };
	Vector src = { 0 };
	Vector dest = { 0 };

	src.begin = buffer;
	src.end = buffer + 1;
	src.end_of_storage = buffer + 10;

	if (setjmp(abort_jmp) == 0) {
		vector_duplicate(&dest, &src);
	} else {
		return;
	}

	TEST_FAIL();
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_grow_out_of_mem);
	RUN_TEST(test_init_out_of_mem);
	RUN_TEST(test_duplicate_out_of_mem);

	return UNITY_END();
}
