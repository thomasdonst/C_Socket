#ifndef C_SOCKET_SERVER_H
#define C_SOCKET_SERVER_H

int runServer();
void initializeServerSocket();
void acceptClientConnection();
void sendInputInformation();
int receiveCommand(char *command);
void showDisconnectionStatus(int status);
void closeServerSocket();

#endif //C_SOCKET_SERVER_H
