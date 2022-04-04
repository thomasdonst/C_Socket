#include "include/server.h"
#include "include/subroutines.h"
#include "include/keyValueStore.h"
#include "include/configuration.h"

#include <string.h>
#include <signal.h>

int main() {
    signal(SIGINT, closeServerSocket);

    initializeServerSocket();
    initializeSharedMemory();
    handleClientConnection();
    sendInputInformation();

    int disconnectionStatus;
    Command command;
    char message[MAX_ENTRY_SIZE];
    char result[MAX_ENTRY_SIZE];
    while (1) {
        disconnectionStatus = receiveMessage(message);
        showMessage(message);

        command = fetchCommand(message);
        processCommand(command, result);
        showMessage(result);
        sendMessageToClient(result);

        if (strcmp((char *) command.type, "quit") == 0 || disconnectionStatus < 1)
            break;
    }

    showDisconnectionStatus(disconnectionStatus);
    closeServerSocket();
}








