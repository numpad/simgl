CC=g++
CFLAGS=-std=c++14 -Wall -pedantic -ggdb -fno-strict-aliasing
INCLUDES=-I lib/linux/
LFLAGS=
LIBS=-lGL -lglfw -ldl -lpthread
SRC=src
SRCS_LIB=lib/linux/gl3w.cpp
SRCS=$(shell find ${SRC}/ -type f -iname "*.cpp") ${SRCS_LIB}
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

