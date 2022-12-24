#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

int nclients = 0;

void CopyFile(int);

void error(const char *msg)
{
	perror(msg);
	exit(0);
}

void printusers()
{
	if(nclients)
	{
		printf("Users on-line: %d\n", nclients);
	}
	else
	{
		printf("Noone is on-line");
	}
}

int main(int argc, char *argv[])
{
	char buff[1024];
	int sockfd, newsockfd;
	int portno;
	int pid;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;

	printf("TCP server test\n");

	if(argc != 2)
	{
		printf("Usage: %s <Port>", argv[0]);
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		error("Error opening socket");
	}

	bzero((char*) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		error("Error on binding");
	}

	listen(sockfd, 5);
	clilen = sizeof(cli_addr);

	while(1)
	{
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0)
		{
			error("Error on accept");
		}

		struct hostent *hst;
		hst = gethostbyaddr((char*)&cli_addr.sin_addr, 4, AF_INET);

		printf("Added %s [%s] new connection\n", (hst) ? hst->h_name : "Unknown host",
		 (char*)inet_ntoa(cli_addr.sin_addr));

		pid = fork();
		if(pid < 0)
		{
			error("Error on fork");
		}
		if(pid == 0)
		{
			close(sockfd);
			CopyFile(newsockfd);
			exit(0);
		}
		else
		{
			close(newsockfd);
		}
	}

	close(sockfd);
	return 0;
}

void CopyFile (int sock)
{
	int bytes_recv;
	char buff[1024];
	nclients++;
	printusers();

	FILE* file;
	file = fopen("RecievedFile.txt", "wb");
	if (file == NULL)
	{
		error("Error creating file");
		return;
	}

	bytes_recv = read(sock, &buff[0], sizeof(buff));
	if (bytes_recv < 0)
	{
		error("Error reading from socket");
	}

	if(strlen(buff)<1024)
	{
		fwrite(&buff, sizeof(char), strlen(buff), file);
		printf("Done copying file----\n");
	}
	else
	{
		fwrite(&buff, sizeof(char), sizeof(buff), file);
	}

	nclients--;
	printf("-disconnecting\n");
	printusers();
	return;
}
