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
	printf("closed connection\n");
}

int main() {

	// create a socket
	int sfd;
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sfd < 0) {
		fatal_error("could not create socket");
	}

	// address for socket
	char *landing_address = "127.0.0.1"; // = "192.168.1.2";
	
	// port for socket
	int landing_port; // = 6789;
	printf("INPUT port to bind socket to: ");
	scanf("%d", &landing_port);
	getchar();

	// socket address
	struct sockaddr_in socket_address;
	socket_address.sin_family = AF_INET;
	socket_address.sin_port = htons(landing_port);
	socket_address.sin_addr.s_addr = inet_addr(landing_address); // = INADDR_ANY; 

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
		printf("\nlistening for a connection...\n");
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
		char *request_header = strtok(reading_buffer, "\n");
		printf("%s\n", request_header);
		char *request_method = strtok(request_header, " ");
		char *requested_page = strtok(NULL, " ");
		char *request_protocol = strtok(NULL, " ");
		strtok(request_protocol, "/");
		char *request_protocol_version = strtok(NULL, "\n");

		// checks
		
		// method checks
		if(strcmp(request_method, "GET") != 0) {
			tidy_up(csfd);
			continue;
		}

		// file path checks		
		if(requested_page == NULL) {
			tidy_up(csfd);
			continue;
		}

		char *requested_file_path;
		char test_file_path[32] = "./site";
		strcat(test_file_path, requested_page);

		FILE *test_file;
		test_file = fopen(test_file_path, "r");
		
		char *status = "200 OK";

		if(test_file == NULL) {
			requested_file_path = "/404.html";
			status = "404 Not Found";
		}
		else if(strcmp(requested_page, "/") == 0) {
			requested_file_path = "/index.html";
			fclose(test_file);
		}
		else {
			requested_file_path = requested_page;
			fclose(test_file);
		}

		// mime type checks
		char requested_file_path_copy[strlen(requested_file_path)];
		strcpy(requested_file_path_copy, requested_file_path);
		strtok(requested_file_path_copy, ".");
		char *file_extension = strtok(NULL, "\n");
		char *mime_type;
		if(strcmp(file_extension, "html") == 0) {
			mime_type = "text/";
		}
		else if(strcmp(file_extension, "css") == 0) {
			mime_type = "text/";
		}
		else {
			mime_type = "text/";
			file_extension = "plain";
		}
	
		// init the body
		char response_buffer[4096];
		memset(&response_buffer, 0, sizeof(response_buffer));
		char response_buffer_size[20];
		memset(&response_buffer_size, 0, sizeof(response_buffer_size));

		char file_path[32] = "./site";
		strcat(file_path, requested_file_path);

		FILE *file_data;
		file_data = fopen(file_path, "r");
		fseek(file_data, 0, SEEK_END);
		unsigned long file_size = ftell(file_data);
		sprintf(response_buffer_size, "%lu", file_size);
		rewind(file_data);
		fread(response_buffer, 1, file_size, file_data);
		fclose(file_data);

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
		printf("served %s\n", file_path);
	
		// tidy up
		tidy_up(csfd);
	}

	// exit program
	close(sfd);
	printf("\nclosed socket\n");

	return 0;
}
