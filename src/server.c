#include "include/server.h"
#include "include/keyValueStore.h"
#include "include/configuration.h"
#include "include/subroutines.h"

#include "stdio.h"
#include "netinet/in.h"
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/msg.h>

struct sockaddr_in serverAddress;
int serverSocket;
int clientAddressLength;

struct sockaddr_in clientAddress;
int clientSocket;

int currentClientNumber;

void initializeServerSocket() {
    // define type of server socket
    serverAddress.sin_family = AF_INET; // AF_INET = IPv4
    serverAddress.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY = socket will be bound to all interfaces
    serverAddress.sin_port = htons(PORT);

    // create server socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) { // SOCK_STREAM = TCP
        showErrorMessage("Server socket can not be initialized");
        exit(EXIT_FAILURE);
    }

    // The purpose of this is to allow to reuse the port even if the process crash or been killed
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int)) < 0) {
        showErrorMessage("setsockopt(SO_REUSEADDR) failed");
        exit(EXIT_FAILURE);
    }

    // bind server socket to ip address and port
    if (bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress))) {
        showErrorMessage("Server socket bind failed");
        exit(EXIT_FAILURE);
    }

    // try to specify maximum of X pending connections for server socket
    if (listen(serverSocket, MAX_PENDING_CONNECTIONS) != 0) {
        showErrorMessage("Server socket can not listen");
        exit(EXIT_FAILURE);
    }

    // show notification
    char info[255];
    sprintf(info, "Server started %s:%d\r\nWaiting for clients to connect ...",
            inet_ntoa(serverAddress.sin_addr), ntohs(serverAddress.sin_port));
    showMessage(info);
}


void handleClientConnection() {
    while (1) {
        acceptClientConnection();

        currentClientNumber++;
        showClientMessage("Client accepted");

        int pid = fork();
        if (pid == -1) {
            showErrorMessage("Could not fork process");
            exit(EXIT_FAILURE);
        }
        if (pid == 0)
            break;

        closeClientSocket();
    }
}

void acceptClientConnection() {
    if ((clientSocket = accept(serverSocket, &clientAddress, &clientAddressLength)) < 0) {
        showMessage("Server closed");
        cleanUp(1);
        exit(0);
    }
}

void greetClient() {
    sendMessageToClient("GET [key]\r\nPUT [key] [value]\r\nDEL [key]\r\n"
                        "SUB [key]\r\nUNSUB [key]\r\nSHOW\r\nBEG\r\nEND\r\nQUIT\r\n");
}

void handleSubscriberNotifications() {
    int pid = fork();
    if (pid == -1) {
        showErrorMessage("Could not fork process");
        exit(EXIT_FAILURE);
    }
    if (pid > 0)
        return;

    while (1) {
        Message message;
        if (msgrcv(messageQueue, &message, PAYLOAD_LENGTH, getppid(), 0) < 0) {
            cleanUp(0);
            return;
        }

        sendMessageToClient(message.payload);
    }
}

int receiveMessage(char *message) {
    int i = 0;
    int disconnectionStatus;
    char input[MESSAGE_BUFFER];
    input[0] = '\0';
    message[0] = '\0';

    while ((disconnectionStatus = recv(clientSocket, input, sizeof(input), 0)) > 0) {
        // Prevent buffer overflow
        if (i > MESSAGE_BUFFER - 1) {
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
        case 2:
            showClientMessage("> Client disconnected");
            break;
        case 1:
            showErrorMessage("Receive failed");
            break;
        default:
            showErrorMessage("Connection lost");
    }
}

int hasClientQuit(char *response, int disconnectionStatus) {
    return strcmp(response, "quit") == 0 || disconnectionStatus < 1;
}

void showMessage(char *message) {
    puts(message);
}

void sendMessageToClient(char *message) {
    char messageWithNewLine[KEY_VALUE_STORE_SIZE *
                            (COUNT_OF_COMMAND_ARGUMENTS * MAX_ARGUMENT_LENGTH + ADDITIONAL_SPACE)];
    messageWithNewLine[0] = '\0';
    sprintf(messageWithNewLine, "%s\r\n", message);
    send(clientSocket, messageWithNewLine, strlen(messageWithNewLine), 0);
}

void showClientMessage(char *message) {
    char clientString[KEY_VALUE_STORE_SIZE *
                      (COUNT_OF_COMMAND_ARGUMENTS * MAX_ARGUMENT_LENGTH + ADDITIONAL_SPACE)];
    clientString[0] = '\0';
    sprintf(clientString, "[Client %d] ", currentClientNumber);
    strcat(clientString, message);
    puts(clientString);
}

void showErrorMessage(char *message) {
    perror(message);
}

void closeServerSocket() {
    close(serverSocket);
}

void closeClientSocket() {
    close(clientSocket);
}

void cleanUp(int forkedProcessId) {
    closeClientSocket();
    closeServerSocket();
    resolveExclusiveAccess();
    closeSharedMemories();

    if (forkedProcessId > 0)
        closeMessageQueue(forkedProcessId);
}