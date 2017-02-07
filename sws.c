#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PATH_MAX 4096

int invokeServer(int argc, char * argv[], int * sock){
	
	// Checks input arguments
	if(!(argc == 2)){
		fprintf(stderr,"Command format: ./sws <port> <root directory>");
		return EXIT_FAILURE;
	}
	
	// Creates socket info	
	*sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(*sock < 0){
		fprintf(stderr, "Could not get a socket.");
		return EXIT_FAILURE;
	}

	// Defines server address information
	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof serverAddress);	
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(atoi(argv[1]));

	// Sets the socket option to allow other sockets to bind to this port
	int optVal = 1;
	int sockBind = setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof optVal);
	if(sockBind < 0){
		fprintf(stderr, "Multiple socket bind option failed.");
		return EXIT_FAILURE;	
	}

	if( bind(*sock, (struct sockaddr*)&serverAddress, sizeof serverAddress) == -1){
		fprintf(stderr, "Socket bind failed.");
		close(sock);
		return EXIT_FAILURE;
	}

	fprintf(stdout,"sws is running on UDP port %s and serving %s \npress 'q' to quit ...\n",argv[1],argv[2]);
	return EXIT_SUCCESS;

}

int verifyRequest(char * ){

}

void logRequest(){

}

int main(int argc, char * argv[]){
	
	int sock;// socket info used
	char dir[PATH_MAX];

	if(invokeServer(argc, argv[], &sock) == EXIT_FAILURE){
		printf("Server not successfully initialized.\n");
		exit(EXIT_FAILURE);
	}

	strncpy(dir[], argv[2], PATH_MAX);

}
