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

int main(void){
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    struct addrinfo hints, *res;
    int sockfd, new_fd;
    char s[INET6_ADDRSTRLEN];
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(NULL, MYPORT, &hints, &res);
    if(status != 0){
	fprintf(stderr, "getaddrinfo error: %s\n",gai_strerror(status));
	return 1;
    }

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(sockfd == -1){
	perror("Socket Error");
	return 2;
    }
    if(bind(sockfd, res->ai_addr, res->ai_addrlen) == -1){
	perror("BInd error");
	return 3;
    }
    if(listen(sockfd, BACKLOG) == -1){
	perror("listen error");
	return 4;
    }


    addr_size = sizeof their_addr;
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
    if(new_fd == -1){
	perror("accept error");
	return 5;
    }
    printf("client connected %s\n", s);
    while(1){
	char buf[1024] ;
	int buf_s = recv(new_fd, buf, 1023, 0);
	printf("%s\n",buf);
	if(buf_s == -1){
	    perror("recv");
	    return 6;
	}
	int send_s = send(new_fd, buf, strlen(buf)+1, 0);
	printf("%d-%d\n", strlen(buf), send_s);
	sleep(1);
    }
}
