all: proxy client s1 s2

proxy: proxy.c
	cc proxy.c -o proxy -pthread
client: tcp_c.c
	cc tcp_c.c -o client
s1: tcp_s1.c
	cc tcp_s1.c -o s1
s2: tcp_s2.c
	cc tcp_s2.c -o s2
