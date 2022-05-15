#include "include/keyValueStore.h"
#include "include/configuration.h"
#include "include/subroutines.h"

#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/ipc.h>

Entry *storage;
int storageMemoryId;

int *exclusiveAccess;
int exclusiveAccessMemoryId;

Subscription *subscription;
int subscriptionMemoryId;

key_t messageQueue;

void initializeSharedMemories() {
    // key value store
    if ((storageMemoryId = shmget(IPC_PRIVATE, sizeof(storage) * KEY_VALUE_STORE_SIZE,
                                  IPC_CREAT | 0777)) == -1) {
        perror("Shared_Memory_Get for storage failed");
        exit(EXIT_FAILURE);
    }

    // exclusive access (beg/end)
    if ((exclusiveAccessMemoryId = shmget(IPC_PRIVATE, sizeof(int),
                                          IPC_CREAT | 0777)) == -1) {
        perror("Shared_Memory_Get for exclusive access failed");
        exit(EXIT_FAILURE);
    }

    // subscriptions
    if ((subscriptionMemoryId = shmget(IPC_PRIVATE, sizeof(subscription) * SUBSCRIPTION_SIZE,
                                       IPC_CREAT | 0777)) == -1) {
        perror("Shared_Memory_Get for subscription failed");
        exit(EXIT_FAILURE);
    }
    attachClientToSharedMemories();
}

void attachClientToSharedMemories() {
    storage = (Entry *) shmat(storageMemoryId, 0, 0);
    if (storage == (Entry *) (-1)) {
        perror("Shared_Memory_Attach for storage failed");
        exit(EXIT_FAILURE);
    }

    exclusiveAccess = (int *) shmat(exclusiveAccessMemoryId, 0, 0);
    if (exclusiveAccess == (int *) (-1)) {
        perror("Shared_Memory_Attach for exclusive access failed");
        exit(EXIT_FAILURE);
    }

    subscription = (Subscription *) shmat(subscriptionMemoryId, 0, 0);
    if (subscription == (Subscription *) (-1)) {
        perror("Shared_Memory_Attach for subscription failed");
        exit(EXIT_FAILURE);
    }
}

void closeSharedMemories() {
    shmdt(storage);
    shmctl(storageMemoryId, IPC_RMID, 0);

    shmdt(exclusiveAccess);
    shmctl(exclusiveAccessMemoryId, IPC_RMID, 0);

    shmdt(subscription);
    shmctl(subscriptionMemoryId, IPC_RMID, 0);
}

void initializeMessageQueue() {
    if ((messageQueue = msgget(IPC_PRIVATE, 0664 | IPC_CREAT)) == -1) {
        puts("Message_get failed");
        exit(0);
    }
}

void closeMessageQueue() {
    msgctl(messageQueue, IPC_RMID, NULL);
}

void resolveExclusiveAccess() {
    if (getpid() == *exclusiveAccess)
        *exclusiveAccess = 0;
}

int keyValueStoreExists() {
    FILE *file;
    file = fopen(RESTORE_FILE_NAME, "r");

    if (file != NULL) {
        fclose(file);
        return 1;
    }
    else
        return 0;
}

void loadKeyValueStore() {
    FILE *fp;
    if (!keyValueStoreExists(RESTORE_FILE_NAME)) {
        if ((fp = fopen(RESTORE_FILE_NAME, "a")) == NULL)
            printf("Unable to create %s", RESTORE_FILE_NAME);
        fclose(fp);
        return;
    }

    char *line = NULL;
    size_t len = 0;

    if ((fp = fopen(RESTORE_FILE_NAME, "r")) == NULL) {
        printf("Unable to open %s", RESTORE_FILE_NAME);
        exit(EXIT_FAILURE);
    }

    int counter = 0;
    Command command;
    while (getline(&line, &len, fp) != -1) {
        command = parseTelnetCommand(line);
        sprintf(storage[counter].key, "%s", command.key);
        sprintf(storage[counter].value, "%s", command.value);

        counter++;
    }

    fclose(fp);
    if (line)
        free(line);
}

void saveKeyValueStore() {
    FILE *fp;
    char tmp[MESSAGE_BUFFER];

    if ((fp = fopen(RESTORE_FILE_NAME, "w")) == NULL) {
        printf("Unable to open %s", RESTORE_FILE_NAME);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < KEY_VALUE_STORE_SIZE; i++) {
        if (strcmp(storage[i].key, "") != 0) {
            sprintf(tmp, "PUT %s %s\n", storage[i].key, storage[i].value);
            fputs(tmp, fp);
        }
    }
    fclose(fp);
}


int getValueByKey(char *key, char *value) {
    value[0] = '\0';
    for (int i = 0; i < KEY_VALUE_STORE_SIZE; i++) {
        if (strcmp(storage[i].key, key) == 0) {
            sprintf(value, "%s", storage[i].value);
            return 1;
        }
    }
    return 0;
}

int isSubscribed(char *key) {
    for (int i = 0; i < SUBSCRIPTION_SIZE; i++) {
        if (subscription[i].processId == getpid() && strcmp(subscription[i].key, key) == 0)
            return 1;
    }
    return 0;
}

void get(char *key, char *result) {
    for (int i = 0; i < KEY_VALUE_STORE_SIZE; i++) {
        if (strcmp(storage[i].key, key) == 0) {
            sprintf(result, "> GET:%s:%s", key, storage[i].value);
            return;
        }
    }
    sprintf(result, "> GET:%s:key_nonexistent", key);
}

void getWithWildCard(char *key, char *result) {
    char tmp[KEY_VALUE_STORE_SIZE *
             (COUNT_OF_COMMAND_ARGUMENTS * MAX_ARGUMENT_LENGTH + ADDITIONAL_SPACE)];
    char message[KEY_VALUE_STORE_SIZE *
                 (COUNT_OF_COMMAND_ARGUMENTS * MAX_ARGUMENT_LENGTH + ADDITIONAL_SPACE)];

    tmp[0] = '\0';
    message[0] = '\0';

    int counter = 0;
    for (int i = 0; i < KEY_VALUE_STORE_SIZE; i++) {
        if (matchWildCard(key, storage[i].key) == 1 && strcmp("", storage[i].key) != 0) {
            sprintf(tmp, "> GET:%s:%s\r\n", key, storage[i].value);
            strcat(message, tmp);
            counter++;
        }
    }

    if (counter > 0) {
        message[strlen(message) - 2] = '\0';
        sprintf(result, "%s", message);
        return;
    }

    sprintf(result, "> GET:%s:key_nonexistent", key);
}

int put(char *key, char *value, char *result) {
    // Search entry with this key -> Replace value
    char previousValue[MAX_ARGUMENT_LENGTH];
    previousValue[0] = '\0';
    for (int i = 0; i < KEY_VALUE_STORE_SIZE; i++) {
        if (strcmp(storage[i].key, key) == 0) {
            sprintf(previousValue, "%s", storage[i].value);
            sprintf(storage[i].key, "%s", key);
            sprintf(storage[i].value, "%s", value);
            sprintf(result, "> PUT:%s:%s", key, storage[i].value);
            return (strcmp(previousValue, storage[i].value) == 0) ? 0 : 1;
        }
    }

    // No entry with this key was found -> Create entry at next free position
    for (int i = 0; i < KEY_VALUE_STORE_SIZE; i++) {
        if (strcmp(storage[i].key, "\0") == 0) {
            sprintf(storage[i].key, "%s", key);
            sprintf(storage[i].value, "%s", value);
            sprintf(result, "> PUT:%s:%s", key, storage[i].value);
            return 1;
        }
    }
    sprintf(result, "> No entry could be created");
    return 0;
}

int del(char *key, char *result) {
    for (int i = 0; i < KEY_VALUE_STORE_SIZE; i++) {
        if (strcmp(storage[i].key, key) == 0) {
            sprintf(storage[i].key, "%s", "");
            sprintf(storage[i].value, "%s", "");
            sprintf(result, "> DEL:%s:key_deleted", key);
            return 1;
        }
    }
    sprintf(result, "> DEL:%s:key_nonexistent", key);
    return 0;
}

void show(char *result) {
    char tmp[KEY_VALUE_STORE_SIZE *
             (COUNT_OF_COMMAND_ARGUMENTS * MAX_ARGUMENT_LENGTH + ADDITIONAL_SPACE)];
    char message[KEY_VALUE_STORE_SIZE *
                 (COUNT_OF_COMMAND_ARGUMENTS * MAX_ARGUMENT_LENGTH + ADDITIONAL_SPACE)];

    tmp[0] = '\0';
    message[0] = '\0';

    int counter = 0;
    for (int i = 0; i < KEY_VALUE_STORE_SIZE; i++) {
        if (strcmp(storage[i].key, "") != 0) {
            sprintf(tmp, "> Index: %d   Key: %s  Value: %s\r\n", i, storage[i].key, storage[i].value);
            strcat(message, tmp);
            counter++;
        }
    }
    if (counter > 0) {
        message[strlen(message) - 2] = '\0';
        sprintf(result, "%s", message);
        return;
    }

    sprintf(result, "%s", "> Key value store is empty");
}

void beg(char *result) {
    if (*exclusiveAccess == getpid()) {
        sprintf(result, "%s", "> BEG:already_used");
        return;
    }

    *exclusiveAccess = getpid();
    sprintf(result, "%s", "> BEG");
}

void end(char *result) {
    if (*exclusiveAccess == 0) {
        sprintf(result, "%s", "> END:use_BEG_first");
        return;
    }

    *exclusiveAccess = 0;
    sprintf(result, "%s", "> END");
}

void sub(char *key, char *result) {
    char value[MAX_ARGUMENT_LENGTH];
    if (getValueByKey(key, value) == 0) {
        sprintf(result, "%s", "> SUB:key_not_exists");
        return;
    }

    if (isSubscribed(key) == 1) {
        sprintf(result, "%s", "> SUB:key_already_subscribed");
        return;
    }

    for (int i = 0; i < SUBSCRIPTION_SIZE; i++) {
        if (subscription[i].processId == 0) {
            subscription[i].processId = getpid();
            sprintf(subscription[i].key, "%s", key);
            sprintf(result, "> SUB:%s:%s", key, value);
            return;
        }
    }

    sprintf(result, "%s", "> SUB:failed");
}

void unsub(char *key, char *result) {
    char value[MAX_ARGUMENT_LENGTH];
    if (getValueByKey(key, value) == 0) {
        sprintf(result, "%s", "> UNSUB:key_not_exists");
        return;
    }

    if (isSubscribed(key) == 0) {
        sprintf(result, "%s", "> UNSUB:SUB_first");
        return;
    }

    for (int i = 0; i < SUBSCRIPTION_SIZE; i++) {
        if (subscription[i].processId == getpid() && strcmp(subscription[i].key, key) == 0) {
            subscription[i].processId = 0;
            sprintf(subscription[i].key, "%s", "");
            sprintf(result, "> UNSUB:%s:%s", key, value);
            return;
        }
    }

    sprintf(result, "%s", "> UNSUB:failed");
}

void op(Command command, char *result) {
    // Get value by key
    char value[MAX_ARGUMENT_LENGTH];
    if (getValueByKey(command.key, value) == 0) {
        sprintf(result, "%s", "> OP:key_not_exists");
        return;
    }

    char *args[MAX_PARAMETER_LENGTH];
    char valueCopy[MAX_ARGUMENT_LENGTH];
    valueCopy[0] = '\0';
    sprintf(valueCopy, "%s", value);
    char *token;
    char delimiter[] = " ";
    char *rest = valueCopy;

    // parse arguments
    args[0] = command.value;
    int counter = 1;
    while ((token = strtok_r(rest, delimiter, &rest)) != NULL) {
        args[counter] = token;
        counter++;
    }
    args[counter] = NULL;

    int pipe_connect[2];
    pipe(pipe_connect);
    if (fork() == 0) {
        dup2(pipe_connect[1], 1); // Connect standard output with pipes write interface
        close(pipe_connect[0]);
        execvp(command.value, args);
        puts("Invalid system call");
        exit(1);
    }
    else if (fork() == 0) {
        dup2(pipe_connect[0], 0); // Connect standard input with pipes read interface
        close(pipe_connect[1]);

        char result[PAYLOAD_LENGTH];
        int nbytes = read(pipe_connect[0], result, sizeof(result));

        if (nbytes > MAX_ARGUMENT_LENGTH)
            result[MAX_ARGUMENT_LENGTH - 1] = '\0';
        else
            result[nbytes - 1] = '\0';

        Command putCommand = {"", "", ""};
        sprintf(putCommand.type, "PUT");
        sprintf(putCommand.key, "%s", command.key);
        sprintf(putCommand.value, "%s", result);

        handlePut(putCommand, result);
        exit(0);
    }
    else {
        close(pipe_connect[0]);
        close(pipe_connect[1]);
    }

    sprintf(result, "> sending %s to system call %s", value, command.value);
}

void notifySubscribers(char *key, char *content) {
    Message message;
    for (int i = 0; i < SUBSCRIPTION_SIZE; i++) {
        if (strcmp(subscription[i].key, key) == 0 && subscription[i].processId != getpid()) {
            message.header = subscription[i].processId;
            sprintf(message.payload, "%s", content);
            msgsnd(messageQueue, &message, PAYLOAD_LENGTH, 0);
        }
    }
}

int hasWildCard(char *string) {
    for (int i = 0; i < strlen(string); ++i) {
        if (string[i] == '*' || string[i] == '?')
            return 1;
    }

    return 0;
}

int matchWildCard(char *wildCardKey, char *key) {
    if (*wildCardKey == '\0' && *key == '\0')
        return 1;

    if (*wildCardKey == '*' && *(wildCardKey + 1) != '\0' && *key == '\0')
        return 0;

    if (*wildCardKey == '?' || *wildCardKey == *key)
        return matchWildCard(wildCardKey + 1, key + 1);

    if (*wildCardKey == '*')
        return matchWildCard(wildCardKey + 1, key) || matchWildCard(wildCardKey, key + 1);

    return 0;
}
