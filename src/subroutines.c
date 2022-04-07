#include "include/subroutines.h"
#include "include/keyValueStore.h"
#include "include/server.h"

#include "stdio.h"
#include "ctype.h"
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>

Command fetchCommand(char *message) {
    Command command = {"", "", ""};
    if (strcmp(message, "") == 0)
        return command;

    if (containsOnlySpaceCharacters(message) == 1)
        return command;

    char messageCopy[BUFFER_LENGTH];
    messageCopy[0] = '\0';
    sprintf(messageCopy, "%s", message);
    char *token;
    char delimiter[] = " ";

    strncpy(command.type, strtok(messageCopy, delimiter), MAX_ARGUMENT_LENGTH);
    command.type[MAX_ARGUMENT_LENGTH - 1] = '\0';
    if ((token = strtok(NULL, delimiter)) != NULL) {
        strncpy(command.key, token, MAX_ARGUMENT_LENGTH);
        command.key[MAX_ARGUMENT_LENGTH - 1] = '\0';
    }
    if ((token = strtok(NULL, delimiter)) != NULL) {
        strncpy(command.value, token, MAX_ARGUMENT_LENGTH);
        command.value[MAX_ARGUMENT_LENGTH - 1] = '\0';
    }

    return command;
}

void processCommand(Command command, char *result) {
    if (hasAccess() == 0 && strcmp(command.type, "quit") != 0) {
        sprintf(result, "> Exclusive access: Command is not permitted");
        return;
    }

    sem_t *semaphor;
    sem_t *keySemaphor;
    result[0] = '\0';
    toLower(command.type);

    if (strcmp(command.type, "get") == 0 && strcmp(command.key, "") != 0 && strcmp(command.value, "") == 0) {
        keySemaphor = sem_open(command.key, O_CREAT, 0777, 1);
        sem_wait(keySemaphor);

        get(command.key, result); // Critical section

        sem_post(keySemaphor);
    }

    else if (strcmp(command.type, "put") == 0 && strcmp(command.key, "") != 0 && strcmp(command.value, "") != 0) {
        keySemaphor = sem_open(command.key, O_CREAT, 0777, 1);
        sem_wait(keySemaphor);

        put(command.key, command.value, result); // Critical section

        sem_post(keySemaphor);
    }

    else if (strcmp(command.type, "del") == 0 && strcmp(command.key, "") != 0 && strcmp(command.value, "") == 0) {
        keySemaphor = sem_open(command.key, O_CREAT, 0777, 1);
        sem_wait(keySemaphor);

        del(command.key, result); // Critical section

        sem_post(keySemaphor);
    }

    else if (strcmp(command.type, "show") == 0 && strcmp(command.key, "") == 0 && strcmp(command.value, "") == 0) {
        semaphor = sem_open(command.type, O_CREAT, 0777, 1);
        sem_wait(semaphor);

        show(result); // Critical section

        sem_post(semaphor);
    }

    else if (strcmp(command.type, "beg") == 0 && strcmp(command.key, "") == 0 && strcmp(command.value, "") == 0) {
        semaphor = sem_open("beg/end", O_CREAT, 0777, 1);
        sem_wait(semaphor);

        beg(result); // Critical section

        sem_post(semaphor);
    }

    else if (strcmp(command.type, "end") == 0 && strcmp(command.key, "") == 0 && strcmp(command.value, "") == 0) {
        semaphor = sem_open("beg/end", O_CREAT, 0777, 1);
        sem_wait(semaphor);

        end(result); // Critical section

        sem_post(semaphor);
    }

    else if (strcmp(command.type, "quit") == 0 && strcmp(command.key, "") == 0 && strcmp(command.value, "") == 0)
        sprintf(result, "%s", "> Connection closed by foreign host");

    else
        sprintf(result, "%s", "> Unknown command");
}

int containsOnlySpaceCharacters(char *string) {
    for (int n = 0; string[n] != '\0'; ++n) {
        if (string[n] != ' ')
            return 0;
    }
    return 1;
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

int hasAccess() {
    return *exclusiveAccess == getpid() || *exclusiveAccess == 0;
}