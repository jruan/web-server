#ifndef utils_H
#define utils_H
#endif



void handle_httpV10(FILE* f, char* path, int clientSock);
void handle_httpV11(FILE* f, char* path, int clientSock);
void handle_404(int clientSock, int version);
void handle_400(int clientSock);
void handle_403(int clientSock, int version);
