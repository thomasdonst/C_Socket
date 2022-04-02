#ifndef C_SOCKET_MAIN_H
#define C_SOCKET_MAIN_H

void initializeServerSocket();
void closeServerSocket();
void acceptClientConnection();
void sendMessageToClient(char *message);
void sendInputInformation();
int receiveMessage(char *message);
void showDisconnectionStatus(int status);
void showMessage(char *message);

#endif //C_SOCKET_MAIN_H
