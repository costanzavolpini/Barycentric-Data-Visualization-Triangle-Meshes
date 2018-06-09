CPP = g++
CC = gcc
CFLAGS = -c
CPPFLAGS = -c -std=c++11 -Wall -Wformat
FLAGS = -lglfw

EXE = main

CSOURCES = glad.c

CPPSOURCES = main.cpp imgui_impl_glfw_gl3.cpp
CPPSOURCES += imgui.cpp imgui_draw.cpp

OBJECTCPP = $(addsuffix .o, $(basename $(notdir $(CPPSOURCES))))

OBJECTC = $(addsuffix .o, $(basename $(notdir $(CSOURCES))))


all: $(EXE)
	@echo Build complete!

$(EXE): $(OBJS)
	$(CPP) $(CPPFLAGS) $(CPPSOURCES) && $(CC) $(CFLAGS) $(CSOURCES) && $(CPP) $(FLAGS) $(OBJECTCPP) $(OBJECTC) -o main

clean:
	rm -f $(EXE) $(OBJECTCPP) $(OBJECTC)

