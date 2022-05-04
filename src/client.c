#include "include/client.h"
#include "include/configuration.h"
#include "include/subroutines.h"
#include "include/keyValueStore.h"

#include "netinet/in.h"
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int serverSocket;
int RESULT_BUFFER;

int main() {
    RESULT_BUFFER = KEY_VALUE_STORE_SIZE *
                    (COUNT_OF_COMMAND_ARGUMENTS *
                     MAX_ARGUMENT_LENGTH + ADDITIONAL_SPACE);
    int connectionStatus = connectToServerSocket();

    if (connectionStatus == -1) {
        printf("No connection could be established.\n");
        return -1;
    }

    char message[RESULT_BUFFER];
    message[0] = '\0';
    char response[RESULT_BUFFER];
    response[0] = '\0';

    while (1) {
        receiveMessage(response);
        showMessage(response);

        if (hasClientSentQuit(response) == 1)
            break;

        getInput(message);
        sendMessage(message);
    }

    close(serverSocket);
    return 0;
}

int connectToServerSocket() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET; // AF_INET = IPv4
    serverAddress.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY = socket will be bound to all interfaces
    serverAddress.sin_port = htons(PORT);

    return connect(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
}

char *receiveMessage(char *response) {
    int receivedBytes;
    if ((receivedBytes = recv(serverSocket, response, RESULT_BUFFER, 0)) < 0)
        puts("recv failed");

    response[receivedBytes - 2] = '\0';
}

void showMessage(char *message) {
    puts(message);
}

int hasClientSentQuit(char *message) {
    if (strcmp(message, "> Connection closed by foreign host") == 0)
        return 1;
    else
        return -1;
}

void getInput(char *message) {
    fgets(message, RESULT_BUFFER, stdin);
}

void sendMessage(char *message) {
    if (send(serverSocket, message, strlen(message), 0) < 0)
        puts("Send failed");
}