#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>

main()
{
	struct sockaddr_in server, client;
	int s, n, sock, pid;
	char b1[100], b2[100] = "reply from server 1";
	s = socket(AF_INET,SOCK_STREAM,0);
	server.sin_family = AF_INET;
	server.sin_port = 49001;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	bind(s, (struct sockaddr *)&server, sizeof server);
	listen(s, 10);
	n = sizeof(client);
	while(1) {
		sock = accept(s, (struct sockaddr *)&client, &n);
		if((pid = fork()) != 0) {
			close(sock);
			continue;
		} else {
			while(1)
			{
				recv(sock, b1, sizeof b1, 0);
				if(strcmp(b1, "end") == 0) {
					send(sock, "end", sizeof("end"), 0);
					break;
				}
				printf("client: %s\n",b1);
				send(sock, b2, sizeof b2, 0);
			}
			printf("closing client (%d) connection\n", sock);
			close(sock);
			break;
		}
	}
//	close(s);
	return 0;
}

