CC=g++
CFLAGS=-std=c++14 -Wall -pedantic -ggdb
INCLUDES=-I lib/linux/
LFLAGS=
LIBS=-lSOIL -lGL -lglfw -ldl
SRC=src
SRCS=$(shell find ${SRC}/ -type f -iname "*.cpp")
OBJS=$(SRCS:.cpp=.o)
MAIN=dm3d

.PHONY: clean

all: $(MAIN)

$(MAIN): $(OBJS)
	$(CC) $(CLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) ${SRC}/*.o $(MAIN)

