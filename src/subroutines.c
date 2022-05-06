#include "include/subroutines.h"
#include "include/keyValueStore.h"

#include "stdio.h"
#include "ctype.h"
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdbool.h>

Command parseCommand(char *message) {
    Command command = {"", "", ""};
    if (strcmp(message, "") == 0)
        return command;

    if (containsOnlySpaceCharacters(message) == 1)
        return command;

    char messageCopy[MESSAGE_BUFFER];
    messageCopy[0] = '\0';
    sprintf(messageCopy, "%s", message);
    char *token;
    char delimiter[] = " ";

    char *rest = messageCopy;

    // fetch command type
    strncpy(command.type, strtok_r(rest, delimiter, &rest), MAX_ARGUMENT_LENGTH);
    command.type[MAX_ARGUMENT_LENGTH - 1] = '\0';

    // fetch command key
    if ((token = strtok_r(rest, delimiter, &rest)) != NULL) {
        strncpy(command.key, token, MAX_ARGUMENT_LENGTH);
        command.key[MAX_ARGUMENT_LENGTH - 1] = '\0';
    }

    // fetch command value
    char *trimmedValue = trim(rest);
    strncpy(command.value, trimmedValue, MAX_ARGUMENT_LENGTH);
    command.value[MAX_ARGUMENT_LENGTH - 1] = '\0';

    return command;
}

void processCommand(Command command, char *result) {
    if (hasAccess() == 0 && strcmp(command.type, "quit") != 0) {
        sprintf(result, "> Exclusive access: Command is not permitted");
        return;
    }

    result[0] = '\0';
    toLower(command.type);

    if (strcmp(command.type, "get") == 0 && strcmp(command.key, "") != 0 && strcmp(command.value, "") == 0)
        handleGet(command, result);

    else if (strcmp(command.type, "put") == 0 && strcmp(command.key, "") != 0 && strcmp(command.value, "") != 0)
        handlePut(command, result);

    else if (strcmp(command.type, "del") == 0 && strcmp(command.key, "") != 0 && strcmp(command.value, "") == 0)
        handleDel(command, result);

    else if (strcmp(command.type, "show") == 0 && strcmp(command.key, "") == 0 && strcmp(command.value, "") == 0)
        handleShow(command, result);

    else if (strcmp(command.type, "beg") == 0 && strcmp(command.key, "") == 0 && strcmp(command.value, "") == 0)
        handleBeg(result);

    else if (strcmp(command.type, "end") == 0 && strcmp(command.key, "") == 0 && strcmp(command.value, "") == 0)
        handleEnd(result);

    else if (strcmp(command.type, "sub") == 0 && strcmp(command.key, "") != 0 && strcmp(command.value, "") == 0)
        handleSub(command, result);

    else if (strcmp(command.type, "unsub") == 0 && strcmp(command.key, "") != 0 && strcmp(command.value, "") == 0)
        handleUnsub(command, result);

    else if (strcmp(command.type, "op") == 0 && strcmp(command.key, "") != 0 && strcmp(command.value, "") != 0)
        handleOp(command, result);

    else if (strcmp(command.type, "quit") == 0 && strcmp(command.key, "") == 0 && strcmp(command.value, "") == 0)
        sprintf(result, "%s", "> Connection closed by foreign host");

    else
        sprintf(result, "%s", "> Unknown command");
}

void handleGet(Command command, char *result) {
    sem_t *keySemaphor = sem_open(command.key, O_CREAT, 0777, 1);
    sem_wait(keySemaphor);

    get(command.key, result); // Critical section

    sem_post(keySemaphor);
}

void handlePut(Command command, char *result) {
    if (containsSlash(command.key) == 1) {
        sprintf(result, "> PUT:invalid_key");
        return;
    }

    sem_t *keySemaphor = sem_open(command.key, O_CREAT, 0777, 1);
    sem_wait(keySemaphor);

    int keyModified = put(command.key, command.value, result); // Critical section

    sem_post(keySemaphor);

    if (keyModified == 1)
        notifySubscribers(command.key, result);
}

void handleDel(Command command, char *result) {
    sem_t *keySemaphor = sem_open(command.key, O_CREAT, 0777, 1);
    sem_wait(keySemaphor);

    int keyDeleted = del(command.key, result); // Critical section

    sem_post(keySemaphor);

    if (keyDeleted == 1)
        notifySubscribers(command.key, result);
}

void handleShow(Command command, char *result) {
    sem_t *semaphor = sem_open(command.type, O_CREAT, 0777, 1);
    sem_wait(semaphor);

    show(result); // Critical section

    sem_post(semaphor);
}

void handleBeg(char *result) {
    sem_t *semaphor = sem_open("beg_end", O_CREAT, 0777, 1);
    sem_wait(semaphor);

    beg(result); // Critical section

    sem_post(semaphor);
}

void handleEnd(char *result) {
    sem_t *semaphor = sem_open("beg_end", O_CREAT, 0777, 1);
    sem_wait(semaphor);

    end(result); // Critical section

    sem_post(semaphor);
}

void handleSub(Command command, char *result) {
    key_t *semaphor = sem_open("sub_unsub", O_CREAT, 0777, 1);
    sem_wait(semaphor);

    sub(command.key, result); // Critical section

    sem_post(semaphor);
}

void handleUnsub(Command command, char *result) {
    key_t *semaphor = sem_open("sub_unsub", O_CREAT, 0777, 1);
    sem_wait(semaphor);

    unsub(command.key, result); // Critical section

    sem_post(semaphor);
}

void handleOp(Command command, char *result) {
    op(command, result);
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

int containsSlash(char *string) {
    int i = 0;
    while (string[i] != '\0') {
        if (string[i] == '/')
            return 1;
        i++;
    }

    return 0;
}

char *ltrim(char *string) {
    while (isspace(*string)) string++;
    return string;
}

char *rtrim(char *string) {
    char *back = string + strlen(string);
    while (isspace(*--back));
    *(back + 1) = '\0';
    return string;
}

char *trim(char *string) {
    return rtrim(ltrim(string));
}

int hasAccess() {
    return *exclusiveAccess == getpid() || *exclusiveAccess == 0;
}