#ifndef C_SOCKET_KEYVALUESTORE_H
#define C_SOCKET_KEYVALUESTORE_H

#define MAX_ARGUMENT_LENGTH 16
#define ADDITIONAL_SPACE 30

typedef struct Entry_{
    char key[MAX_ARGUMENT_LENGTH];
    char value[MAX_ARGUMENT_LENGTH];
} Entry;

void initializeSharedMemory();
void attachClientToSharedMemory();
void closeSharedMemory();

void get(char *key, char *result);
void put(char *key, char *value, char *result);
void del(char *key, char *result);
void show(char *result);

#endif //C_SOCKET_KEYVALUESTORE_H