all: async_server server chat_server client chat_client

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

chat_server: chat_server.o
	g++ -o chat_server chat_server.o -lboost_system -lboost_thread -lpthread 

chat_server.o: src/chat/chat_server.cpp
	g++ -c -std=c++11 src/chat/chat_server.cpp
	
chat_client: chat_client.o
	g++ -o chat_client chat_client.o -lboost_system -lboost_thread -lpthread 

chat_client.o: src/chat/chat_client.cpp
	g++ -c -std=c++11 src/chat/chat_client.cpp
	
clean:
	rm -f *.o async_server server chat_server chat_client client

#CC = g++
#CFLAGS = -c -std=c++11
#BOOST = -lboost_system -lboost_thread -lpthread 
#SOURCES = src/server.cpp src/client.cpp src/async_server.cpp src/chat/server.cpp 
#OBJECTS = ($SOURCES:.cpp=.o)
#EXECUTABLE = client server async_server chat_server
#all: $(OBJECTS) $(EXECUTABLE)
#
#client: client.o
#	$(CC) client.o -o $@  
#		
#async_server: async_server.o
#	$(CC) server.o -o $@  
#	
#server: server.o
#	$(CC) async_server.o -o $@  
#	
#chat_server: chat_server.o
#	$(CC) chat_server.o -o $@  
#	
#.cpp.o: *.h
#	$(CC) $(CFLAGS) $(BOOST) $< -o$@
#	
#claen:
#	rm -f $(OBJECTS) $(EXECUTABLE)
#
#.PHONY: all clean
#	
	