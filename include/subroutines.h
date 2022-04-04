#ifndef C_SOCKET_SUBROUTINES_H
#define C_SOCKET_SUBROUTINES_H

#define MAX_ARGUMENT_LENGTH 16

typedef struct Command_{
    char type[MAX_ARGUMENT_LENGTH];
    char key[MAX_ARGUMENT_LENGTH];
    char value[MAX_ARGUMENT_LENGTH];
} Command;


Command fetchCommand(char *message);
void processCommand(Command command, char *result);
void toLower(char *string);
int isAlphanumeric(char *string);

#endif //C_SOCKET_SUBROUTINES_H