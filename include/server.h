#ifndef C_SOCKET_SERVER_H
#define C_SOCKET_SERVER_H



extern struct sockaddr_in serverAddress;
extern int serverSocket;
extern int clientAddressLength;

extern struct sockaddr_in clientAddress;
extern int clientSocket;

extern int currentClientNumber;

void initializeServerSocket();
void closeServerSocket();
void handleClientConnection();
void acceptClientConnection();
void sendMessageToClient(char *message);
void greetClient();
void handleSubscriberNotifications();
int receiveMessage(char *message);
void showDisconnectionStatus(int status);
int hasClientQuit(char *response, int disconnectionStatus);
void showMessage(char *message);
void showClientMessage(char *message);
void showErrorMessage(char *message);
void closeServerSocket();
void closeClientSocket();
void cleanUp(int forkedProcessId);


#endif //C_SOCKET_SERVER_H