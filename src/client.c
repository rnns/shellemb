#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <string.h>
#include <netdb.h>

int main(int argc, char *argv[])
{
	int sockfd, portno, auth = 0, stop = 0;
	struct sockaddr_in serv_addr;
	struct hostent *server;
 
	struct timeval timeout;
	fd_set set;
	
	char buffer[1024];

	if (argc < 3) {
		printf("Invalid arguments.\nusage: ./client [HOST_IP_ADDRESS] [PORT]\n");
		exit(0);
	}
	
	// socket openning process
	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) error("error on openning socket");
    
	server = gethostbyname(argv[1]);

	if (server == NULL) {
		printf("Invalid host.\nusage: ./client [HOST_IP_ADDRESS] [PORT]\n");
		exit(0);
	}

	FD_ZERO(&set);
	FD_SET(sockfd, &set);

	memset((char *) &serv_addr, 0,sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
	serv_addr.sin_port = htons(portno);

	// connection process with server
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) { printf("error on connection"); exit(0); }
  
	while(!stop) {

		// authentication check
		if (!auth) {
			printf("Enter Credential: ");
			bzero(buffer,1024);
			fgets(buffer, 1023, stdin);

		if (write(sockfd,buffer,strlen(buffer)) < 0 ) {
			printf("error on sending credential");
			exit(0);
		} else {
				
			memset(buffer,'\0',strlen(buffer));

			if (read(sockfd,buffer,1024) < 0) {
				printf("error on getting response");
				exit(0);
			} else {
				if (strncmp(buffer,"authenticated\n",14) == 0) {
					printf("Authenticated.\n");
					auth = 1;
				} else {
					printf("Authentication failure.\n");
					auth = 0;
				}				
			}
		}

		} else {

			// command execution area
    			printf("cmd ~ $ ");
    			memset(buffer,'\0',1024);
    			fgets(buffer,1023,stdin);
    	
			// case command help, displays help message
			if (strncmp(buffer, "help\n",5) == 0 || strncmp(buffer, "h\n",2) == 0) { 
				printf("Type shell commands here to be executed on the server.\nCommand execution errors will not be displayed.\nUse 'quit' to leave without killing the server connection.\n");	
			} else {
				write(sockfd,buffer,1024);

				bzero(buffer,1024);

				// exit condition
				if (strncmp(buffer, "quit\n",5) == 0) stop = 1;

				// timeout setup to not get locked when connection drops or something else happens
				timeout.tv_sec = 3;
       				timeout.tv_usec = 300000;
	
   				if(select(sockfd+1, &set, NULL, NULL, &timeout) <= 0) {
	
					printf("timeout\n");
	
				} else {
	
					// response from command await
					memset(buffer,'\0',1024);

					if (read(sockfd,buffer,1024) < 0) printf("error on receiving message\n");
    					else if (strncmp(buffer,"no output\n", 10) != 0) {
						printf("%s\n",buffer);
					}
				}

			}
	
		}
	}
	close(sockfd);
	return 0;
}
