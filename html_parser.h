#ifndef html_PARSER_H
#define html_PARSER_H
#endif

#include <stdio.h>

char* substr(char* buffer, int length);
int get_substr_len(char* buffer, char* end_of_substr);
char* parse_html_request(char* request);
char* parse_html_method_header(char* header);
int get_version_number(char* header);
int get_length(char* buff);
int handle_special_case(char* filename);

