LIBS =  -lm -lpthread -lGL -ldl -lrt -lX11

all:
	cc src/*.c $(LIBS) -o raycasting 
debug:
	cc src/*.c $(LIBS) -g -o  raycasting /home/john/lib/raylib/lib/libraylib.a
