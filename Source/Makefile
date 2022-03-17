OBJS = main.cpp

INCLUDE = include

CC = g++

LINKER_FLAGS = -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lassimp

OBJ_NAME = main

all : $(OBJS)
		$(CC) $(OBJS) $(INCLUDE)/*.c* $(LINKER_FLAGS) -o $(OBJ_NAME)