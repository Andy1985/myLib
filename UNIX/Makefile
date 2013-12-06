CC = gcc
LINK = gcc
RM = rm
FLAGS = -Wall #-Werror
FLAGS += -g 
FLAGS += -DHAVE_MSGHDR_MSG_CONTROL
INCS = -I./
LIBS = -lpthread

TARGET = server client test tcp_client udp_client daytime_client iteration_server \
		mycat openfile select_server web udp_server timeout_server timeout_client \
		send_mob recv_mob recv_mob2 send_mob2 heartbeat_client heartbeat_server \
		sigio_udp_server pthread_tcp_client pthread_tcp_server pthread_web \
		test_pthread test_client fork_server prefork_server fork_poll_server \
		pthread_server pthread_poll_server

all: $(TARGET)

server: server.o
	$(LINK) -o $@ server.o $(LIBS)
client: client.o
	$(LINK) -o $@ client.o $(LIBS)
tcp_client: tcp_client.o
	$(LINK) -o $@ tcp_client.o $(LIBS)
udp_client: udp_client.o
	$(LINK) -o $@ udp_client.o $(LIBS)
daytime_client: daytime_client.o
	$(LINK) -o $@ daytime_client.o $(LIBS)
iteration_server: iteration_server.o
	$(LINK) -o $@ iteration_server.o $(LIBS)
mycat: mycat.o
	$(LINK) -o $@ mycat.o $(LIBS)
openfile: openfile.o
	$(LINK) -o $@ openfile.o $(LIBS)
select_server: select_server.o
	$(LINK) -o $@ select_server.o $(LIBS)
web: web.o
	$(LINK) -o $@ web.o $(LIBS)
udp_server: udp_server.o
	$(LINK) -o $@ udp_server.o $(LIBS)
timeout_server: timeout_server.o
	$(LINK) -o $@ timeout_server.o $(LIBS)
timeout_client: timeout_client.o
	$(LINK) -o $@ timeout_client.o $(LIBS)
send_mob: send_mob.o
	$(LINK) -o $@ send_mob.o $(LIBS)
recv_mob: recv_mob.o
	$(LINK) -o $@ recv_mob.o $(LIBS)
recv_mob2: recv_mob2.o
	$(LINK) -o $@ recv_mob2.o $(LIBS)
send_mob2: send_mob2.o
	$(LINK) -o $@ send_mob2.o $(LIBS)
heartbeat_client: heartbeat_client.o
	$(LINK) -o $@ heartbeat_client.o $(LIBS)
heartbeat_server: heartbeat_server.o
	$(LINK) -o $@ heartbeat_server.o $(LIBS)
sigio_udp_server: sigio_udp_server.o
	$(LINK) -o $@ sigio_udp_server.o $(LIBS)
pthread_tcp_client: pthread_tcp_client.o
	$(LINK) -o $@ pthread_tcp_client.o $(LIBS)
pthread_tcp_server: pthread_tcp_server.o
	$(LINK) -o $@ pthread_tcp_server.o $(LIBS)
pthread_web: pthread_web.o
	$(LINK) -o $@ pthread_web.o $(LIBS)
test_pthread: test_pthread.o
	$(LINK) -o $@ test_pthread.o $(LIBS)
ping: ping.o
	$(LINK) -o $@ ping.o $(LIBS)
test_client: test_client.o
	$(LINK) -o $@ test_client.o $(LIBS)
fork_server: fork_server.o
	$(LINK) -o $@ fork_server.o $(LIBS)
prefork_server: prefork_server.o
	$(LINK) -o $@ prefork_server.o $(LIBS)
fork_poll_server: fork_poll_server.o
	$(LINK) -o $@ fork_poll_server.o $(LIBS)
pthread_server: pthread_server.o
	$(LINK) -o $@ pthread_server.o $(LIBS)
pthread_poll_server: pthread_poll_server.o
	$(LINK) -o $@ pthread_poll_server.o $(LIBS)

test: test.o
	$(LINK) -o $@ test.o $(LIBS)

%.o: %.c
	$(CC) -o $@ -c $< $(INCS) $(FLAGS)

.PHONY: clean
clean:
	$(RM) $(TARGET) -f
	$(RM) *.o -f
