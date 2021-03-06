#include "include/subroutines.h"
#include "include/keyValueStore.h"

#include "stdio.h"
#include "ctype.h"
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>

Command parseTelnetCommand(char *message) {
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

    // parse command type
    strncpy(command.type, strtok_r(rest, delimiter, &rest), MAX_ARGUMENT_LENGTH);
    command.type[MAX_ARGUMENT_LENGTH - 1] = '\0';

    // parse command key
    if ((token = strtok_r(rest, delimiter, &rest)) != NULL) {
        strncpy(command.key, token, MAX_ARGUMENT_LENGTH);
        command.key[MAX_ARGUMENT_LENGTH - 1] = '\0';
    }

    // parse command value
    char *trimmedValue = trim(rest);
    strncpy(command.value, trimmedValue, MAX_ARGUMENT_LENGTH);
    command.value[MAX_ARGUMENT_LENGTH - 1] = '\0';

    return command;
}


Command parseHttpCommand(char *message) {
    Command command = (Command) {"", "", ""};

    char tmp[255];
    tmp[0] = '\0';

    int length = 7;

    char messageCopy[MESSAGE_BUFFER];
    messageCopy[0] = '\0';
    sprintf(messageCopy, "%s", message);
    char *token;
    char *rest = messageCopy;

    token = strtok_r(rest, "\r", &rest);
    token = strtok_r(token, " ", &rest);
    sprintf(command.type, "%s", token);

    token = strtok_r(rest, " ", &rest);
    substring(token, tmp, length, strlen(token) - length + 1);
    token = strtok_r(tmp, "?", &rest);
    if (token != NULL)
        sprintf(command.key, "%s", token);

    tmp[0] = '\0';
    substring(rest, tmp, length, strlen(rest) - length + 1);
    sprintf(command.value, "%s", tmp);

    return command;
}

void processCommand(Command command, char *result) {
    if (hasAccess() == 0 && strcmp(command.type, "quit") != 0) {
        sprintf(result, "> Exclusive access: Command is not permitted");
        return;
    }

    result[0] = '\0';
    sprintf(result, "> Unknown Command");
    toLower(command.type);

    if (isValidCommand(command, (Command) {"get", "!", ""})) {
        handleGet(command, result);
        return;
    }

    if (isValidCommand(command, (Command) {"put", "!", "!"})) {
        handlePut(command, result);
        return;
    }

    if (isValidCommand(command, (Command) {"del", "!", ""})) {
        handleDel(command, result);
        return;
    }

    if (isValidCommand(command, (Command) {"show", "", ""})) {
        handleShow(result);
        return;
    }

    if (isValidCommand(command, (Command) {"beg", "", ""})) {
        handleBeg(result);
        return;
    }

    if (isValidCommand(command, (Command) {"end", "", ""})) {
        handleEnd(result);
        return;
    }

    if (isValidCommand(command, (Command) {"sub", "!", ""})) {
        handleSub(command, result);
        return;
    }

    if (isValidCommand(command, (Command) {"unsub", "!", ""})) {
        handleUnsub(command, result);
        return;
    }

    if (isValidCommand(command, (Command) {"op", "!", "!"})) {
        handleOp(command, result);
        return;
    }

    if (isValidCommand(command, (Command) {"quit", "", ""})) {
        sprintf(result, "%s", "> Connection closed by foreign host");
        return;
    }
}


void handleGet(Command command, char *result) {
    if (containsSlash(command.key) == 1) {
        sprintf(result, "> GET:invalid_key");
        return;
    }

    sem_t *semaphor = sem_open("keyValueStore", O_CREAT, 0777, 1);
    sem_wait(semaphor);

    if (hasWildCard(command.key) == 1)
        getWithWildCard(command.key, result); // Critical section
    else
        get(command.key, result); // Critical section

    sem_post(semaphor);
}

void handlePut(Command command, char *result) {
    if (containsSlash(command.key) == 1) {
        sprintf(result, "> PUT:invalid_key");
        return;
    }

    sem_t *semaphor = sem_open("keyValueStore", O_CREAT, 0777, 1);
    sem_wait(semaphor);

    int keyModified = put(command.key, command.value, result); // Critical section

    sem_post(semaphor);

    if (keyModified == 1)
        notifySubscribers(command.key, result);
}

void handleDel(Command command, char *result) {
    if (containsSlash(command.key) == 1) {
        sprintf(result, "> DEL:invalid_key");
        return;
    }

    int keyDeleted;
    sem_t *semaphor = sem_open("keyValueStore", O_CREAT, 0777, 1);
    sem_wait(semaphor);

    if (hasWildCard(command.key) == 1)
        keyDeleted = delWithWildCard(command.key, result); // Critical section
    else
        keyDeleted = del(command.key, result); // Critical section

    sem_post(semaphor);

    if (keyDeleted == 1)
        notifySubscribers(command.key, result);
}

void handleShow(char *result) {
    sem_t *semaphor = sem_open("keyValueStore", O_CREAT, 0777, 1);
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

void substring(char s[], char sub[], int p, int l) {
    int c = 0;

    while (c < l) {
        sub[c] = s[p + c - 1];
        c++;
    }
    sub[c] = '\0';
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

int isValidCommand(Command is, Command should) {
    return strcmp(is.type, should.type) == 0
           && strlen(is.key) > 0 == strlen(should.key)
           && strlen(is.value) > 0 == strlen(should.value);
}
