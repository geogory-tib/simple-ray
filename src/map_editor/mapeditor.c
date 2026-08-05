#define RAYGUI_IMPLEMENTATION
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>
#include <math.h>
#include "../include/raygui.h"

typedef struct{
  int type;
  int texture_id;
}cell_t;

typedef struct{
  int texture_id;
  Vector3 pos;
}sprite;
sprite sprites[100];
int current_sprite;
struct{
  cell_t *buffer;
  int height;
  int width;
}map_data = {.buffer = NULL,.height = 10,.width = 10};
int window_width = 1280;
int window_height = 720;
char texture_folder_boxText[1024];
char output_folder_boxText[1024];
bool setup_complete  = false;
void setup_map_data(){
  static  bool texture_folder_boxEditMode = false;
  static  bool output_folder_boxEditMode = false;
  BeginDrawing();
  ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
   GuiGroupBox((Rectangle){ 0, 48, 432, 232 }, "Provide Texture, Sprite Textures and output path");
            if (GuiTextBox((Rectangle){ 24, 88, 240, 40 }, texture_folder_boxText, 128, texture_folder_boxEditMode)) texture_folder_boxEditMode = !texture_folder_boxEditMode;
            if (GuiTextBox((Rectangle){ 24, 160, 240, 40 }, output_folder_boxText, 128, output_folder_boxEditMode)) output_folder_boxEditMode = !output_folder_boxEditMode;
            GuiLabel((Rectangle){ 24, 64, 240, 24 }, "Texture Folder");
            GuiLabel((Rectangle){ 24, 136, 240, 24 }, "Map Output");
            setup_complete = GuiButton((Rectangle){304, 248, 120, 24 }, "Submit"); 
  EndDrawing();
}

int main(void){
  InitWindow(window_width, window_height,"mapcaster");
  GuiLoadStyle("style_terminal.rgs");
  SetTargetFPS(60);
  while(!WindowShouldClose()){
	setup_map_data();
	if(setup_complete){
	  //map_editor_loop();
	}
  }
}
