#include "stdio.h"
#include "ctype.h"
#include "include/subroutines.h"
#include <string.h>
#include <sys/shm.h>
#include <stdlib.h>
#include "include/configuration.h"
//#include "include/keyValueStore.h"

int *number;

//Entry *storage;
int sharedMemoryId;

void initializeSharedMemory() {
//    if ((sharedMemoryId = shmget(IPC_PRIVATE, sizeof(storage) * KEY_VALUE_STORE_SIZE, IPC_CREAT | 0777)) == -1) {
//        perror("shared memory get failed");
//        exit(EXIT_FAILURE);
//    }
//
//    storage = (Entry *) shmat(sharedMemoryId, 0, 0);
//    if (storage == (Entry *) (-1)) {
//        perror("shared memory attach failed");
//        exit(EXIT_FAILURE);
//    }

    if ((sharedMemoryId = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0600)) == -1) {
        perror("shared memory get failed");
        exit(EXIT_FAILURE);
    }

    number = (int *) shmat(sharedMemoryId, 0, 0);
    if (number == (int *) (-1)) {
        perror("shared memory attach failed");
        exit(EXIT_FAILURE);
    }
}

void closeSharedMemory() {
//    shmdt(storage);
//    shmctl(sharedMemoryId, IPC_RMID, 0);

    shmdt(number);
    shmctl(sharedMemoryId, IPC_RMID, 0);
}

void increment() {
    *number += 1;

    char tmp[MAX_ENTRY_SIZE];
    tmp[0] = '\0';
}
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

    strncpy(command.type, strtok(messageCopy, delimiter), MAX_ARGUMENT_LENGTH);
    if ((token = strtok(NULL, delimiter)) != NULL)
        strncpy(command.key, token, MAX_ARGUMENT_LENGTH);
    if ((token = strtok(NULL, delimiter)) != NULL)
        strncpy(command.value, token, MAX_ARGUMENT_LENGTH);

    return command;
}

void processCommand(Command command, char *result) {
    result[0] = '\0';
    toLower(command.type);

    // toDo(): Remove later
    if (strcmp(command.type, "add") == 0 && strcmp(command.key, "") == 0 && strcmp(command.value, "") == 0){
        result[0] = '+';
        result[1] = '\0';
        increment();
    }
    // toDo(): end

//    if (strcmp(command.type, "get") == 0 && strcmp(command.key, "") != 0 && strcmp(command.value, "") == 0)
//        get(command.key, result);
//
//    else if (strcmp(command.type, "put") == 0 && strcmp(command.key, "") != 0 && strcmp(command.value, "") != 0)
//        put(command.key, command.value, result);
//
//    else if (strcmp(command.type, "del") == 0 && strcmp(command.key, "") != 0 && strcmp(command.value, "") == 0)
//        del(command.key, result);
//
//    else if (strcmp(command.type, "show") == 0 && strcmp(command.key, "") == 0 && strcmp(command.value, "") == 0)
//        show(result);
//
//    else if (strcmp(command.type, "quit") == 0 && strcmp(command.key, "") == 0 && strcmp(command.value, "") == 0)
//        sprintf(result, "%s", "> Connection closed by foreign host");
//
//    else
//        sprintf(result, "%s", "> Unknown command");
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
