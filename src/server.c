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
#include <signal.h>

struct sockaddr_in telnetServerAddress, httpServerAddress;
int telnetServerSocket, httpServerSocket;
int clientAddressLength;

struct sockaddr_in clientAddress;
int clientSocket;

int currentClientNumber;

int processId;

void initializeSignals() {
    processId = getpid();

    signal(SIGINT, handleInterrupt);
    signal(SIGCHLD, SIG_IGN);
}

void initializeServerSockets() {
    telnetServerSocket = createServerSocket("telnet", telnetServerAddress, TELNET_PORT);
    httpServerSocket = createServerSocket("http", httpServerAddress, HTTP_PORT);
    showMessage("Waiting for clients to connect ...");
}

void handleClientConnection() {
    int pid = fork();
    if (pid == -1) {
        showErrorMessage("Could not fork process");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        acceptTelnetClientConnection();
        attachClientToSharedMemories();
        serveTelnetClient();
    }
    else {
        acceptHttpClientConnection();
        attachClientToSharedMemories();
        serveHttpClient();
    }
}

void acceptTelnetClientConnection() {
    while (1) {
        if ((clientSocket = accept(telnetServerSocket, &clientAddress, &clientAddressLength)) < 0) {
            showMessage("Telnet port closed");
            exit(0);
        }

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

void acceptHttpClientConnection() {
    while (1) {
        if ((clientSocket = accept(httpServerSocket, &clientAddress, &clientAddressLength)) < 0) {
            showMessage("Http port closed");
            exit(0);
        }

        printf("[Http client] Client connected\n");
        char *responseHeader = "HTTP/1.1 200 OK\n\n";
        send(clientSocket, responseHeader, strlen(responseHeader), 0);

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

void serveTelnetClient() {
    const int RESULT_BUFFER = KEY_VALUE_STORE_SIZE *
                              (COUNT_OF_COMMAND_ARGUMENTS *
                               MAX_ARGUMENT_LENGTH + ADDITIONAL_SPACE);
    int disconnectionStatus;
    Command command;
    char message[MESSAGE_BUFFER];
    char result[RESULT_BUFFER];

    listenSubscriberNotifications();
    greetClient();

    while (1) {
        disconnectionStatus = receiveMessage(message);
        showClientMessage(message);

        command = parseCommand(message);
        processCommand(command, result);

        showClientMessage(result);
        sendMessageToClient(result);

        if (hasClientQuit(command.type, disconnectionStatus) == 1) {
            showDisconnectionStatus(disconnectionStatus);
            handleInterrupt();
            kill(getpid() + 1, SIGKILL);
            return;
        }
    }
}

void serveHttpClient() {
    char message[MESSAGE_BUFFER];
    message[0] = '\0';
    receiveMessage(message);
    sendMessageToClient(message);

    message[0] = '\0';
    receiveMessage(message);
    send(clientSocket, message, strlen(message), 0);
}

void greetClient() {
    sendMessageToClient("GET [key]\r\nPUT [key] [value]\r\nDEL [key]\r\n"
                        "SUB [key]\r\nUNSUB [key]\r\nOP [key] [sys_call]\r\nSHOW\r\nBEG\r\nEND\r\nQUIT\r\n");
}

void listenSubscriberNotifications() {
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
            exit(0);
        }

        puts("test\n");
        sendMessageToClient(message.payload);
    }
}

int receiveMessage(char *message) {
    int currentBytes = 0;
    int receivedBytes;
    char input[MESSAGE_BUFFER];
    input[0] = '\0';
    message[0] = '\0';

    while ((receivedBytes = recv(clientSocket, input, sizeof(input), 0)) > 0) {
        // Prevent buffer overflow
        if (currentBytes > MESSAGE_BUFFER - 1) {
            sprintf(message, "%s", "\r\n> Too many characters");
            sendMessageToClient(message);
            sprintf(message, "%s", "> Too many characters");
            return receivedBytes;
        }

        if (input[0] == '\r' || input[0] == '\n')
            return receivedBytes;

        if (receivedBytes >= 2) {
            sprintf(message, "%s", input);
            message[receivedBytes] = '\0';
            return receivedBytes;
        }
        else {
            input[1] = '\0';
            // Only symbols, letters and numbers get registered as an input
            if (input[0] > 31 && input[0] < 127) {
                strcat(message, input);
                currentBytes++;
            }
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
    sprintf(clientString, "[Telnet client %d] ", currentClientNumber);
    strcat(clientString, message);
    puts(clientString);
}

void showErrorMessage(char *message) {
    perror(message);
}

int createServerSocket(char *protocol, struct sockaddr_in address, int port) {
    int fileDescriptor;

    // define type of server socket
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;

    // create server socket
    if ((fileDescriptor = socket(AF_INET, SOCK_STREAM, 0)) == 0) { // SOCK_STREAM = TCP
        showErrorMessage("Server socket can not be initialized");
        exit(EXIT_FAILURE);
    }

    // The purpose of this is to allow to reuse the port even if the process crash or been killed
    if (setsockopt(fileDescriptor, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int)) < 0) {
        showErrorMessage("setsockopt(SO_REUSEADDR) failed");
        exit(EXIT_FAILURE);
    }

    // bind server filediscriptor to ip address and port
    if (bind(fileDescriptor, (struct sockaddr *) &address, sizeof(address))) {
        showErrorMessage("Server socket bind failed");
        exit(EXIT_FAILURE);
    }

    // try to specify maximum of X pending connections for server filediscriptor
    if (listen(fileDescriptor, MAX_PENDING_CONNECTIONS) != 0) {
        showErrorMessage("Server socket can not listen");
        exit(EXIT_FAILURE);
    }

    // show notification
    char info[255];
    sprintf(info, "Opened %s on %s:%d",
            protocol, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
    showMessage(info);

    return fileDescriptor;
}

void closeServerSockets() {
    close(telnetServerSocket);
    close(httpServerSocket);
}

void closeClientSocket() {
    close(clientSocket);
}

void handleInterrupt() {
    if (processId == getpid()) {
        saveKeyValueStore();
        closeMessageQueue();
    }

    closeClientSocket();
    closeServerSockets();
    resolveExclusiveAccess();
    closeSharedMemories();
}