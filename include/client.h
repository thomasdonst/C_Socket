#ifndef C_SOCKET_CLIENT_H
#define C_SOCKET_CLIENT_H


int connectToServerSocket();
char* receiveMessage(char *response);
void showMessage(char *message);
void getInput(char *message);
void sendMessage(char *message);
int hasClientSentQuit(char *message);

#endif //C_SOCKET_CLIENT_H

