#ifndef C_SOCKET_KEYVALUESTORE_H
#define C_SOCKET_KEYVALUESTORE_H

#include <sys/ipc.h>

#define MAX_ARGUMENT_LENGTH 16
#define ADDITIONAL_SPACE 30
#define PAYLOAD_LENGTH 256

extern int *exclusiveAccess;
extern int exclusiveAccessMemoryId;

extern key_t messageQueue;

typedef struct Entry_{
    char key[MAX_ARGUMENT_LENGTH];
    char value[MAX_ARGUMENT_LENGTH];
} Entry;

typedef struct Subscription_{
    int processId;
    char key[MAX_ARGUMENT_LENGTH];
} Subscription;

typedef struct Message_{
    long header;
    char payload[PAYLOAD_LENGTH];
} Message;

void initializeSharedMemories();
void attachClientToSharedMemories();
void closeSharedMemories();
void resolveExclusiveAccess();
void initializeMessageQueue();
void closeMessageQueue();

void get(char *key, char *result);
int put(char *key, char *value, char *result);
int del(char *key, char *result);
void show(char *result);
void beg(char *result);
void end(char *result);
void sub(char *key, char *result);
void unsub(char *key, char *result);

void notifySubscribers(char *key, char *content);

#endif //C_SOCKET_KEYVALUESTORE_H