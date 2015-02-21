#

CC = gcc
CFLAGS = -g -Wall

default: httpd 

httpd: TCPServer.o html_parser.o utils.o
	$(CC) $(CFLAGS) -o httpd TCPServer.o html_parser.o utils.o

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c 

html_parser.o: html_parser.c html_parser.h
	$(CC) $(CFLAGS) -c html_parser.c

TCPServer.o: TCPServer.c html_parser.h
	$(CC) $(CFLAGS) -c TCPServer.c 


clean:
	$(RM) https *.o *~
