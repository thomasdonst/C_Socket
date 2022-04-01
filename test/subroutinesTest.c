#include "include/subroutines.h"
#include "include/configuration.h"
#include "include/minunit.h"
#include <stdio.h>


void before(void) {
}

void after(void) {

}

void fetchCommandTest(char *message, char *expectedType, char *expectedArg1, char *expectedArg2){
    char type[MAX_ENTRY_SIZE], arg1[MAX_ENTRY_SIZE], arg2[MAX_ENTRY_SIZE];
    type[0] = '\0';
    arg1[0] = '\0';
    arg2[0] = '\0';

    fetchCommand(message, type, arg1, arg2);
    mu_assert_string_eq(expectedType, type);
    mu_assert_string_eq(expectedArg1, arg1);
    mu_assert_string_eq(expectedArg2, arg2);
}
MU_TEST(fetchCommandTests) {
    char m[] = "";
    fetchCommandTest(m, "", "", "");

    char m2[] = "   A  B C ";
    fetchCommandTest(m2, "A", "B", "C");

    char m3[] = "A \0 B C";
    fetchCommandTest(m3, "A", "", "");

    char m4[] = " AAA    B C   D";
    fetchCommandTest(m4, "AAA", "B", "C");

//    char m5[] = "  ";
//    fetchCommandTest(m5, "AAA", "B", "C");
}


MU_TEST_SUITE(test_suite) {
    MU_SUITE_CONFIGURE(&before, &after);
    MU_RUN_TEST(fetchCommandTests);
}

int main() {
    MU_RUN_SUITE(test_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}

