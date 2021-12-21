CC=g++
CFLAGS=-c -Wall -O2
LFLAGS=-static
OBJS=flipflop.o textmatch.o
PROGRAM=flipflop textmatch

flipflop: flipflop.o flipflop.cpp
	$(CC) flipflop.cpp -o flipflop $(LFLAGS)

textmatch: textmatch.o textmatch.cpp
	$(CC) textmatch.cpp -o textmatch $(LFLAGS)

flipflop.o: flipflop.cpp
	$(CC) flipflop.cpp -o flipflop.o $(CFLAGS)

textmatch.o: textmatch.cpp
	$(CC) textmatch.cpp -o textmatch.o $(CFLAGS)

clean:
	@rm -f $(OBJS)
	@rm -f $(PROGRAM)