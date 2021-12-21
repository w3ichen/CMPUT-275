CC=g++
CFLAGS=-c -Wall -O2
LFLAGS=-static
OBJS=student_hash.o complex_postfix.o
PROGRAM=student calc

all: student calc

student: student_hash.o student_hash.cpp
	$(CC) student_hash.cpp -o student $(LFLAGS)

student_hash.o: student_hash.cpp
	$(CC) student_hash.cpp -o student_hash.o $(CFLAGS)

calc: complex_postfix.o complex_postfix.cpp
	$(CC) complex_postfix.cpp -o calc $(LFLAGS)

complex_postfix.o: complex_postfix.cpp
	$(CC) student_hash.cpp -o student_hash.o $(CFLAGS)


clean:
	@rm -f $(OBJS)
	@rm -f $(PROGRAM)