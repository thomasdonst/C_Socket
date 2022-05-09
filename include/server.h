#ifndef C_SOCKET_SERVER_H
#define C_SOCKET_SERVER_H



extern struct sockaddr_in telnetServerAddress, httpServerAddress;
extern int telnetServerSocket, httpServerSocket;
extern int clientAddressLength;

extern struct sockaddr_in clientAddress;
extern int clientSocket;

extern int currentClientNumber;

void initializeSignals();
void initializeServerSockets();
void closeServerSockets();
void handleClientConnection();
void acceptTelnetClientConnection();
void acceptHttpClientConnection();
void serveTelnetClient();
void serveHttpClient();
void sendMessageToClient(char *message);
void greetClient();
void listenSubscriberNotifications();
int receiveMessage(char *message);
void showDisconnectionStatus(int status);
int hasClientQuit(char *response, int disconnectionStatus);
void showMessage(char *message);
void showClientMessage(char *message);
void showErrorMessage(char *message);
int createServerSocket(char *protocol, struct sockaddr_in address, int port);
void closeServerSockets();
void closeClientSocket();
void handleInterrupt();


#endif //C_SOCKET_SERVER_H