#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"
#include "include/subroutines.h"
#include <string.h>
#include "include/configuration.h"

int containsOnlySpaceCharacters(char *string) {
//    int i = 0;
//    while (str[i++] != '\0')
//        if(str[i] != ' ')
//            return 0;
//
//    return 1;
    char letter = string[0];
    int i = 0;

    while(letter != '\0'){
        letter = string[i++];
        if(letter != ' ')
            return 0;
    }

    return 1;
}

void fetchCommand(char *message, char *type, char *arg1, char *arg2) {
    if (strcmp(message, "") == 0)
        return;

    if (containsOnlySpaceCharacters(message) == 1)
        return;

    char *token;
    char delimiter[] = " ";

    strcpy(type, strtok(message, delimiter));
    if ((token = strtok(NULL, delimiter)) != NULL)
        strcpy(arg1, token);
    if ((token = strtok(NULL, delimiter)) != NULL)
        strcpy(arg2, token);
}

