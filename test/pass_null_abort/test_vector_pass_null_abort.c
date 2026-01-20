#include "unity/unity.h"
#include "vector_generated.h"

jmp_buf abort_jmp;

void setUp(void)
{
}

void tearDown(void)
{
}

void test_grow_pass_null_abort(void)
{
	if (setjmp(abort_jmp) == 0) {
		vector_grow(NULL, 1);
	} else {
		return;
	}
	TEST_FAIL();
}

void test_resize_pass_null_abort(void)
{
	if (setjmp(abort_jmp) == 0) {
		vector_resize(NULL, 1);
	} else {
		return;
	}
	TEST_FAIL();
}

void test_free_pass_null_abort(void)
{
	if (setjmp(abort_jmp) == 0) {
		vector_free(NULL);
	} else {
		return;
	}
	TEST_FAIL();
}

void test_init_pass_null_abort(void)
{
	if (setjmp(abort_jmp) == 0) {
		vector_init(NULL, 0);
	} else {
		return;
	}
	TEST_FAIL();
}

void test_push_pass_null_abort(void)
{
	if (setjmp(abort_jmp) == 0) {
		vector_push(NULL, 0);
	} else {
		return;
	}
	TEST_FAIL();
}

void test_pop_pass_null_abort(void)
{
	if (setjmp(abort_jmp) == 0) {
		vector_pop(NULL);
	} else {
		return;
	}
	TEST_FAIL();
}

void test_get_pass_null_abort(void)
{
	if (setjmp(abort_jmp) == 0) {
		vector_get(NULL, 0);
	} else {
		return;
	}
	TEST_FAIL();
}

void test_set_pass_null_abort(void)
{
	if (setjmp(abort_jmp) == 0) {
		vector_set(NULL, 0, 0);
	} else {
		return;
	}
	TEST_FAIL();
}

void test_insert_pass_null_abort(void)
{
	if (setjmp(abort_jmp) == 0) {
		vector_insert(NULL, 0, 0);
	} else {
		return;
	}
	TEST_FAIL();
}

void test_delete_pass_null_abort(void)
{
	if (setjmp(abort_jmp) == 0) {
		vector_delete(NULL, 0);
	} else {
		return;
	}
	TEST_FAIL();
}

void test_duplicate_pass_null_abort_dest(void)
{
	Vector src;
	if (setjmp(abort_jmp) == 0) {
		vector_duplicate(NULL, &src);
	} else {
		return;
	}
	TEST_FAIL();
}

void test_duplicate_pass_null_abort_src(void)
{
	Vector dest;
	if (setjmp(abort_jmp) == 0) {
		vector_duplicate(&dest, NULL);
	} else {
		return;
	}
	TEST_FAIL();
}

void test_duplicate_pass_null_abort_both(void)
{
	if (setjmp(abort_jmp) == 0) {
		vector_duplicate(NULL, NULL);
	} else {
		return;
	}
	TEST_FAIL();
}

void test_clear_pass_null_abort(void)
{
	if (setjmp(abort_jmp) == 0) {
		vector_clear(NULL);
	} else {
		return;
	}
	TEST_FAIL();
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_grow_pass_null_abort);
	RUN_TEST(test_resize_pass_null_abort);
	RUN_TEST(test_free_pass_null_abort);
	RUN_TEST(test_init_pass_null_abort);
	RUN_TEST(test_push_pass_null_abort);
	RUN_TEST(test_pop_pass_null_abort);
	RUN_TEST(test_get_pass_null_abort);
	RUN_TEST(test_set_pass_null_abort);
	RUN_TEST(test_insert_pass_null_abort);
	RUN_TEST(test_delete_pass_null_abort);
	RUN_TEST(test_duplicate_pass_null_abort_dest);
	RUN_TEST(test_duplicate_pass_null_abort_src);
	RUN_TEST(test_duplicate_pass_null_abort_both);
	RUN_TEST(test_clear_pass_null_abort);

	return UNITY_END();
}
