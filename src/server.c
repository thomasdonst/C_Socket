#include "stdio.h"
#include "include/server.h"
#include "include/configuration.h"

#include "netinet/in.h"
#include<sys/socket.h>
#include<unistd.h>

struct sockaddr_in serverAddress;
int serverSocket;
int clientAddressLength;

struct sockaddr_in clientAddress;
int clientSocket;

int runServer() {
    char message[MAX_ENTRY_SIZE];
    int disconnectionStatus;

    initializeServerSocket();
    acceptClientConnection();
    sendInputInformation();

    while ((disconnectionStatus = receiveCommand(message)) > 0) {
        puts(message);
        sendInputInformation();
    }

    showDisconnectionStatus(disconnectionStatus);
    closeServerSocket();
}

void initializeServerSocket() {
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

    int backlog = 5;
    listen(serverSocket, backlog);

    puts("Server started.\nWaiting for clients to connect ...");
}

void acceptClientConnection() {
    clientSocket = accept(serverSocket, &clientAddress, &clientAddressLength);

    if (clientSocket < 0)
        perror("Accept failed");
    else
        puts("Connection accepted.");
}

void sendInputInformation(char * str) {
    write(clientSocket, "GET [key]\nPUT [key] [value]\nDEL [key]\nSHOW\nQUIT\nBEG\nEND\n", MAX_ENTRY_SIZE);
}

int receiveCommand(char *command) {
    return recv(clientSocket, command, MAX_ENTRY_SIZE, 0);
}

void showDisconnectionStatus(int status) {
    switch (status) {
        case 0:
            puts("client disconnected");
            break;
        case 1:
            perror("Receive failed");
            break;
        default:
            perror("Connection lost");
    }
}

void closeServerSocket() {
    close(serverSocket);
}







