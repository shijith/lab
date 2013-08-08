#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

main()
{
	struct sockaddr_in server,client;
	int s,n;
	char b1[100],b2[100];
	s = socket(AF_INET,SOCK_STREAM,0);
	server.sin_family = AF_INET;
	server.sin_port = 49000;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	if(connect(s,(struct sockaddr *)&server,sizeof server) == -1)
		perror("in connect");
	printf("client %d\n", s);
	while(1)
	{
		memset(b1, 0, 100);
		memset(b2, 0, 100);
		printf("client: ");
		scanf("%s",b2);
		if(send(s,b2,sizeof b2,0) <= 0)
			perror("in send");
		if(strcmp(b2,"end") == 0)
			break;
		if(recv(s,b1,sizeof b1,0) <= 0)
			perror("in recv");
		printf("server: %s\n",b1);

	}
}

