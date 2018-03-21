CC=g++
CFLAGS=-std=c++14 -Wall -pedantic -ggdb -fno-strict-aliasing
INCLUDES=-I lib/linux/ -I lib/linux/angelscript/angelscript/include/ -I lib/linux/angelscript/add_on/
LFLAGS=-L lib/linux/angelscript/angelscript/lib/
LIBS=-Wl,-Bstatic -langelscript -Wl,-Bdynamic -lGL -lglfw -ldl -lpthread
SRC=src
SRCS_LIB=lib/linux/imgui/imgui.cpp lib/linux/imgui/imgui_draw.cpp lib/linux/imgui/imgui_impl.cpp lib/linux/gl3w.cpp $(shell find lib/linux/angelscript/add_on/* -type f -iname "*.cpp")
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

