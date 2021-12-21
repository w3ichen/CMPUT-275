CC=g++
CFLAGS=-c -Wall -O2
LFLAGS=-static
OBJS=exercise4.o 
PROGRAM=exercise4

exercise4: exercise4.o exercise4.cpp
	$(CC) exercise4.cpp -o exercise4 $(LFLAGS)

exercise4.o: exercise4.cpp
	$(CC) exercise4.cpp -o exercise4.o $(CFLAGS)

clean:
	@rm -f $(OBJS)
	@rm -f $(PROGRAM)