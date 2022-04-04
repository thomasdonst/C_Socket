#include "include/keyValueStore.h"
#include "include/configuration.h"
#include "include/subroutines.h"

#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <stdlib.h>

Entry *storage;
int sharedMemoryId;

void initializeSharedMemory() {
    if ((sharedMemoryId = shmget(IPC_PRIVATE,
                                 sizeof(storage) * KEY_VALUE_STORE_SIZE,
                                 IPC_CREAT | 0777)) == -1) {
        perror("Shared_Memory_Get failed");
        exit(EXIT_FAILURE);
    }
    attachClientToSharedMemory();
}

void attachClientToSharedMemory() {
    storage = (Entry *) shmat(sharedMemoryId, 0, 0);
    if (storage == (Entry *) (-1)) {
        perror("Shared_Memory_Attach failed");
        exit(EXIT_FAILURE);
    }
}

void closeSharedMemory() {
    shmdt(storage);
    shmctl(sharedMemoryId, IPC_RMID, 0);
}

void get(char *key, char *result) {
    if (isAlphanumeric(key) == 0) {
        sprintf(result, "> Not an alphanumeric value");
        return;
    }

    for (int i = 0; i < KEY_VALUE_STORE_SIZE; i++) {
        if (strcmp(storage[i].key, key) == 0) {
            sprintf(result, "> GET:%s:%s", key, storage[i].value);
            return;
        }
    }
    sprintf(result, "> GET:%s:key_nonexistent", key);
}

void put(char *key, char *value, char *result) {
    if (isAlphanumeric(key) == 0 || isAlphanumeric(value) == 0) {
        sprintf(result, "> Not an alphanumeric value");
        return;
    }

    // Search entry with this key -> Replace value
    for (int i = 0; i < KEY_VALUE_STORE_SIZE; i++) {
        if (strcmp(storage[i].key, key) == 0) {
            sprintf(storage[i].key, "%s", key);
            sprintf(storage[i].value, "%s", value);
            sprintf(result, "> PUT:%s:%s", key, storage[i].value);
            return;
        }
    }

    // No entry with this key was found -> Create entry at next free position
    for (int i = 0; i < KEY_VALUE_STORE_SIZE; i++) {
        if (strcmp(storage[i].key, "\0") == 0) {
            sprintf(storage[i].key, "%s", key);
            sprintf(storage[i].value, "%s", value);
            sprintf(result, "> PUT:%s:%s", key, storage[i].value);
            return;
        }
    }
    sprintf(result, "> No entry could be created");
}

void del(char *key, char *result) {
    if (isAlphanumeric(key) == 0) {
        sprintf(result, "> Not an alphanumeric value");
        return;
    }

    for (int i = 0; i < KEY_VALUE_STORE_SIZE; i++) {
        if (strcmp(storage[i].key, key) == 0) {
            sprintf(storage[i].key, "%s", "");
            sprintf(storage[i].value, "%s", "");
            sprintf(result, "> DEL:%s:key_deleted", key);
            return;
        }
    }
    sprintf(result, "> DEL:%s:key_nonexistent", key);
}

void show(char *result) {
    char tmp[KEY_VALUE_STORE_SIZE * (3 * MAX_ARGUMENT_LENGTH + 30)];
    char message[KEY_VALUE_STORE_SIZE * (3 * MAX_ARGUMENT_LENGTH + 30)];

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
    } else
        sprintf(result, "%s", "> Key value store is empty");
}