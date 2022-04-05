#include "include/server.h"
#include "include/subroutines.h"
#include "include/keyValueStore.h"
#include "include/configuration.h"

#include <signal.h>

int main() {
    int disconnectionStatus;
    Command command;
    char message[BUFFER_LENGTH];
    char result[KEY_VALUE_STORE_SIZE *
                (COUNT_OF_COMMAND_ARGUMENTS * MAX_ARGUMENT_LENGTH + ADDITIONAL_SPACE)];

    signal(SIGINT, cleanUp);

    initializeServerSocket();
    initializeSharedMemory();

    // toDo: Remove if/else with acceptClientConnection later (debugging purposes)
    if (ALLOW_MULTIPLE_CLIENTS == 1)
        handleClientConnection();
    else {
        showMessage("DEBUG: Single Session");
        acceptClientConnection();
    }
    attachClientToSharedMemory();
    greetClient();

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