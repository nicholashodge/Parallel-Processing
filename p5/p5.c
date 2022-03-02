//Author: Nicholas Hodge
//Due Date:  4/14/21 7:30
//Project p5

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>

#define BUFFER_LEN		1024

void client(char *server_host, int port, char *args);
void send_msg(int fd, char *buffer, int size);
int recv_msg(int fd, char *buffer);
void error_check(int val, const char *str);
int connect_to_server(char *hostname, int port);

void main(int argc, char *argv[]){

	//"-hw" command line argument
	if(! strncmp(argv[1], "-hw", 3)){

		printf("hello world\n");
		exit(0);

	}

	char buffer[BUFFER_LEN];

	if(argc > 3){

		strcpy(buffer, argv[3]);
		strcat(buffer, "\n");

		//create an arrray of strings from the command line arguments to pass to the server
		for(int i = 4; i < argc; i++){

			strcat(buffer, argv[i]);
			strcat(buffer, "\n");

		}

		//create a null terminator to mark the end of the buffer
		buffer[strlen(buffer) - 1] = '\0';
	}

	client(argv[1], atoi(argv[2]), buffer);
	return;

}

void client(char *server_host, int port, char *args){

	int rc, client_socket;
	char buffer[BUFFER_LEN];
	char *token;

	client_socket = connect_to_server(server_host, port);
	
	token = strtok(args, "\n");
	
	while(token != NULL){
	strcpy(buffer, token);

	send_msg(client_socket, buffer, strlen(buffer) + 1);
	rc = recv_msg(client_socket, buffer);
	printf("%s\n", buffer);
	sleep(2);

	token = strtok(NULL, "\n");
	
	}

	
}

void send_msg(int fd, char *buffer, int size){

	int n;

	n = write(fd, buffer, size);
	error_check(n, "send_msg write");

}

int recv_msg(int fd, char *buffer){

	int bytes_read;

	bytes_read = read(fd, buffer, BUFFER_LEN);
	error_check(bytes_read, "recv_msg read");
	return(bytes_read);

}

void error_check(int val, const char *str){

	if(val < 0){

		printf("%s: %d: %s\n", str, val, strerror(errno));
		exit(1);

	}

}

int connect_to_server(char *hostname, int port){

	int client_socket, rc;
	int optval = 1;
	struct sockaddr_in listener;
	struct hostent *hp;

	hp = gethostbyname(hostname);
	//error checking
	if(hp == NULL){

		printf("connect_to_server: gethostname %s: %s -- exiting\n", hostname, strerror(errno));
		exit(-1);

	}

	bzero((void *)&listener, sizeof(listener));
	bcopy((void *)hp->h_addr, (void *)&listener.sin_addr, hp->h_length);
	listener.sin_family = hp->h_addrtype;
	listener.sin_port = htons(port);

	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	error_check(client_socket, "net_connect_to_server socket");

	rc = connect(client_socket, (struct sockaddr *) &listener, sizeof(listener));
	error_check(rc, "net_connect_to_server connect");

	setsockopt(client_socket, IPPROTO_TCP, TCP_NODELAY, (char *)&optval, sizeof(optval));
	return(client_socket);

}
