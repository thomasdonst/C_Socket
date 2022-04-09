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
                (COUNT_OF_COMMAND_ARGUMENTS *
                 MAX_ARGUMENT_LENGTH + ADDITIONAL_SPACE)];

    signal(SIGINT, cleanUp);

    initializeServerSocket();
    initializeSharedMemories();
    initializeMessageQueue();

    handleClientConnection();
    attachClientToSharedMemories();
    greetClient();
    handleSubscriberNotifications();

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