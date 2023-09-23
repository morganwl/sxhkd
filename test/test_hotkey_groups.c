/* Test selective enabling and disabling of hotkey groups */

#include "unity.h"
#include "unity_fixture.h"

void test_successful_test(void) {
    TEST_ASSERT_EQUAL(2, 2);
}

void test_failing_test(void) {
    TEST_ASSERT_EQUAL(2, 3);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_successful_test);
    RUN_TEST(test_failing_test);
    return UNITY_END();
}
