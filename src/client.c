#include "stdio.h"
#include "include/client.h"
#include "include/configuration.h"

#include "netinet/in.h"
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

int serverSocket;


int main() {
    int connectionStatus = connectToServerSocket();

    if (connectionStatus == -1){
        printf("No connection could be established.\n");
        return -1;
    }

    char message[MAX_ENTRY_SIZE];
    char response[MAX_ENTRY_SIZE];

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
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    return connect(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
}

char *receiveMessage(char *response) {
    if (recv(serverSocket, response, MAX_ENTRY_SIZE, 0) < 0)
        puts("recv failed");
}

void showMessage(char *message) {
    puts(message);
}

int hasClientSentQuit(char *message) {
    if (strcmp(message, ">Disconnected") == 0)
        return 1;
    else
        return -1;
}

void getInput(char *message) {
    printf("command: ");
    scanf(" %[^\n]", message);
}

void sendMessage(char *message) {
    if (send(serverSocket, message, MAX_ENTRY_SIZE, 0) < 0)
        puts("Send failed");
}



