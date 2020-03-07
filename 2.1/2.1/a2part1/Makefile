CC=g++
CFLAGS=-c -Wall -O2 -std=c++11
LFLAGS=
PROGS= server
OBJS= server.o dijkstra.o digraph.o

server: server.o dijkstra.o digraph.o
	$(CC) server.cpp dijkstra.cpp digraph.cpp -o server $(LFLAGS)

server.o: server.cpp
	$(CC) server.cpp -o server.o $(CFLAGS)

dijkstra.o: dijkstra.h
	$(CC) dijkstra.h -o dijkstra.o $(CFLAGS)

digraph.o: digraph.h
	$(CC) digraph.h -o digraph.o $(CFLAGS)

clean:
	@rm $(OBJS) $(PROGS)
