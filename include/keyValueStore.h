#ifndef C_SOCKET_KEYVALUESTORE_H
#define C_SOCKET_KEYVALUESTORE_H

typedef struct Entry_{
    char key[128];
    char value[128];
} Entry;


void get(char *key, char *result);
void put(char *key, char *value, char *result);
void del(char *key, char *result);
void show(char *result);
#endif //C_SOCKET_KEYVALUESTORE_H
