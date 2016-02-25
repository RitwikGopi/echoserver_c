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
    int sockfd, new_fd, i, j;
    char s[INET6_ADDRSTRLEN];
    int fdmax;
    
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
    printf("socket created no = %d\n", sockfd);
    if(bind(sockfd, res->ai_addr, res->ai_addrlen) == -1){
	perror("BInd error");
	return 3;
    }
    if(listen(sockfd, BACKLOG) == -1){
	perror("listen error");
	return 4;
    }
    printf("listen fun over\n");
    addr_size = sizeof their_addr;
    fd_set read_fds;
    fd_set master;
    FD_ZERO(&read_fds);
    FD_ZERO(&master);
    FD_SET(0, &master);
    FD_SET(sockfd, &master);
    fdmax = sockfd;
    while(1){
	read_fds = master;
	select(fdmax + 1, &read_fds, NULL, NULL, NULL);
	for(i = 0; i <= fdmax; i++){
	    if(FD_ISSET(i, &read_fds)){
		if(i == 0){
		    char buf[1024];
		    printf("STDIN SELECTED\n");
		    while(!fgets(buf, 1024, stdin));
		}else if(i == sockfd){
		    printf("Incoming request SELECTED\n");
		    new_fd = accept(sockfd, 
			    (struct sockaddr *)&their_addr, &addr_size);
		    printf("cllient connected %d\n", new_fd);
		    FD_SET(new_fd, &master);
		    if(fdmax < new_fd)
			fdmax = new_fd;
		}else{
		    printf("message from client %d\n", i);
		    char buf[1024] = "";
		    int nbytes = recv(i, buf, sizeof buf, 0);
		    printf("NO of recv = %d,%d, %s\n", nbytes, 
			    strlen(buf),buf);
		    if(nbytes == 0){
			printf("connection closed\n");
			close(i);
			FD_CLR(i, &master);
		    }else{
			for(j = sockfd + 1; j <= fdmax; j++){
			    if(FD_ISSET(j, &master)){
				printf("sending to %d\n", j);
				send(j, buf, strlen(buf) + 1, 0);
			    }
			}
		    }
		}
	    }
	}
    }
}
