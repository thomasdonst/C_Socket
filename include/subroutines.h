#ifndef C_SOCKET_SUBROUTINES_H
#define C_SOCKET_SUBROUTINES_H

typedef struct Command_{
    char type[128];
    char key[128];
    char value[128];
} Command;

Command fetchCommand(char *message);
void executeCommand(Command command, char *result);
void toLower(char *string);
int isAlphanumeric(char *string);

#endif //C_SOCKET_SUBROUTINES_H
