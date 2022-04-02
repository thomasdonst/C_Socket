#include <stdio.h>
#include <string.h>
#include "include/keyValueStore.h"
#include "include/configuration.h"
#include "include/subroutines.h"

const int storageSize = 1000;
Entry storage[1000];
int storageItemCount = 0;

void get(char *key, char *result) {
    if (isAlphanumeric(key) == 0) {
        sprintf(result, "Not an alphanumeric value");
        return;
    }

    for (int i = 0; i < storageSize; i++) {
        if (strcmp(storage[i].key, key) == 0) {
            sprintf(result, "GET:%s:%s", key, storage[i].value);
            return;
        }
    }
    sprintf(result, "GET:%s:key_nonexistent", key);
}

void put(char *key, char *value, char *result) {
    if (isAlphanumeric(key) == 0 || isAlphanumeric(value) == 0) {
        sprintf(result, "Not an alphanumeric value");
        return;
    }

    // Search entry with this key -> Replace value
    for (int i = 0; i < storageSize; i++) {
        if (strcmp(storage[i].key, key) == 0) {
            strcpy(storage[i].key, key);
            strcpy(storage[i].value, value);
            sprintf(result, "PUT:%s:%s", key, storage[i].value);
            return;
        }
    }

    // No entry with this key was found -> Create entry at next free position
    for (int i = 0; i < storageSize; i++) {
        if (strcmp(storage[i].key, "\0") == 0) {
            strcpy(storage[i].key, key);
            strcpy(storage[i].value, value);
            storageItemCount++;
            sprintf(result, "PUT:%s:%s", key, storage[i].value);
            return;
        }
    }
    sprintf(result, "No entry could be created");
}

void del(char *key, char *result) {
    if (isAlphanumeric(key) == 0) {
        sprintf(result, "Not an alphanumeric value");
        return;
    }

    for (int i = 0; i < storageSize; i++) {
        if (strcmp(storage[i].key, key) == 0) {
            strcpy(storage[i].key, "");
            strcpy(storage[i].value, "");
            storageItemCount--;
            sprintf(result, "DEL:%s:key_deleted", key);
            return;
        }
    }
    sprintf(result, "DEL:%s:key_nonexistent", key);
}

void show(char *result) {
    char tmp[MAX_ENTRY_SIZE];
    char message[MAX_ENTRY_SIZE];

    tmp[0] = '\0';
    message[0] = '\0';

    for (int i = 0; i < storageSize; i++) {
        if (strcmp(storage[i].key, "") != 0) {
            sprintf(tmp, "Index: %d   Key: %s  Value: %s\r\n", i, storage[i].key, storage[i].value);
            strcat(message, tmp);
        }
    }
    sprintf(tmp, "Number of existing entries: %d", storageItemCount);
    strcat(message, tmp);

    sprintf(result, "%s", message);
}
