#include "unity/unity.h"
#include "vector_generated.h"

void setUp(void)
{
}

void tearDown(void)
{
}

#undef VECTOR_NO_PANIC_ON_NULL

void test_grow_pass_null_ignore(void)
{
	vector_grow(NULL, 0);
}

void test_free_pass_null_ignore(void)
{
	vector_free(NULL);
}

void test_init_pass_null_ignore(void)
{
	vector_init(NULL, 0);
}

void test_push_pass_null_ignore(void)
{
	vector_push(NULL, 0);
}

void test_pop_pass_null_ignore(void)
{
	vector_pop(NULL);
}

void test_get_pass_null_ignore(void)
{
	vector_get(NULL, 0);
}

void test_set_pass_null_ignore(void)
{
	vector_set(NULL, 0, 0);
}

void test_insert_pass_null_ignore(void)
{
	vector_insert(NULL, 0, 0);
}

void test_delete_pass_null_ignore(void)
{
	vector_delete(NULL, 0);
}

void test_duplicate_pass_null_ignore_dest(void)
{
	Vector src = { 0 };
	vector_duplicate(NULL, &src);
}

void test_duplicate_pass_null_ignore_src(void)
{
	Vector dest = { 0 };
	vector_duplicate(&dest, NULL);
}

void test_duplicate_pass_null_ignore_both(void)
{
	vector_duplicate(NULL, NULL);
}

void test_clear_pass_null_ignore(void)
{
	vector_clear(NULL);
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_grow_pass_null_ignore);
	RUN_TEST(test_free_pass_null_ignore);
	RUN_TEST(test_init_pass_null_ignore);
	RUN_TEST(test_push_pass_null_ignore);
	RUN_TEST(test_pop_pass_null_ignore);
	RUN_TEST(test_get_pass_null_ignore);
	RUN_TEST(test_set_pass_null_ignore);
	RUN_TEST(test_insert_pass_null_ignore);
	RUN_TEST(test_delete_pass_null_ignore);
	RUN_TEST(test_duplicate_pass_null_ignore_dest);
	RUN_TEST(test_duplicate_pass_null_ignore_src);
	RUN_TEST(test_duplicate_pass_null_ignore_both);
	RUN_TEST(test_clear_pass_null_ignore);

	return UNITY_END();
}
