#ifndef C_SOCKET_SUBROUTINES_H
#define C_SOCKET_SUBROUTINES_H

#define MAX_ARGUMENT_LENGTH 24
#define MAX_PARAMETER_LENGTH 100
#define COUNT_OF_COMMAND_ARGUMENTS 3
#define MESSAGE_BUFFER 128

typedef struct Command_{
    char type[MAX_ARGUMENT_LENGTH];
    char key[MAX_ARGUMENT_LENGTH];
    char value[MAX_ARGUMENT_LENGTH];
} Command;


Command parseTelnetCommand(char *message);
Command parseHttpCommand(char *message);
void processCommand(Command command, char *result);
int containsOnlySpaceCharacters(char *string);
void toLower(char *string);
int containsSlash(char *string);
char *trim(char *string);
int hasAccess();
int isValidCommand(Command is, Command should);
void substring(char s[], char sub[], int p, int l);

void handleGet(Command command, char *result);
void handlePut(Command command, char *result);
void handleDel(Command command, char *result);
void handleShow(Command command, char *result);
void handleBeg(char *result);
void handleEnd(char *result);
void handleSub(Command command, char *result);
void handleUnsub(Command command, char *result);
void handleOp(Command command, char *result);

#endif //C_SOCKET_SUBROUTINES_H