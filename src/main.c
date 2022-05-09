#include "include/server.h"
#include "include/keyValueStore.h"

int main() {
    initializeSignals();
    initializeServerSockets();
    initializeSharedMemories();
    initializeMessageQueue();

    loadKeyValueStore();
    handleClientConnection();

    return 0;
}