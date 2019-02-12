CC=g++
CFLAGS=-std=c++14 -Wall -pedantic -ggdb
INCLUDES=-I lib/linux/
LFLAGS=
LIBS=-lGL -lglfw -ldl -lpthread -lopenal
SRC=src
SRCS_LIB=lib/linux/gl3w.cpp lib/linux/nuklear/nuklear.c lib/linux/imgui/imgui.cpp lib/linux/imgui/imgui_draw.cpp lib/linux/imgui/imgui_impl.cpp lib/linux/stb_vorbis.c
SRCS=$(shell find ${SRC}/ -type f -iname "*.cpp") ${SRCS_LIB}
OBJS=$(SRCS:.cpp=.o)
MAIN=dm3d

.PHONY: clean

all: $(MAIN)
	./$(MAIN)

$(MAIN): $(OBJS)
	$(CC) $(CLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) ${SRC}/*.o $(MAIN)

