all: async_server server client

async_server: async_server.o
	g++ -o async_server async_server.o -lboost_system -lboost_thread -lpthread 

async_server.o: src/async_server.cpp
	g++ -c src/async_server.cpp
	
server: server.o
	g++ -o server server.o -lboost_system -lboost_thread -lpthread 

server.o: src/server.cpp
	g++ -c src/server.cpp
	
client: client.o
	g++ -o client client.o -lboost_system -lboost_thread -lpthread
	
client.o: src/client.cpp
	g++ -c src/client.cpp

clean:
	rm -f *.o async_server server client