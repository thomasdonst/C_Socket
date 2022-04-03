#include "include/server.h"
#include "include/configuration.h"

#include "stdio.h"
#include "netinet/in.h"
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

struct sockaddr_in serverAddress;
int serverSocket;
int clientAddressLength;

struct sockaddr_in clientAddress;
int clientSocket;

void initializeServerSocket() {
    // define type of server socket
    serverAddress.sin_family = AF_INET; // AF_INET = IPv4
    serverAddress.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY = socket will be bound to all interfaces
    serverAddress.sin_port = htons(PORT);

    // create server socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) { // SOCK_STREAM = TCP; SOCK_DGRAM = UDP
        perror("Server socket can not be initialized");
        exit(EXIT_FAILURE);
    }

    // The purpose of this is to allow to reuse the port even if the process crash or been killed
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

    // bind server socket to localhost port 5678
    if (bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress))) {
        perror("Server socket bind failed.");
        exit(EXIT_FAILURE);
    }

    // try to specify maximum of 5 pending connections for server socket
    listen(serverSocket, 5);

    // show notification
    char info[255];
    sprintf(info, "Server started %s:%d\r\nWaiting for clients to connect ...",
            inet_ntoa(serverAddress.sin_addr), ntohs(serverAddress.sin_port));
    puts(info);
}

void acceptClientConnection() {
    clientSocket = accept(serverSocket, &clientAddress, &clientAddressLength);

    if (clientSocket < 0)
        perror("Client could not be accepted");
    else
        puts("Client accepted");
}

void sendMessageToClient(char *message) {
    char messageWithNewLine[strlen(message)];
    sprintf(messageWithNewLine, "%s\r\n", message);
    send(clientSocket, messageWithNewLine, strlen(messageWithNewLine), 0);
}

void sendInputInformation() {
    char message[] = "GET [key]\r\nPUT [key] [value]\r\nDEL [key]\r\nSHOW\r\nQUIT\r\n";
    sendMessageToClient(message);
}

int receiveMessage(char *message) {
    int i = 0;
    int disconnectionStatus;
    char input[MAX_ENTRY_SIZE];
    input[0] = '\0';
    message[0] = '\0';

    while ((disconnectionStatus = recv(clientSocket, input, sizeof(input), 0)) > 0) {
        // Prevent buffer overflow
        if (i > MAX_ENTRY_SIZE - 1) {
            sprintf(message, "%s", "\r\n> Too many characters");
            sendMessageToClient(message);
            sprintf(message, "%s", "> Too many characters");
            return disconnectionStatus;
        }

        if (input[0] == '\r' || input[0] == '\n')
            return disconnectionStatus;

        input[1] = '\0';
        // Only symbols, letters and numbers get registered as an input
        if (input[0] > 31 && input[0] < 127) {
            strcat(message, input);
            i++;
        }
    }
}

void showDisconnectionStatus(int status) {
    switch (status) {
        case 0:
            puts("Client disconnected");
            break;
        case 1:
            perror("Receive failed");
            break;
        case 2:
            perror("Session closed");
            break;
        default:
            perror("Connection lost");
    }
}

void showMessage(char *message) {
    puts(message);
}

void closeServerSocket() {
    close(serverSocket);
}
