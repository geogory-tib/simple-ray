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

#define DEFAULT_SCREEN_WIDTH 1280
#define DEFAULT_SCREEN_HEIGHT 720
#define DEFAULT_PROJECTION_SCALE 1
#define mapWidth 10
#define mapHeight 10
#define DEFAULT_MOVE_SPEED  0.1450 / 4
#define DEFAULT_MOUSE_SENS 0.50
#define TEXTURE_WALL 1000
typedef struct{
  int width;
  int height;
  Color *contents;
}texture_t;

typedef struct{
  double x;
  double y;
  double fov;
  double pa;
  double mv_speed;
  double mse_sens;
}player_t;
player_t player = {2.0,2.0,90,90.0,DEFAULT_MOVE_SPEED,DEFAULT_MOUSE_SENS};
double g_time;
double old_time;
struct{
  double increment_angle;
  double prec;
}rc_data = {.prec = 64.0};
struct{
  int width;
  int height;
  int half_width;
  int half_height;
  int proj_scale;
}screen = {DEFAULT_SCREEN_WIDTH,DEFAULT_SCREEN_HEIGHT, DEFAULT_SCREEN_WIDTH/2,DEFAULT_SCREEN_HEIGHT/2,DEFAULT_PROJECTION_SCALE};

struct{
  int width;
  int height;
  int half_height;
  int half_width;
}project = {0};
texture_t skybox = {.width = 64, .height = 64};
double skybox_y_inc;
texture_t floor_text = {0};
texture_t loaded_textures[100];



unsigned int worldMap[10][10] =
{
    {1001,1001,1001,1001,1001,1001,1001,1001,1001,1001},
    {1001,   0,   0,   0,   0,   0,   0,   0,   0,1001},
    {1001,   0,   0,   0,   1001,   0,   0,   0,   0,1001},
    {1001,   0,   0,   0,   0,   0,   0,   0,   0,1001},
    {1001,   0,   0, 0 ,    1001,  0,  0,0,  0,    1001},
    {1001,   0,   0,   0,   0,   0,   0,   0,   0,1001},
    {1001,   0,   0,   0,   0,   0,   0,   0,   0,1001},
    {1001,   0,   0,   0,   0,   0,   0,   0,   0,1001},
    {1001,   0,   0,   0,   0,   0,   0,   0,   0,1001},
    {1001,1001,1001,1001,1001,1001,1001,1001,1001,1001}
};



// TODO -- FIX HOW FUCKING SLOW THE SKYBOX RENDERING IS - MAKE THIS SHIT WORK AND WORK GOOD!


void draw_texture(Vector2 wall_pos,int texture_x,double wall_height,texture_t texture){
  double y_inc = (wall_height * 2) / texture.height;
  int col = (texture_x * texture.height);
  for(int i = 0;i < texture.height;i++){
	//	DrawPixel(start.x, y,texture.contents[(texture_x * texture.width) + i]);
	Vector2 line_start = {.x = wall_pos.x, .y = wall_pos.y};
	Vector2 line_end = {.x = wall_pos.x, .y = wall_pos.y += y_inc};
	DrawLineV(line_start, line_end,texture.contents[col + i]);
  }
}
void draw_skybox(unsigned int skybox_pos,int x,int wall_y){
  double y = 0;
  int row = skybox_pos * skybox.width;
  skybox_pos = skybox_pos & skybox.width - 1;
  for(int i = 0;i < skybox.height && y < wall_y;i++){
	//	DrawPixel(start.x, y,texture.contents[(texture_x * texture.width) + i]);
	Vector2 line_start = {.x = x, .y = y};
	Vector2 line_end = {.x = x, .y = y += skybox_y_inc};
	DrawLineV(line_start, line_end,skybox.contents[row + i]);
  }
}
Color *convert_image_to_colum_major(Image img){
  Color *ret = malloc((img.height * img.width) * sizeof(Color));
  Color *contents = LoadImageColors(img);
  int x = 0;
  int y = 0;
  for(;x < img.width;){
	ret[(x * img.height) + y] = contents[y * img.width + x];
	y++;
	if(y > img.height){
	  x++;
	  y = 0;
	}
	
  }
  UnloadImage(img);
  // UnloadImageColors(contents);
  return ret;
}
void raycast(){
  double half_fov = player.fov / 2;
  double ray_angle = player.pa - half_fov;
  BeginDrawing();
  ClearBackground(GRAY);
  for(int ray_cnt = 0;ray_cnt < project.width;ray_cnt++){
	double rayCos = cos(DEGREE_TO_RADIANS(ray_angle)) /* / rc_data.prec */;
	double raySin = sin(DEGREE_TO_RADIANS(ray_angle)) /* / rc_data.prec */;
	int wall = 0;
	double delta_dist_x = fabs(1 / rayCos);
	double delta_dist_y = fabs(1/ raySin);
	double side_dist_x;
	double side_dist_y;
	int step_x;
	int step_y;
	int mapx = (int)player.x;
	int mapy = (int)player.y;
	int side;
	if(rayCos < 0){
	  step_x = -1;
	  side_dist_x = (player.x - (double)mapx) *delta_dist_x;
	}else{
	  step_x = 1;
	  side_dist_x = ((double)mapx + 1.0 - player.x) * delta_dist_x;
	}
	if(raySin < 0){
	  step_y = -1;
	  side_dist_y = (player.y - (double)mapy) *delta_dist_y;
	}else{
	  step_y = 1;
	  side_dist_y = ((double)mapy + 1.0 - player.y) * delta_dist_y;
	}
	while(!wall){
	  if(side_dist_x < side_dist_y){
		side_dist_x += delta_dist_x;
		mapx += step_x;
		side = 0;
	  }else{
		side_dist_y += delta_dist_y;
		mapy += step_y;
		side  = 1;
	  }
	  wall = worldMap[mapy][mapx];
	}
	double dist;
	Color clr = BLUE;
	double hitx;
	if(side == 0){
	  dist = (side_dist_x - delta_dist_x);
	  hitx = player.y + (dist * raySin);
	}
	else{
	  dist = (side_dist_y - delta_dist_y);
	  hitx = player.x + (dist * rayCos);
	  hitx -= floor(hitx);
	}

	double wall_height = (double)project.half_height / dist;
	//Color color_map[6] = {GREEN,RED,BLUE,WHITE,RAYWHITE,PURPLE};
	Vector2 wall_pos = {.x = ray_cnt, .y = project.half_height - wall_height};

	if(wall != 1){
	  int texture_id = wall & ~ (TEXTURE_WALL); texture_id--;
	  texture_t texture = loaded_textures[texture_id];
	  int texture_pos = ((int)(texture.width * hitx) & (texture.width - 1));
	  unsigned int skybox_pos =(((unsigned int)(ray_angle / 360 * skybox.width) % skybox.width));
	  if(skybox_pos < 0){
		skybox_pos = 0;
	  }
	  printf("\n%d: wall=%d dist=%f wall_ind = %d,%d hitx = %lf player_pos = %lf,%lf player_angle = %lf skybox_x = %d\n", ray_cnt, wall,hitx, dist,mapx,mapy,player.x,player.y,player.pa,skybox_pos); 
	  draw_skybox(skybox_pos, ray_cnt,wall_pos.y);
	  draw_texture(wall_pos,texture_pos,wall_height,texture);
	}
	ray_angle += rc_data.increment_angle;
  }
  DrawFPS(0, 0);
  EndDrawing();
}

void player_input(){
  double speed = player.mv_speed;
  if(IsKeyDown(KEY_RIGHT_SHIFT) || IsKeyDown(KEY_LEFT_SHIFT)){
	speed *= 1.5;
  }
  if(IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)){
	speed *= 0.5;
  }
  double paCos = cos(DEGREE_TO_RADIANS(player.pa)) * speed;
  double paSin = sin(DEGREE_TO_RADIANS(player.pa)) * speed;
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
	double strfCos = cos(angle_in_radians + PI/2) * speed;
	double strfSin = sin(angle_in_radians + PI/2) * speed;
	double new_player_x = 	player.x + strfCos;
	double new_player_y = 	player.y + strfSin;
	if(!worldMap[(int)new_player_y][(int)new_player_x]){
	  player.x = new_player_x;
	  player.y = new_player_y;
	}
  }
  if(IsKeyDown(KEY_F1)){
	EnableCursor();
  }
  if(IsKeyDown(KEY_F2)){
	DisableCursor();
  }
  Vector2 mouse_delta = GetMouseDelta();
  if(mouse_delta.x != 0){
	player.pa += mouse_delta.x * player.mse_sens;
    if(player.pa > 360.0){
	  player.pa = 0;
	}
	if(player.pa < 0.0){
	  player.pa = 360.0;
	}
  }
}

void init_screen(){
  InitWindow(screen.width, screen.height, "Raycasting");
  DisableCursor();
  /* SetTargetFPS(60); */
  project.width = screen.width / screen.proj_scale;
  project.height = screen.height / screen.proj_scale;
  project.half_width = project.width / 2;
  project.half_height = project.height / 2;
  rc_data.increment_angle = player.fov / project.width;
}

void load_textures(){
  Image img_skybox = LoadImage("betterbox.png");
  skybox.height = img_skybox.height;
  skybox.width = img_skybox.width;
  skybox.contents = convert_image_to_colum_major(img_skybox);
  skybox_y_inc = ((double)project.half_height / (double)skybox.height);
  /* skybox.contents = calloc(skybox.width * skybox.height, sizeof(Color)); */
  /* for(int i = 0; i < skybox.width * skybox.height;i++){ */
  /* 	skybox.contents[i] = GRAY; */
  /* } */
  Image test_text = LoadImage("brick128.png");
  loaded_textures[0].contents = convert_image_to_colum_major(test_text);
  loaded_textures[0].width = test_text.width;
  loaded_textures[0].height = test_text.height;
}

int main()
{
  init_screen();
  load_textures();
  while(!WindowShouldClose()){
	raycast();
	player_input();
  }
}

