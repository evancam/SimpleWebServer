#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>

#define PATH_MAX 4096 
#define BUFFER_SIZE 1024
#define STATUS_SIZE 128

struct clientRequest{
	struct sockaddr_in address;
	int addressSize;
	char buffer[PATH_MAX];
	char dir[PATH_MAX];
	int serverSocket;
};

int invokeServer(int argc, char * argv[], int * sock){
	
	// Checks input arguments
	if(!(argc == 3)){
		fprintf(stderr,"Command format: ./sws <port> <root directory>\n");
		return EXIT_FAILURE;
	}
	
	// Creates socket info	
	*sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(*sock < 0){
		fprintf(stderr, "Could not get a socket.\n");
		return EXIT_FAILURE;
	}

	// Defines server address information
	struct sockaddr_in serverAddress;
	int port = atoi(argv[1]);
	memset(&serverAddress, 0, sizeof serverAddress);	
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(port);

	// Sets the socket option to allow other sockets to bind to this port
	int optVal = 1;
	int sockBind = setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof optVal);
	if(sockBind < 0){
		fprintf(stderr, "Multiple socket bind option failed.\n");
		return EXIT_FAILURE;	
	}

	if( bind(*sock, (struct sockaddr*)&serverAddress, sizeof serverAddress) == -1){
		fprintf(stderr, "Socket bind failed.\n");
		close(*sock);
		return EXIT_FAILURE;
	}

	fprintf(stdout,"sws is running on UDP port %s and serving %s \npress 'q' to quit ...\n",argv[1],argv[2]);
	return EXIT_SUCCESS;

}

int verifyRequest(char method[], char * path, char appendedPath[], char httpVer[]){
	
	char relative[PATH_MAX] = ".";
	if(strcmp(appendedPath,"/") == 0){
		 strncat(path, "index.html",11);
	}else{
		strncat(path,appendedPath,strlen(appendedPath));
	}

	strcat(relative,path);

	if((access(relative,F_OK) == -1) || strstr(path, "../"))  return 404;
		
	// Returns 200 if method is GET, path does not go up in directories, and HTTP version is correct
	if(strcasestr(method, "GET") && !(strstr(path, "../")) && strcasestr(httpVer, "HTTP/1.0\r\n\r\n")) return 200;	

	return 400;

}



void logRequest(char * status, struct clientRequest *request){
	time_t t = time(NULL);
	struct tm * currentTime = localtime(&t);
	char formattedTime[18], clientMessage[PATH_MAX];
	strftime(formattedTime,17,"%b %d %H:%M:%S",currentTime);
	sscanf(request->buffer,"%s[ ]", clientMessage);
	fprintf(stdout,"%s %s:%d %s; %s\n",formattedTime, inet_ntoa(request->address.sin_addr),ntohs(request->address.sin_port),clientMessage,status);
}



void handleRequest(struct clientRequest *request){

	char * status, * buffer;
	char method[7],path[PATH_MAX],httpVer[PATH_MAX],clientMessage[PATH_MAX];
	sscanf(request->buffer,"%s %s %16c", method, path, httpVer);
	buffer = calloc(BUFFER_SIZE, sizeof(char));
	status = calloc(STATUS_SIZE, sizeof(char));	

	switch(verifyRequest(method, request->dir, path, httpVer)){

	case 200:
		strncat(status,"HTTP/1.0 200 OK",15);
		sprintf(buffer,"%s\r\n\r\n",status);
		
		size_t bytes_read;
		int fSize;
		
		break;

	case 400:
		strncat(status,"HTTP/1.0 400 Bad Request",24);
		sprintf(buffer,"%s\r\n\r\n",status);
		sendto(request->serverSocket, buffer, 28, 0, (struct sockaddr *)&request->address, request->addressSize);
		break;

	case 404:	
		strncat(status,"HTTP/1.0 404 Not Found",22);
		sprintf(buffer,"%s\r\n\r\n",status);
		sendto(request->serverSocket, buffer, 26, 0, (struct sockaddr *)&request->address, request->addressSize);
		break;
	
	default:
		strncat(status,"HTTP/1.0 400 Bad Request",24);
		sprintf(buffer,"%s\r\n\r\n",status);
		sendto(request->serverSocket, buffer, 28, 0, (struct sockaddr *)&request->address, request->addressSize);
		break;

	}
	
	logRequest(status, request);
}

int main(int argc, char * argv[]){
	
	int sock;// socket info used
	char dir[PATH_MAX];
	char buf[BUFFER_SIZE];
	ssize_t requestData;
	//fd_set master;

	if(invokeServer(argc, argv, &sock) == EXIT_FAILURE){
		printf("Server not successfully initialized.\n");
		exit(EXIT_FAILURE);
	}

	strncpy(dir, argv[2], PATH_MAX);

	/* main loop for request handling */
	while(1){
		
		//FD_ZERO(&master);
		//FD_SET(0,&master);
		//FD_SET(sock,&master);

		struct clientRequest *request = calloc(1, sizeof *request);
		request->addressSize = sizeof(request->address);
		strncpy(request->dir, dir, PATH_MAX);
		request->serverSocket = sock;

		requestData = recvfrom(request->serverSocket, &request->buffer, BUFFER_SIZE, 0, (struct sockaddr *) &request->address,(socklen_t *) &request->addressSize);
		
		if(requestData < 0){
			fprintf(stderr,"Could not read from socket.\n");
			exit(-1);
		}
		handleRequest(request);

	}		
}
