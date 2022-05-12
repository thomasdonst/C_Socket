#include "include/subroutines.h"
#include "include/minunit.h"

#include <stdio.h>
#include <stdlib.h>


void before(void) {
}

void after(void) {

}

MU_TEST(parseCommandTests) {
    Command command;
    Command expectedCommand;
    char message[MESSAGE_BUFFER];

    strcpy(message, "");
    expectedCommand = (Command) {"", "", ""};
    command = parseTelnetCommand(message);
    mu_assert_string_eq(expectedCommand.type, command.type);
    mu_assert_string_eq(expectedCommand.key, command.key);
    mu_assert_string_eq(expectedCommand.value, command.value);

    strcpy(message, " ");
    expectedCommand = (Command) {"", "", ""};
    command = parseTelnetCommand(message);
    mu_assert_string_eq(expectedCommand.type, command.type);
    mu_assert_string_eq(expectedCommand.key, command.key);
    mu_assert_string_eq(expectedCommand.value, command.value);

    strcpy(message, "   ");
    expectedCommand = (Command) {"", "", ""};
    command = parseTelnetCommand(message);
    mu_assert_string_eq(expectedCommand.type, command.type);
    mu_assert_string_eq(expectedCommand.key, command.key);
    mu_assert_string_eq(expectedCommand.value, command.value);

    strcpy(message, "  A B C ");
    expectedCommand = (Command) {"A", "B", "C"};
    command = parseTelnetCommand(message);
    mu_assert_string_eq(expectedCommand.type, command.type);
    mu_assert_string_eq(expectedCommand.key, command.key);
    mu_assert_string_eq(expectedCommand.value, command.value);

    strcpy(message, "A \0 B C ");
    expectedCommand = (Command) {"A", "", ""};
    command = parseTelnetCommand(message);
    mu_assert_string_eq(expectedCommand.type, command.type);
    mu_assert_string_eq(expectedCommand.key, command.key);
    mu_assert_string_eq(expectedCommand.value, command.value);

    strcpy(message, "AAA    B C  D");
    expectedCommand = (Command) {"AAA", "B", "C  D"};
    command = parseTelnetCommand(message);
    mu_assert_string_eq(expectedCommand.type, command.type);
    mu_assert_string_eq(expectedCommand.key, command.key);
    mu_assert_string_eq(expectedCommand.value, command.value);

    strcpy(message, "A");
    expectedCommand = (Command) {"A", "", ""};
    command = parseTelnetCommand(message);
    mu_assert_string_eq(expectedCommand.type, command.type);
    mu_assert_string_eq(expectedCommand.key, command.key);
    mu_assert_string_eq(expectedCommand.value, command.value);

    strcpy(message, "PUT calculation  4 + 1  ");
    expectedCommand = (Command) {"PUT", "calculation", "4 + 1"};
    command = parseTelnetCommand(message);
    mu_assert_string_eq(expectedCommand.type, command.type);
    mu_assert_string_eq(expectedCommand.key, command.key);
    mu_assert_string_eq(expectedCommand.value, command.value);

    strcpy(message, "a a      y = 3*2 (3) +1");
    expectedCommand = (Command) {"a", "a", "y = 3*2 (3) +1"};
    command = parseTelnetCommand(message);
    mu_assert_string_eq(expectedCommand.type, command.type);
    mu_assert_string_eq(expectedCommand.key, command.key);
    mu_assert_string_eq(expectedCommand.value, command.value);

    strcpy(message, "put c 2233 / 22 * 3 - 212");
    expectedCommand = (Command) {"put", "c", "2233 / 22 * 3 - 212"};
    command = parseTelnetCommand(message);
    mu_assert_string_eq(expectedCommand.type, command.type);
    mu_assert_string_eq(expectedCommand.key, command.key);
    mu_assert_string_eq(expectedCommand.value, command.value);

    strcpy(message, "put / 33 2 3");
    expectedCommand = (Command) {"put", "/", "33 2 3"};
    command = parseTelnetCommand(message);
    mu_assert_string_eq(expectedCommand.type, command.type);
    mu_assert_string_eq(expectedCommand.key, command.key);
    mu_assert_string_eq(expectedCommand.value, command.value);
}

MU_TEST_SUITE(test_suite) {
    MU_SUITE_CONFIGURE(&before, &after);
    MU_RUN_TEST(parseCommandTests);
}

int main() {
    MU_RUN_SUITE(test_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}
