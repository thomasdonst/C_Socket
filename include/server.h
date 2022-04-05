#ifndef C_SOCKET_SERVER_H
#define C_SOCKET_SERVER_H

#define BUFFER_LENGTH 128

extern struct sockaddr_in serverAddress;
extern int serverSocket;
extern int clientAddressLength;

extern struct sockaddr_in clientAddress;
extern int clientSocket;

extern int currentClient;

void initializeServerSocket();
void closeServerSocket();
void handleClientConnection();
void acceptClientConnection();
void sendMessageToClient(char *message);
void greetClient();
int receiveMessage(char *message);
void showDisconnectionStatus(int status);
int hasClientQuit(char *response, int disconnectionStatus);
void showMessage(char *message);
void showClientMessage(char *message);
void showErrorMessage(char *message);
void closeServerSocket();
void closeClientSocket();
void cleanUp();


#endif //C_SOCKET_SERVER_H