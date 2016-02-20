#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/select.h>

#define MYPORT "2005"
#define BACKLOG 10

int main(int argc, char *argv[]){
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    struct addrinfo hints, *res;
    int sockfd, new_fd;
    char s[INET6_ADDRSTRLEN];
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(argv[1], MYPORT, &hints, &res);
    if(status != 0){
	fprintf(stderr, "getaddrinfo error: %s\n",gai_strerror(status));
	return 1;
    }

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(sockfd == -1){
	perror("Socket Error");
	return 2;
    }

    if(connect(sockfd, res->ai_addr, res->ai_addrlen) == -1){
	close(sockfd);
	perror("Client connect");
	return 3;
    }

    printf("connecting to server\n");
    while(1){
	char buf[1024];
	while(!fgets(buf, 1024, stdin));
	send(sockfd, buf, strlen(buf)+1, 0);
	recv(sockfd, buf, 1023, 0);
	printf("%s", buf);
    }
}
