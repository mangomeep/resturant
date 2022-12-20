#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void fatal_error(char *msg) {
	perror(msg);
	exit(1);
}

void error(char *msg) {
	perror(msg);
}

void tidy_up(int client) {
	close(client);
	printf("\nclosed client socket\n");
}

int main() {

	// create a socket
	int sfd;
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sfd < 0) {
		fatal_error("could not create socket");
	}

	// address for socket
	const char *landing_address = "192.168.1.2"; //"INADD_ANY"
	
	// port for socket
	const int landing_port = 6789;

	// socket address
	struct sockaddr_in socket_address;
	socket_address.sin_family = AF_INET;
	socket_address.sin_port = htons(landing_port);
	socket_address.sin_addr.s_addr = inet_addr(landing_address);

	// bind the socket with an address and port
	int binding;
	binding = bind(sfd, (struct sockaddr *)&socket_address, sizeof(socket_address));
	if(binding < 0) {
		fatal_error("could not bind socket");
	}

	// listen to connections
	int listening;
	listening = listen(sfd, 16);
	if(listening < 0) {
		fatal_error("could not listen on socket");
	}

	while(true) {
		printf("\nlistening for connections...\n");
		// accept connection
		int csfd;
		memset(&csfd, 0, sizeof(csfd));
		csfd = accept(sfd, NULL, NULL);
		if(csfd < 0) {
			error("could not accept connection");
			tidy_up(csfd);
			continue;
		}
		printf("\naccepted connection\n");
	
		// recieve, process, and request
		
		// recieve data from client
		char reading_buffer[2048];
		memset(&reading_buffer, 0, sizeof(reading_buffer));
		int reading;
		reading = read(csfd, reading_buffer, (sizeof(reading_buffer) - 1));
		if(reading < 0) {
			error("could not read from socket");
			tidy_up(csfd);
			continue;
		}
	
		// process the request
		char response_buffer[4096];
		memset(&response_buffer, 0, sizeof(response_buffer));

		// checks
		char status[16];
		memset(&status, 0, sizeof(status));
		strcat(status, "200 OK");
		char file_extension[16];
		memset(&file_extension, 0, sizeof(file_extension));
		strcat(file_extension, "plain");
		char mime_type[16];
		memset(&mime_type, 0, sizeof(mime_type));
		strcat(mime_type, "text/");
		
		// init the body

		// make the body
		strcat(response_buffer, "hello there");

		// get details
		char response_buffer_size[20];
		sprintf(response_buffer_size, "%lu", strlen(response_buffer));

		// init the http header
		char writing_buffer[4096];
		memset(&writing_buffer, 0, sizeof(writing_buffer));
		char http_version[16] = "HTTP/1.1 ";
		char http_status[16];
		memset(&http_status, 0, sizeof(http_status));
		strcat(http_status, status);
		char content_type[32];
		memset(&content_type, 0, sizeof(content_type));
		strcat(content_type, mime_type);
		strcat(content_type, file_extension);
		char content_length[20];
		memset(&content_length, 0, sizeof(content_length));
		strcat(content_length, response_buffer_size);

		// make the http header
		strcat(writing_buffer, http_version);
		strcat(writing_buffer, http_status);
		strcat(writing_buffer, "\r\nContent-Type: ");
		strcat(writing_buffer, content_type);
		strcat(writing_buffer, "\r\nContent-Length: ");
		strcat(writing_buffer, content_length);
		strcat(writing_buffer, "\r\n\r\n");
		
		// merge the response
		strcat(writing_buffer, response_buffer);

		// send the response
		int writing;
		writing = write(csfd, &writing_buffer, sizeof(writing_buffer));
		if(writing < 0) {
			error("could not write to socket");
			tidy_up(csfd);
			continue;
		}
	
		// tidy up
		tidy_up(csfd);
	}

	// exit program
	close(sfd);
	printf("\nclosed socket\n");

	return 0;
}
