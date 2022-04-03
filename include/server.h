#ifndef C_SOCKET_SERVER_H
#define C_SOCKET_SERVER_H

extern struct sockaddr_in serverAddress;
extern int serverSocket;
extern int clientAddressLength;

extern struct sockaddr_in clientAddress;
extern int clientSocket;

void initializeServerSocket();
void closeServerSocket();
void acceptClientConnection();
void sendMessageToClient(char *message);
void sendInputInformation();
int receiveMessage(char *message);
void showDisconnectionStatus(int status);
void showMessage(char *message);
void showErrorMessage(char *message);

#endif //C_SOCKET_SERVER_H
