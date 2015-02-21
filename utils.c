#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

void handle_httpV10(FILE* f, char* file_type, int clientSock){
	send(clientSock, "HTTP/1.0 200 Ok \r\n", 17, 0);
	char* line = NULL;
	ssize_t read;
	size_t len = 0;
	
/*	fseek(f, 0, SEEK_END);
	len = ftell(f);
	fseek(f, 0, SEEK_SET);
	char length[length_of_file];
	sprintf(length, "%d", length_of_file);
	*/
	if(strcmp(file_type, ".html") == 0)
		send(clientSock, "Content-Type: text/html\r\n\r\n", 27, 0);
	

	else if(strcmp(file_type, ".jpg") == 0)
		send(clientSock, "Content-Type: image/jpeg\r\n\r\n", 28, 0);

	else{
		if(strcmp(file_type, ".png") == 0)
			send(clientSock, "Content-Type: image/png\r\n\r\n", 27, 0);
	}
	
	while((read = getline(&line, &len, f)) != -1){
		send(clientSock, line, read, 0);
	}
}

void handle_httpV11(FILE* f, char* file_type, int clientSock){
	send(clientSock, "HTTP/1.1 200 Ok \r\n", 17, 0);
        char* line = NULL;
        ssize_t read;
        size_t len = 0;

        if(strcmp(file_type, ".html") == 0)
                send(clientSock, "Content-Type: text/html\r\n\r\n", 27, 0);


        else if(strcmp(file_type, ".jpg") == 0)
                send(clientSock, "Content-Type: image/jpeg\r\n\r\n", 28, 0);

        else{
                if(strcmp(file_type, ".png") == 0)
                        send(clientSock, "Content-Type: image/png\r\n\r\n", 27, 0);
        }
        while((read = getline(&line, &len, f)) != -1){
                send(clientSock, line, read, 0);
        }
}

void handle_404(int clientSock, int version){
	if(version == 0)
		send(clientSock, "HTTP/1.0 404 Not Found\r\n", 24, 0);

	else
		send(clientSock, "HTTP/1.1 404 Not Found\r\n", 24, 0);
	
	send(clientSock, "Content-Type: text/html\r\n\r\n", 27, 0);
        send(clientSock, "404 Bad Request\n", 16, 0);
}

void handle_400(int clientSock){
	send(clientSock, "HTTP/1.1 400 Bad Request\r\n", 26, 0);
	send(clientSock, "Content-Type: text/html\r\n\r\n", 27, 0);
	send(clientSock, "400 Bad Request\n", 16, 0);
}

void handle_403(int clientSock, int version){
	if(version == 0){
		send(clientSock, "HTTP/1.1 403 Forbidden\r\n", 24, 0);
		send(clientSock, "Content-Type: text/html\r\n\r\n", 27, 0);
		send(clientSock, "Permission Denied\n", 18, 0);
	}

	else{
		send(clientSock, "HTTP/1.1 403 Forbidden\r\n", 24, 0);
		send(clientSock, "Content-Type: text/html\r\n\r\n", 27, 0);
		send(clientSock, "Permission Denied\n", 18, 0);
	}
}
