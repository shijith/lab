#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#define SERVER_NUM 2

typedef struct server_connection {
	int server, client;
} server_conn;

typedef struct client_connection {
	int client, servers[SERVER_NUM];
}client_conn;

void *p_server_with_servers(void *conn)
{
	char buff[100];
	int client, server;
	client = ((server_conn *)conn)->client;
	server = ((server_conn *)conn)->server;

//	printf("server (%d), client (%d)\n", server, client);

	while(1) {
		recv(server, buff, sizeof(buff), 0);
		printf("Received from server (%d): %s\n", server, buff);
		send(client, buff, sizeof(buff), 0);
		printf("Send to client (%d) from server (%d): %s\n", client, server, buff);
		if(strcmp(buff, "end") == 0) {
			close(server);
			break;
		}
	}
	pthread_exit(NULL);
}

void *p_server_with_client(void *conn)
{
	char buff[100], err_msg[100] = "proxy: unknown destination\n";
	int *servers, client;
	client = ((client_conn *)conn)->client;
	servers = ((client_conn *)conn)->servers;

//	printf("server %d client %d\n", servers[0], client);

	while(1) {
		recv(client, buff, sizeof(buff), 0);
		if(strcmp(buff, "end") == 0)
			break;
		switch (buff[0]) {
			case '1':
				printf("to server 1: %s\n", buff + 1);
				send(servers[0], buff + 1, sizeof(buff) - 1, 0);
				break;
			case '2':
				printf("to server 2: %s\n", buff + 1);
				send(servers[1], buff + 1, sizeof(buff) - 1, 0);
				break;
			default:
				printf("some invalid client message: %s\n", buff);
				send(client, err_msg, sizeof(err_msg), 0 );
				break;
		}
	}
	close(client);
	pthread_exit(NULL);

}


main()
{
	struct sockaddr_in proxy_server, client, server1, server2;
	int ps, n, sock, s1, s2, pid;
	int reuse_addr = 1;
	struct server_connection srv1, srv2;
	struct client_connection cli;
	pthread_t t_client, t_server1, t_server2;

	ps = socket(AF_INET,SOCK_STREAM,0);
	proxy_server.sin_family = AF_INET;
	proxy_server.sin_port = 49000;
	proxy_server.sin_addr.s_addr = inet_addr("127.0.0.1");

	s1 = socket(AF_INET,SOCK_STREAM,0);
	server1.sin_family = AF_INET;
	server1.sin_port = 49001;
	server1.sin_addr.s_addr = inet_addr("127.0.0.1");

	s2 = socket(AF_INET,SOCK_STREAM,0);
	server2.sin_family = AF_INET;
	server2.sin_port = 49002;
	server2.sin_addr.s_addr = inet_addr("127.0.0.1");

	bind(ps,(struct sockaddr *)&proxy_server,sizeof proxy_server);
	listen(ps,10);
	n = sizeof(client);
	while(1)
	{
		sock = accept(ps,(struct sockaddr *)&client,&n);
		if((pid = fork()) != 0) {
			close(sock);
			continue;
		} else {
			if(connect(s1, (struct sockaddr *)&server1, sizeof(server1)) < 0)
				perror("while connecting to server 1");
			if(connect(s2, (struct sockaddr *)&server2, sizeof(server2)) < 0)
				perror("while connecting to server 2");

			srv1.server = s1;
			srv2.server = s2;
			cli.servers[0] = s1;
			cli.servers[1] = s2;

			srv1.client = srv2.client = sock;
			cli.client = sock;

			pthread_create(&t_client, 0, p_server_with_client, (void *)&cli);
			pthread_create(&t_server1, 0, p_server_with_servers, (void *)&srv1);
			pthread_create(&t_server2, 0, p_server_with_servers, (void *)&srv2);

			pthread_join(t_server1, NULL);
			pthread_join(t_server2, NULL);
			pthread_join(t_client, NULL);

			printf("closing connection for servers (%d, %d) with client (%d)\n", s1, s2, sock);
//			close(s1);
//			close(s2);
//			close(sock);
			break;
		}
	}

	close(ps);
}

