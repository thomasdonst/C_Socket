#include "include/server.h"
#include "include/subroutines.h"
#include "include/keyValueStore.h"
#include "include/configuration.h"

#include <signal.h>

int main() {
    signal(SIGINT, cleanUp);

    initializeServerSocket();
    initializeSharedMemory();

    // toDo: Remove if/else with acceptClientConnection later (debugging purposes)
    if (MULTIPLE_CLIENT_SESSIONS == 1)
        handleClientConnection();
    else
        acceptClientConnection();

    attachClientToSharedMemory();
    greetClient();

    int disconnectionStatus;
    Command command;
    char message[MAX_STRING_SIZE];
    char result[KEY_VALUE_STORE_SIZE * (3 * MAX_ARGUMENT_LENGTH + 30)];
    while (1) {
        disconnectionStatus = receiveMessage(message);
        showClientMessage(message);

        command = fetchCommand(message);
        processCommand(command, result);

        showClientMessage(result);
        sendMessageToClient(result);

        if (hasClientQuit(command.type, disconnectionStatus) == 1)
            break;
    }

    showDisconnectionStatus(disconnectionStatus);
    cleanUp();
}