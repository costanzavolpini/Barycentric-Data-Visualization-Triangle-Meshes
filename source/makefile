CPP = g++
CC = gcc
CFLAGS = -c
CPPFLAGS = -c -std=c++11 `pkg-config --cflags gtk+-3.0`
FLAGS = -lglfw
GTKFLAGS = `pkg-config --libs gtk+-3.0`

OBJECTC = glad.o main.o

CSOURCES = \
	 glad.c

CPPSOURCES = \
	main.cpp

main:
	$(CPP) $(CPPFLAGS) $(CPPSOURCES) && $(CC) $(CFLAGS) $(CSOURCES) && $(CPP) $(FLAGS) $(OBJECTC) -o main $(GTKFLAGS)

.PHONY: all main clean
clean:
	$(RM) *.o main
