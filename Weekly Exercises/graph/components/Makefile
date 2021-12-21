CC=g++
CFLAGS=-c -Wall -O2
LFLAGS=-static
OBJS=graph_concepts.o 
PROGRAM=graph_concepts

graph_concepts: graph_concepts.o graph_concepts.cpp
	$(CC) graph_concepts.cpp -o graph_concepts $(LFLAGS)

graph_concepts.o: graph_concepts.cpp
	$(CC) graph_concepts.cpp -o graph_concepts.o $(CFLAGS)

clean:
	@rm -f $(OBJS)
	@rm -f $(PROGRAM)