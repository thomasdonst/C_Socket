#ifndef C_SOCKET_KEYVALUESTORE_H
#define C_SOCKET_KEYVALUESTORE_H

#define MAX_LENGTH 128

typedef struct Entry_{
    char key[MAX_LENGTH];
    char value[MAX_LENGTH];
} Entry;

void get(char *key, char *result);
void put(char *key, char *value, char *result);
void del(char *key, char *result);
void show(char *result);

#endif //C_SOCKET_KEYVALUESTORE_H
