#include <cmocka_extensions.h>
#include <test.h>

void test_testme_function(void **state)
{
	assert_int_equal(testme(), 42);
}

void test_success(void **state)
{
	(void)state;
	assert_int_equal(2, 2);
}

int main(int argc, char *argv[])
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_testme_function),
		cmocka_unit_test(test_success),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}
