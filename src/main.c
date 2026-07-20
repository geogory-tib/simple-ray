/*
Copyright (c) 2004-2021, Lode Vandevenne

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>

#define DEGREE_TO_RADIANS(X)(X * PI/180)


/*
g++ *.cpp -lSDL -O3 -W -Wall -ansi -pedantic
g++ *.cpp -lSDL
*/

//place the example code below here:

#define screenWidth 1280
#define screenHeight 720
#define mapWidth 20
#define mapHeight 20
#define DEFAULT_MOVE_SPEED  0.1450 / 4
const int screen_width_half = screenWidth / 2;
const int screen_height_half = screenHeight / 2;
typedef struct{
  double x;
  double y;
  double fov;
  double pa;
  double mv_speed;
  double max_view;
}player_t;
player_t player = {4.0,3.0,90,90.0,DEFAULT_MOVE_SPEED};
double g_time;
double old_time;
struct{
  double increment_angle;
  double prec;
}rc_data = {.prec = 64.0,.increment_angle = 90.0/screenWidth};
int worldMap[mapHeight][mapWidth] = {
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,1,1,1,1,0,0,0,1,1,1,1,0,0,0,1},
  {1,0,0,0,0,1,0,0,1,0,0,0,1,0,0,1,0,0,0,1},
  {1,0,0,0,0,1,0,0,1,0,0,0,1,0,0,1,0,0,0,1},
  {1,0,0,0,0,1,0,0,1,0,0,0,1,0,0,1,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};

void raycast(){
  double half_fov = player.fov / 2;
  double ray_angle = player.pa - half_fov;
  BeginDrawing();
  ClearBackground(GRAY);
 
  for(int ray_cnt = 0;ray_cnt < screenWidth;ray_cnt++){
	double ray_x = player.x;
	double ray_y = player.y;
	double rayCos = cos(DEGREE_TO_RADIANS(ray_angle)) / rc_data.prec;
	double raySin = sin(DEGREE_TO_RADIANS(ray_angle)) / rc_data.prec;
	int wall;
	for(wall = 0; wall == 0; wall = worldMap[(int)ray_y][(int)ray_x]){
	  ray_x += rayCos;
	  ray_y += raySin;
	  /* if(ray_x > player.max_view || ray_y > player.max_view){ */
	  /* 	break; */
	  /* } */
	}
	if(wall == 0){
	  continue;
	}
	double ray_diffX = player.x - ray_x;
	double ray_diffY = player.y - ray_y;
	double dist = sqrt((pow(ray_diffX, 2)) + pow(ray_diffY,2));
	double angle_dif = ray_angle - player.pa ;
	/* if (angle_dif < -180) angle_dif += 360; */
    /* if (angle_dif > 180)  angle_dif -= 360; */
	/* dist = dist * cos(DEGREE_TO_RADIANS(angle_dif)); */
	printf("\n%d: wall=%d dist=%f wall_ind = %d,%d\n", ray_cnt, wall, dist,(int)ray_x,(int)ray_y);
	double wall_height = (double)screen_height_half / dist;
	Vector2 line_start = {.x = ray_cnt,.y = screen_height_half - wall_height};
	Vector2 line_end = {.x = ray_cnt,.y = screen_height_half + wall_height};
	//Color color_map[6] = {GREEN,RED,BLUE,WHITE,RAYWHITE,PURPLE};
	DrawLineV(line_start, line_end, RED);
	ray_angle += rc_data.increment_angle;
  }
  EndDrawing();
}

void player_input(){
  double paCos = cos(DEGREE_TO_RADIANS(player.pa)) * player.mv_speed;
  double paSin = sin(DEGREE_TO_RADIANS(player.pa)) * player.mv_speed;
  double speed = player.mv_speed;
  if(IsKeyDown(KEY_RIGHT_SHIFT) || IsKeyDown(KEY_LEFT_SHIFT)){
	speed *= 1.5;
  }
  if(IsKeyDown(KEY_W)){
	double new_player_x = 	player.x + paCos;
	double new_player_y = 	player.y + paSin;
	if(!worldMap[(int)new_player_y][(int)new_player_x] != 0){
	  player.x = new_player_x;
	  player.y = new_player_y;
	}
  }
  if(IsKeyDown(KEY_S)){
	double new_player_x = 	player.x - paCos;
	double new_player_y = 	player.y - paSin;
	if(!worldMap[(int)new_player_y][(int)new_player_x] != 0){
	  player.x = new_player_x;
	  player.y = new_player_y;
	}
  }
  if(IsKeyDown(KEY_A)){
	double angle_in_radians = DEGREE_TO_RADIANS(player.pa);
	double strfCos = cos( angle_in_radians - PI/2) * speed;
	double strfSin = sin(angle_in_radians - PI/2) * speed;
	double new_player_x = 	player.x + strfCos;
	double new_player_y = 	player.y + strfSin;
	if(!worldMap[(int)new_player_y][(int)new_player_x] != 0){
	  player.x = new_player_x;
	  player.y = new_player_y;
	}
  }
  if(IsKeyDown(KEY_D)){
	double angle_in_radians = DEGREE_TO_RADIANS(player.pa);
	double strfCos = cos( angle_in_radians + PI/2) * speed;
	double strfSin = sin(angle_in_radians + PI/2) * speed;
	double new_player_x = 	player.x + strfCos;
	double new_player_y = 	player.y + strfSin;
	if(!worldMap[(int)new_player_y][(int)new_player_x]){
	  player.x = new_player_x;
	  player.y = new_player_y;
	}
  }
	
  Vector2 mouse_delta = GetMouseDelta();
  if(mouse_delta.x != 0){
	player.pa += mouse_delta.x;
    if(player.pa > 360.0){
	  player.pa = 0;
	}
	if(player.pa < 0.0){
	  player.pa = 360.0;
	}
  }
}


int main()
{
  InitWindow(screenWidth, screenHeight, "Raycasting");
  SetTargetFPS(60);
  
  while(!WindowShouldClose()){

	raycast();
	player_input();
  }
	
}

