#include "stdio.h"
#include "ctype.h"
#include "include/subroutines.h"
#include <string.h>
#include "include/configuration.h"
#include "include/keyValueStore.h"

int containsOnlySpaceCharacters(char *string) {
    for (int n = 0; string[n] != '\0'; ++n) {
        if (string[n] != ' ')
            return 0;
    }
    return 1;
}

Command fetchCommand(char *message) {
    Command command = {"", "", ""};
    if (strcmp(message, "") == 0)
        return command;

    if (containsOnlySpaceCharacters(message) == 1)
        return command;

    char messageCopy[MAX_ENTRY_SIZE];
    sprintf(messageCopy, "%s", message);
    char *token;
    char delimiter[] = " ";

    strcpy(command.type, strtok(messageCopy, delimiter));
    if ((token = strtok(NULL, delimiter)) != NULL)
        strcpy(command.key, token);
    if ((token = strtok(NULL, delimiter)) != NULL)
        strcpy(command.value, token);

    return command;
}

void executeCommand(Command command, char *result) {
    result[0] = '\0';
    toLower(command.type);

    if (strcmp(command.type, "get") == 0 && strcmp(command.key, "") != 0 && strcmp(command.value, "") == 0)
        get(command.key, result);

    else if (strcmp(command.type, "put") == 0 && strcmp(command.key, "") != 0 && strcmp(command.value, "") != 0)
        put(command.key, command.value, result);

    else if (strcmp(command.type, "del") == 0 && strcmp(command.key, "") != 0 && strcmp(command.value, "") == 0)
        del(command.key, result);

    else if (strcmp(command.type, "show") == 0 && strcmp(command.key, "") == 0 && strcmp(command.value, "") == 0)
        show(result);

    else if (strcmp(command.type, "quit") == 0 && strcmp(command.key, "") == 0 && strcmp(command.value, "") == 0)
        sprintf(result, "%s", "> Connection closed by foreign host");

    else
        sprintf(result, "%s", "> Unknown command");
}

void toLower(char *string) {
    int i = 0;
    while (string[i] != '\0') {
        string[i] = tolower(string[i]);
        i++;
    }
}

int isAlphanumeric(char *string) {
    if (string[0] == '\0')
        return 0;

    int i = 0;
    while (string[i] != '\0') {
        if (!isalnum(string[i]))
            return 0;
        i++;
    }

    return 1;
}
