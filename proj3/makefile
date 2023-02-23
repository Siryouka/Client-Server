CC = g++

flags = -std=c++17 
flags += -Wall 
flags += -g

text-client: ./ram/reader/text_reader.cc ./ram/threading/threadload.cc ./ram/shm/shm.cc ./ram/sock/unix_domain_socket.cc ./ram/sock/domain_socket_client.cc ./client.cc
	$(CC) -o text-client ./ram/threading/threadload.cc ./ram/reader/text_reader.cc ./ram/shm/shm.cc ./ram/sock/unix_domain_socket.cc ./ram/sock/domain_socket_client.cc ./client.cc -lrt -lpthread

text-server: ./ram/reader/text_reader.cc ./ram/threading/threadload.cc ./ram/shm/shm.cc ./ram/sock/unix_domain_socket.cc ./ram/sock/domain_socket_server.cc ./server.cc
	$(CC) -o text-server ./ram/threading/threadload.cc ./ram/reader/text_reader.cc ./ram/shm/shm.cc ./ram/sock/unix_domain_socket.cc ./ram/sock/domain_socket_server.cc ./server.cc -lrt -lpthread

all:
	make text-client
	make text-server

clean:
	rm *.o text-server text-client