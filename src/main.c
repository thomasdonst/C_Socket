#include "include/server.h"
#include "include/subroutines.h"
#include "include/keyValueStore.h"
#include "include/configuration.h"

#include <signal.h>

int main() {
    const int RESULT_BUFFER = KEY_VALUE_STORE_SIZE *
                              (COUNT_OF_COMMAND_ARGUMENTS *
                                MAX_ARGUMENT_LENGTH + ADDITIONAL_SPACE);
    int disconnectionStatus;
    Command command;
    char message[MESSAGE_BUFFER];
    char result[RESULT_BUFFER];

    signal(SIGINT, cleanUp);
    signal(SIGCHLD, SIG_IGN);

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

        if (hasClientQuit(command.type, disconnectionStatus) == 1) {
            showDisconnectionStatus(disconnectionStatus);
            cleanUp(0);
            return 0;
        }
    }
}