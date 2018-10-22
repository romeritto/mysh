#include "return5.h"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

static void test_return5(void **state) {
    (void) state; /* unused */
    assert_int_equal(return5(), 5);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_return5),
    };
    
    return cmocka_run_group_tests(tests, NULL, NULL);
}
