#include "include/subroutines.h"
#include "include/configuration.h"
#include "include/minunit.h"
#include <stdio.h>


void before(void) {
}

void after(void) {

}

MU_TEST(fetchCommandTests) {
    Command command;
    Command expectedCommand;
    char message[MAX_ENTRY_SIZE];

    strcpy(message, "");
    expectedCommand = (Command) {"", "", ""};
    command = fetchCommand(message);
    mu_assert_string_eq(expectedCommand.type, command.type);
    mu_assert_string_eq(expectedCommand.key, command.key);
    mu_assert_string_eq(expectedCommand.value, command.value);

    strcpy(message, " ");
    expectedCommand = (Command) {"", "", ""};
    command = fetchCommand(message);
    mu_assert_string_eq(expectedCommand.type, command.type);
    mu_assert_string_eq(expectedCommand.key, command.key);
    mu_assert_string_eq(expectedCommand.value, command.value);

    strcpy(message, "   ");
    expectedCommand = (Command) {"", "", ""};
    command = fetchCommand(message);
    mu_assert_string_eq(expectedCommand.type, command.type);
    mu_assert_string_eq(expectedCommand.key, command.key);
    mu_assert_string_eq(expectedCommand.value, command.value);

    strcpy(message, "  A B C ");
    expectedCommand = (Command) {"A", "B", "C"};
    command = fetchCommand(message);
    mu_assert_string_eq(expectedCommand.type, command.type);
    mu_assert_string_eq(expectedCommand.key, command.key);
    mu_assert_string_eq(expectedCommand.value, command.value);

    strcpy(message, "A \0 B C ");
    expectedCommand = (Command) {"A", "", ""};
    command = fetchCommand(message);
    mu_assert_string_eq(expectedCommand.type, command.type);
    mu_assert_string_eq(expectedCommand.key, command.key);
    mu_assert_string_eq(expectedCommand.value, command.value);

    strcpy(message, "AAA    B C  D");
    expectedCommand = (Command) {"AAA", "B", "C"};
    command = fetchCommand(message);
    mu_assert_string_eq(expectedCommand.type, command.type);
    mu_assert_string_eq(expectedCommand.key, command.key);
    mu_assert_string_eq(expectedCommand.value, command.value);
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

