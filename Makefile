LIBS =  -lm -lpthread -lGL -ldl -lrt -lX11

all: mapeditor engine

engine:
	cc src/engine/*.c $(LIBS) -O3 -march=native -o raycasting -lraylib
mapeditor:
	cc src/map_editor/*c $(LIBS) -O3 -march=native -o mapeditor -lraylib
debug: debug_engine debug_mapeditor

debug_engine:
	cc src/engine/*.c $(LIBS) -Wall -DDEBUG -g -o raycasting /home/john/lib/raylib/lib/libraylib.a
debug_mapeditor:
	cc src/map_editor/*c $(LIBS) -O0 -g -o mapeditor -lraylib
