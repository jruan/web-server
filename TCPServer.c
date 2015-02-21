#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/stat.h>

#include "html_parser.h"
#include "utils.h"

static const int MAXPENDING = 30;
static const int MAX_LINE = 256;

int main(int argc, char *argv[]){

	// Struct for Server
        struct sockaddr_in sin;
	char buf[MAX_LINE];
	int len;
	printf("number of argunments:%d\n", argc);	
        if (argc != 3){
                perror("Parameters(s) needed:  <Port, Root>");
		exit(1);
	}

        /*Create a TCP socket*/
	//in_port_t servPort
        int servPort = atoi(argv[1]); //convert port string to int

        int servSock; //socket descriptor for server
        if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
                perror("socket() failed");
                exit(1);
        }
	printf("server socket created\n");

        /* Bind socket to a port */
       	bzero((char *) &sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(servPort);

	if((bind(servSock, (struct sockaddr *)&sin, sizeof(sin))) <0){
		perror("simplex-talk: bind");
		exit(1);
	} 
	printf("has binded\n");
	/*Set socket to listen*/
	if(listen(servSock, MAXPENDING) < 0){
		perror("listen() failed");
		exit(1);
	}

	int entire_msg_len = 0;

	//Run forever
	for(;;){

		struct sockaddr_in clientAddr;
		socklen_t clientAddrLen = sizeof(clientAddr);
		
		int clientSock = accept(servSock, (struct sockaddr*) &clientAddr, &clientAddrLen);
		if(clientSock < 0){
			perror("accept() failed");
			exit(1);
		}

		printf("accept\n");
		/* if fork is zero, that means a child process to handle the connetion has successfully been created
 		 * always remember, when an error occurs, to call exit(0) so that the child process can exit freeing up memory.
 		 * Then we can close the client connection after 
 		 */
		
		if(fork() == 0){	
			char double_carriage[5] = "\r\n\r\n";
			int number_of_buf = 0;

			char *entire_msg = NULL;
			char *remaining_msg = NULL;
			struct timeval timeout;
			timeout.tv_sec = 10;
			timeout.tv_usec = 0;
			if (setsockopt (clientSock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0){
			     perror("setsockopt failed\n");	
			}

			memset(buf, '\0', MAX_LINE);
			while( (len = recv(clientSock, buf, sizeof(buf) - 1, 0)) || get_length(entire_msg) != 0){
				if(len == -1){
					exit(0);
					break;
				}
				
				entire_msg_len = get_length(entire_msg);
				number_of_buf += 1;

				/*allocate memory for entre msg where it can fit the buf size and the messages from last iteration
 				 * so we can take care of them now
 				 */
				entire_msg = (char*)realloc(entire_msg, (number_of_buf * sizeof(buf)) + entire_msg_len);
				strcat(entire_msg, buf);


				/*try to detect if there is a double carriage return, if so proceed. if not, keep
 				 *keep on concatinating the buf to our entire msg buffer until there is
				 */
				remaining_msg = strstr(entire_msg, double_carriage);
				
				if(remaining_msg == NULL){
					memset(buf, '\0', MAX_LINE);
					continue;
				}

				char *request = substr(entire_msg, get_substr_len(entire_msg, remaining_msg) + 4);
				char* header = parse_html_request(request);
				char* filename = NULL;

				
				if(header != NULL){
					filename = parse_html_method_header(header);
					if(filename != NULL){
						int version = get_version_number(parse_html_request(request));
						FILE* f;
						char* root_path = argv[2];
						char path[strlen(root_path) + strlen(filename)];
						char* file_type = strstr(filename, ".");
						struct stat fileattrib;
						int filemode;
						/* if version zero check if the file exists.
 						 * if it doesnt, return 404 not found
 						 * if it does, send back the contents, exit the process and then close the client
 						 * connection. Not persistence*/
						if(version == 0){
							if(file_type == NULL){
								/* checks that / -> index.html without having to specify a request for index.html*/
								if(handle_special_case(filename) == 1){
									char index_path[strlen(root_path) + strlen(filename) + strlen("index.html")];
									strcpy(index_path, root_path);
									strcat(index_path, filename);
									strcat(index_path, "index.html");
									f = fopen(index_path, "r");
									if(f == NULL){
										handle_404(clientSock, 0);	
										exit(0);
									}
								
									/*checks for permission. If not world readable, send 403 error*/
									if(stat(index_path, &fileattrib) == -1){
										perror("permission error");
										exit(1);
									}
									
									filemode = fileattrib.st_mode;
									if(!(filemode & S_IROTH)){
										handle_403(clientSock, 0);
										exit(0);
									}

									else{
										handle_httpV10(f, ".html", clientSock);
										exit(0);										
									}
									break;
								}
								/*if no file type. mayb a dirctory?? gotta check this later, or mayb it is a 404*/
								else{
									handle_404(clientSock, 0);
									exit(0);
									break;
								}
							}

							strcpy(path, root_path);
							strcat(path, filename);
							/*opens the file, if it is not found, return 404*/
							if(strcmp(file_type, ".html") == 0)
								f = fopen(path, "r");

							else if(strcmp(file_type, ".jpg") == 0)
								f = fopen(path, "rb");

							else if(strcmp(file_type, ".png") == 0)
								f = fopen(path, "rb");			
							
							/*404 error*/
							if(f == NULL){
								printf("in here then\n");
								handle_404(clientSock, 0);
								exit(0);
							}

							/* checks if the permission is world readable. If not, send 403 error*/
							if(stat(path, &fileattrib) == -1){
								perror("permission error");
								exit(1);
							}
									
							filemode = fileattrib.st_mode;

							if(!(filemode & S_IROTH)){
								handle_403(clientSock, 0);
								exit(0);
							}
							/*if not send the contents back*/
							else{
								handle_httpV10(f, file_type, clientSock);
								exit(0);
							}
							break;
						}
						else{
							/*fork a process to handle each http 1.1 connection for pipelineing*/
							/* everything else is the same as http 1.0*/
							if(fork() == 0){
								if(file_type == NULL){
									if(handle_special_case(filename) == 1){
										char index_path[strlen(root_path) + strlen(filename) + strlen("index.html")];
										strcpy(index_path, root_path);
										strcat(index_path, filename);
										strcat(index_path, "index.html");
										f = fopen(index_path, "r");
										if(f == NULL){				
											handle_404(clientSock, 1);
											exit(0);
										}
			
										/*checking for the permission. if not world reable, send back error*/
										if(stat(index_path, &fileattrib) == -1){
											perror("permission error");
											exit(1);
										}
									
										filemode = fileattrib.st_mode;
										
										if(!(filemode & S_IROTH)){
											handle_403(clientSock, 1);
											exit(0);
										}

										else{
											handle_httpV11(f, ".html", clientSock);
											exit(0);
										}
										break;
									}
									else{
										handle_404(clientSock, 1);
										exit(0);
									}
								}
								strcpy(path, root_path);
								strcat(path, filename);
							
								if(strcmp(file_type, ".html") == 0)
									f= fopen(path, "r");		
			
								else if(strcmp(file_type, ".jpg") == 0)
									f = fopen(path, "rb");

								else if(strcmp(file_type, ".png") == 0)
									f = fopen(path, "rb");

								if(f == NULL){
									handle_404(clientSock, 1);
									exit(0);
								}
							
								/*send back error if not world readable*/	
								if(stat(path, &fileattrib) == -1){
									perror("permission error");
									exit(1);
								}
									
								filemode = fileattrib.st_mode;

								if(!(filemode & S_IROTH)){
									handle_403(clientSock, 1);
									exit(0);
								}
				
								else{	
									handle_httpV11(f, file_type, clientSock);
									exit(0);
								}
							}//end of if fork
						}//end of version number 1.1 else
					}//end of if file == null
					else{
						handle_400(clientSock);	
						exit(0);
						break;
					}
				}
				entire_msg = NULL;
				if((remaining_msg + 4) != '\0'){
					entire_msg = (char*)realloc(entire_msg, get_length(remaining_msg) - 4);
					strcpy(entire_msg, remaining_msg + 4);
				}
				memset(buf, '\0', sizeof(buf));
				number_of_buf = 0;
//				break;
			} //End of while receive loop
		} //end of if for statement
		close(clientSock);
	}//end of forever loop
	
	close(servSock);
        return 0;
}

