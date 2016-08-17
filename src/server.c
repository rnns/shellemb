#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#define portno 1234 // port that is going to be used

int connected = 0, loop, newsockfd;

// exception handling on broken connection pipe
void sig_pipe(int signum)
{
	printf("broken socket pipe\n");
	connected = 0;
	loop = 0;
	if (newsockfd) close(newsockfd);
	signal(SIGPIPE, sig_pipe);
}

int main(int argc, char *argv[])
{
	int sockfd, i;
	socklen_t clilen;
	char buffer[1024], buffer2[1024];
	char* cmdout;
	struct sockaddr_in serv_addr, cli_addr;
	FILE *fd;
	size_t chread;
	size_t comalloc = 1024;
	size_t comlen   = 0;

	// socket creation process
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) { 
		printf("error on creating socket\n");
		exit(0);
	} else
		printf("socket created\n");

	// ip_address and port treatment
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	// socket binding process
	if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
		printf("error on binding\n");
		exit(0);
	} else {
		printf("socket binded\n");

     		clilen = sizeof(cli_addr);
		
		// listening for incoming conections on portno
		printf("listening on port %d\n", portno);
     		listen(sockfd,5);

		while(1) {
                	
			// exception handling on function sig_pipe
			signal(SIGPIPE, sig_pipe);

			// acceptance of incoming connection 
     			newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
     			if (newsockfd < 0) {

          			printf("error on accepting new socket connection\n");

			} else {

				printf("new socket connection accepted\n");
				connected = 1;

				// athentication of connected client
				while(connected) {
					i=0;
					while(i<1024 && buffer[i]!='\0') {buffer[i]='\0';i++;}
					if (read(newsockfd,buffer,1024) < 0) printf("error on reading socket to validate client");

					else {
						// password used by client
						if(strncmp(buffer, "password\n", 9) == 0) {
							printf("user authenticated\n");
							write(newsockfd, "authenticated\n", 14);
							loop = 1;
							break;
						} else {
							printf("authentication failure with password: %s\n", buffer);
							write(newsockfd, "authentication failure\n", 22);
							loop = 0;
							break;
						}	
					}
				}

				// info trading loop
     				while (loop) {

     					cmdout = malloc(comalloc);
					i = 0;
					while(i<1024 && (buffer[i] != '\0'|| buffer2[i] !='\0')) {buffer[i]='\0';buffer2[i]='\0';i++;}
					i = 0;
					while(i<1024 && cmdout[i]!='\0') {cmdout[i]='\0';i++;}

					// receiving data
     					if (read(newsockfd,buffer,1024) < 0) printf("error when reading from socket\n");
     					else {
						printf("cmd received: %s\n",buffer);
						
						// end connection case, when user digits quit
						if (strncmp(buffer, "quit\n", 5) == 0) {
							loop = 0;
							connected = 0;
							printf("user disconnected\n");
							write(newsockfd,"disconnected\n", 13);

						} else {
							// command execution on shell
    							fd = popen(buffer, "r");
							comlen = 0;
						
							// read from command return
   							while ((chread = fread(buffer2, 1, sizeof(buffer2), fd)) != 0) {
        							if (comlen + chread >= comalloc) {
           								comalloc *= 2;
            								cmdout = realloc(cmdout, comalloc);
        							}
        							memmove(cmdout + comlen, buffer2, chread);
        							comlen += chread;
    							}
    							
							// output through socket from response
							if (comlen == 0) {
								fwrite("no output\n", 1, 10, stdout);
								write(newsockfd,"no output\n",10);
							} else {
								fwrite(cmdout, 1, comlen, stdout);
								if (write(newsockfd,cmdout,comlen) < 0) printf("error on writing to socket\n"); 

							}

							// pipe from command execution closed
   							pclose(fd);
							//system(buffer);
							// variable with message return from command execution freed 
							free(cmdout);
						}
					}	
     				}
        		}

			// socket closed due to quit call
     			close(newsockfd);
     		}
	}
	close(sockfd);
	return 0; 
}
