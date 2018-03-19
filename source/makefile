CPP = g++
CC = gcc
CFLAGS = -c
CPPFLAGS = -c -std=c++11
FLAGS = -framework OpenGL -lglfw

OBJECTC = glad.o main.o

CSOURCES = \
	 glad.c

CPPSOURCES = \
	main.cpp 

main: 
	$(CPP) $(CPPFLAGS) $(CPPSOURCES) && $(CC) $(CFLAGS) $(CSOURCES) && $(CPP) $(FLAGS) $(OBJECTC) -o main

.PHONY: all main clean
clean:
	$(RM) *.o
