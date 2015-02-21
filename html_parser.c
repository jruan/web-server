#include "html_parser.h"
#include <stdio.h>
#include <assert.h>

#include <string.h>
#include <stdlib.h>

/* return a substring of the buffer of the length input*/
char* substr(char* buffer, int length){
	if(buffer == NULL){
		return NULL;
	}
	char* buf = (char*)malloc(length + 1);
	memset(buf, '\0', length + 1);
	memcpy(buf, buffer, length);
	return buf;
}

/*get the length of substrings*/
int get_substr_len(char *buffer, char* end_of_substr){
	int count = 0;
	while(end_of_substr != buffer){
		count++;
		end_of_substr--;
	}
	return count;
}

/*get length of a sring*/
int get_length(char* buf){
	if(buf == NULL)
		return 0;

	char* tmp = buf;
	int len = 0;

	while(*tmp != '\0'){
		len++;		
		tmp++;
	}
	
	return len;
}

/*parse the whole html request and just return the request method header*/
char* parse_html_request(char* request){
	if(request == NULL)
		return NULL;

	char* header = NULL;		
	char* new_line= "\n";
	header = substr(request, get_substr_len(request, strstr(request, new_line) - 1));
	return header;
}

/*parse the header and if all fields of a header are thre, return the file name, else null for error*/
char* parse_html_method_header(char* header){
	if(header == NULL)
		return NULL;

	char space[2] = " ";
	char* msg_header_array[3];

	char* http1 = "HTTP/1.1";
	char* http0 = "HTTP/1.0";
	char* get = "GET";
	char* msg_header_token;
	
	int index = 0;
        msg_header_token = strtok(header, space);
	if(msg_header_token == NULL){
        	return NULL;
	}

	while(msg_header_token != NULL){
        	msg_header_array[index] = strdup(msg_header_token);
                msg_header_token = strtok(NULL, space);
		index++;
        }
	if(index != 3)
		return NULL;
/*
	if(msg_header_array[0] == NULL || msg_header_array[1] == NULL || msg_header_array[2] == NULL){
		printf("in here\n");
		return NULL;
	}*/

	if(strcmp(msg_header_array[0], get) != 0)
		return NULL;

	if(strcmp(msg_header_array[2], http0) != 0 && strcmp(msg_header_array[2], http1) != 0){
		printf("in http\n");
		return NULL;
	}

	return msg_header_array[1];
}

/*gets the protocol version number
 * 0->1.0
 * 10>1.1
 */
int get_version_number(char* header){
	if(header == NULL)
		return -1;
	
	char space[2] = " ";
	char* msg_header_array[3];
	char* http1 = "HTTP/1.1";
	char* http0 = "HTTP/1.0";

	char* msg_header_token;
	
	int index = 0;
        msg_header_token = strtok(header, space);

	if(msg_header_token == NULL)
        	return -1;

	while(msg_header_token != NULL){
        	msg_header_array[index] = strdup(msg_header_token);
                index++;
                msg_header_token = strtok(NULL, space);
        }

	if(strcmp(msg_header_array[2],http0) == 0)
		return 0;
	
	if(strcmp(msg_header_array[2], http1) == 0)
		return 1;

	return -1;
}

/*this method handles the special case such that we request for some subdirectories 
 *and end with a slash. If this is the case, then we must return the default of it which is 
 *index if it exists. This method returns 1 if it ends wit ha slash, -1 if not
 */

int handle_special_case(char* filename){
	char* tmp = filename;
	while(*tmp != '\0'){
		tmp++;
	}
	tmp = tmp - 1;
	if(*tmp == '/'){
		return 1;
	}
	return -1;
}
