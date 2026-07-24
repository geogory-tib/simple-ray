LIBS =  -lm -lpthread -lGL -ldl -lrt -lX11

all:
	cc src/*.c $(LIBS) -O3 -o raycasting -lraylib
debug:
	cc src/*.c $(LIBS) -Wall -g -o  raycasting /home/john/lib/raylib/lib/libraylib.a
