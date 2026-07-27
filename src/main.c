

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>
#include <math.h>
const float  PI_DIV_180 = PI / 180.0; 
#define DEGREE_TO_RADIANS(X)(X * PI_DIV_180)


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
typedef enum{
  TYP_FLOOR = 0,
  TYP_WALL
}grid_type;

typedef struct{
  grid_type type;
  int texture_id;
}cell_t;
typedef struct{
  float x;
  float y;
  float fov;
  float half_fov;
  float pa;
  float mv_speed;
  float mse_sens;
}player_t;
player_t player = {2.0,2.0,90,45,90.0,DEFAULT_MOVE_SPEED,DEFAULT_MOUSE_SENS};
struct{
  float increment_angle;
  float prec;
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
Texture skybox;
float skybox_y_inc;
Texture floor_text;
Texture loaded_textures[100];



/* unsigned int worldMap[10][10] = */
/* { */
/*     {1001,1001,1001,1001,1001,1001,1001,1001,1001,1001}, */
/*     {1001,   0,   0,   0,   0,   0,   0,   0,   0,1001}, */
/*     {1001,   0,   0,   0,   1001,   0,   0,   0,   0,1001}, */
/*     {1001,   0,   0,   0,   0,   0,   0,   0,   0,1001}, */
/*     {1001,   0,   0, 0 ,    0,  0,  0,0,  0,    1001}, */
/*     {1001,   0,   0,   0,  0,   0,   0,   0,   0,1001}, */
/*     {1001,   0,   0,   0,   0,   0,   0,   0,   0,1001}, */
/*     {1001,   0,   0,   0,   1001,   0,   0,   0,   0,1001}, */
/*     {1001,   0,   0,   0,   0,   0,   0,   0,   0,1001}, */
/*     {1001,1001,1001,1001,1001,1001,1001,1001,1001,1001} */
/* }; */

cell_t worldMap[10][10] =
{
    { {TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,0},{TYP_WALL,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_WALL,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_WALL,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,0},{TYP_WALL,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_WALL,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0} }
};



// TODO -- FIX HOW FUCKING SLOW THE SKYBOX RENDERING IS - MAKE THIS SHIT WORK AND WORK GOOD!


void draw_texture(Vector2 wall_pos,int texture_x,double wall_height,Texture texture){
  /* float y_inc = (wall_height * 2) / texture.height; */
  /* int col = (texture_x * texture.height); */
  /* for(int i = 0;i < texture.height;i++){ */
  /* 	//	DrawPixel(start.x, y,texture.contents[(texture_x * texture.width) + i]); */
  /* 	Vector2 line_start = {.x = wall_pos.x, .y = wall_pos.y}; */
  /* 	Vector2 line_end = {.x = wall_pos.x, .y = wall_pos.y += y_inc}; */
  /* 	DrawLineV(line_start, line_end,texture.contents[col + i]); */
  /* } */
  Rectangle texture_rect = {.x = texture_x,.y = 0,.height = texture.height,.width = 1};
  Rectangle screen_rect = {.x = wall_pos.x,.y = wall_pos.y,.height = wall_height * 2,.width = 1};
  DrawTexturePro(texture, texture_rect, screen_rect, (Vector2){0.0f,0.0f},0.0f, WHITE);
}

// just use the gpu to draw this lol. DrawTexturePro
void draw_skybox(unsigned int skybox_pos){
  /* float y = 0; */
  /* int row = skybox_pos * skybox.width; */
  /* skybox_pos = skybox_pos & skybox.width - 1; */
  /* for(int i = 0;i < skybox.height && y < wall_y;i++){ */
  /* 	//	DrawPixel(start.x, y,texture.contents[(texture_x * texture.width) + i]); */
  /* 	Vector2 line_start = {.x = x, .y = y}; */
  /* 	Vector2 line_end = {.x = x, .y = y += skybox_y_inc}; */
  /* 	DrawLineV(line_start, line_end,skybox.contents[row + i]); */
  // }
  Rectangle skybox_rect = {.x = skybox_pos,.y = 0,.width = skybox.width, .height = skybox.height};
  Rectangle screen_rect = {.x = 0,.y = 0,.height = project.half_height,.width = project.width};
  DrawTexturePro(skybox, skybox_rect, screen_rect, (Vector2){0.0}, 0.0f, WHITE);
}
void floor_cast(){
  /* float player_dir_x; */
  /* float player_dir_y; */
  /* sincosf(DEGREE_TO_RADIANS(player.pa), &player_dir_y, &player_dir_x); */
  /* float plane_x = -player_dir_y; */
  /* float plane_y = player_dir_x; */
  /* float fov_tan = tanf(DEGREE_TO_RADIANS(player.half_fov)); */
  /* plane_x *= fov_tan; */
  /* plane_y *= fov_tan;*/
  float right_angle = player.pa + player.half_fov;
  float left_angle = player.pa - player.half_fov;
  float rayDirX0;
  float rayDirY0;
  float rayDirX1; 
  float rayDirY1;
  sincosf(DEGREE_TO_RADIANS(left_angle), &rayDirY0, &rayDirX0);
  sincosf(DEGREE_TO_RADIANS(right_angle), &rayDirY1, &rayDirX1);
  Image floor_img = LoadImageFromTexture(floor_text);
  Color *floor_colors = LoadImageColors(floor_img);
  Image buffered_floor = GenImageColor(project.width,project.half_height, BLANK);
  for(int y = project.half_height;y < project.height;y++){
	  float posZ = 0.5 * project.height;
	  int horizon_dif = y - project.half_height + 1;
	  float row_dist = posZ / horizon_dif;
	  float floor_step_x = row_dist * (rayDirX1 - rayDirX0) / (project.width - 1);
      float floor_step_y = row_dist * (rayDirY1 - rayDirY0) / (project.width - 1);
	  float floor_x = player.x + row_dist  * rayDirX0;
      float floor_y = player.y + row_dist * rayDirY0;
	  for(int x = 0; x < project.width;x++){
		int cellX = (int)floor_x;
		int cellY = (int)floor_y;
		int texture_x = (int)(floor_img.width * (floor_x - cellX)) & (floor_img.width - 1);
		int texture_y = (int)(floor_img.height * (floor_y - cellY)) & (floor_img.height - 1);
		floor_x += floor_step_x;
		floor_y += floor_step_y;
		ImageDrawPixel(&buffered_floor, x, y - project.half_height, floor_colors[floor_img.width * texture_y + texture_x]);
		//DrawPixel(x, y, floor_colors[floor_img.width * texture_y + texture_x]);
	  }
  }
  Texture floor_buf_text = LoadTextureFromImage(buffered_floor);
  Rectangle source_rect = {.x = 0,.y = 0,.width = floor_buf_text.width, .height = floor_buf_text.height};
  Rectangle dest_rect = {.x = 0,.y = project.half_height,.height = project.half_height,.width = project.width};
  DrawTexturePro(floor_buf_text, source_rect, dest_rect, (Vector2){0.0f,0.0f}, 0.0f, WHITE);
  // UnloadTexture(floor_buf_text);
}
void raycast(float ray_angle,float raySin, float rayCos,float cosDelta, float sinDelta){
  for(int ray_cnt = 0;ray_cnt < project.width;ray_cnt++){
   	/* float rayCos = fast_cosine(ray_angle) /\* / rc_data.prec *\/; */
	/* float raySin = fast_sin(ray_angle) /\* / rc_data.prec *\/; */
	int wall = 0;
	float delta_dist_x = fabs(1 / rayCos);
	float delta_dist_y = fabs(1/ raySin);
	float side_dist_x;
	float side_dist_y;
	int step_x;
	int step_y;
	int mapx = (int)player.x;
	int mapy = (int)player.y;
	int side;
	if(rayCos < 0){
	  step_x = -1;
	  side_dist_x = (player.x - (float)mapx) *delta_dist_x;
	}else{
	  step_x = 1;
	  side_dist_x = ((float)mapx + 1.0 - player.x) * delta_dist_x;
	}
	if(raySin < 0){
	  step_y = -1;
	  side_dist_y = (player.y - (float)mapy) *delta_dist_y;
	}else{
	  step_y = 1;
	  side_dist_y = ((float)mapy + 1.0 - player.y) * delta_dist_y;
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
	  wall = worldMap[mapy][mapx].type;
	}
	cell_t wall_cell = worldMap[mapx][mapy];
	float dist;
	float hitx;
	if(side == 0){
	  dist = (side_dist_x - delta_dist_x);
	  hitx = player.y + (dist * raySin);
	}
	else{
	  dist = (side_dist_y - delta_dist_y);
	  hitx = player.x + (dist * rayCos);
	}
	hitx -= floorf(hitx);

	float wall_height = (float)project.half_height / dist;
	//Color color_map[6] = {GREEN,RED,BLUE,WHITE,RAYWHITE,PURPLE};
	Vector2 wall_pos = {.x = ray_cnt, .y = project.half_height - wall_height};
	Texture texture = loaded_textures[wall_cell.texture_id];
	int texture_pos = ((int)(texture.width * hitx) & (texture.width - 1));	
	// printf("\n%d: wall=%d  dist=%f raySin = %lf rayCos = %lf ray_angle = %lf wall_ind = %d,%d hitx = %lf player_pos = %lf,%lf player_angle = %lf skybox_x = %d\n", ray_cnt, wall,hitx, dist,raySin,rayCos, ray_angle,mapx,mapy,player.x,player.y,player.pa,skybox_pos); 
	draw_texture(wall_pos,texture_pos,wall_height,texture);
	float new_cos = rayCos * cosDelta - raySin * sinDelta;
	raySin = raySin * cosDelta +  rayCos  * sinDelta;
	rayCos = new_cos;
	ray_angle += rc_data.increment_angle;
  }
  
  
}
void render(){
  float ray_angle = player.pa - player.half_fov;
  float rayCos /* = cosf(DEGREE_TO_RADIANS(ray_angle)); */;
  float raySin /*= sinf(DEGREE_TO_RADIANS(ray_angle)); */;
  sincosf(DEGREE_TO_RADIANS(ray_angle),&raySin,&rayCos);
  float rot_delta = DEGREE_TO_RADIANS(rc_data.increment_angle);
  float sinDelta;
  float cosDelta;
  sincosf(DEGREE_TO_RADIANS(rc_data.increment_angle),&sinDelta,&cosDelta);
  unsigned int skybox_pos = (((unsigned int)(ray_angle / 360 * skybox.width) % skybox.width));
  if(skybox_pos < 0){
	skybox_pos = 0;
  }
  BeginDrawing();
  draw_skybox(skybox_pos);
  floor_cast();
  raycast(ray_angle, raySin, rayCos,cosDelta,sinDelta);
  DrawFPS(0,0);
  EndDrawing();
}


void player_input(){
  float speed = player.mv_speed;
  if(IsKeyDown(KEY_RIGHT_SHIFT) || IsKeyDown(KEY_LEFT_SHIFT)){
	speed *= 1.5;
  }
  if(IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)){
	speed *= 0.5;
  }
  float paCos = cos(DEGREE_TO_RADIANS(player.pa)) * speed;
  float paSin = sin(DEGREE_TO_RADIANS(player.pa)) * speed;
  if(IsKeyDown(KEY_W)){
	float new_player_x = 	player.x + paCos;
	float new_player_y = 	player.y + paSin;
	if(!worldMap[(int)new_player_y][(int)new_player_x].type){
	  player.x = new_player_x;
	  player.y = new_player_y;
	}
  }
  if(IsKeyDown(KEY_S)){
	float new_player_x = 	player.x - paCos;
	float new_player_y = 	player.y - paSin;
	if(!worldMap[(int)new_player_y][(int)new_player_x].type){
	  player.x = new_player_x;
	  player.y = new_player_y;
	}
  }
  if(IsKeyDown(KEY_A)){
	float angle_in_radians = DEGREE_TO_RADIANS(player.pa);
	float strfCos = cosf( angle_in_radians - PI/2) * speed;
	float strfSin = sinf(angle_in_radians - PI/2) * speed;
	float new_player_x = 	player.x + strfCos;
	float new_player_y = 	player.y + strfSin;
	if(!worldMap[(int)new_player_y][(int)new_player_x].type){
	  player.x = new_player_x;
	  player.y = new_player_y;
	}
  }
  if(IsKeyDown(KEY_D)){
	float angle_in_radians = DEGREE_TO_RADIANS(player.pa);
	float strfCos = cosf(angle_in_radians + PI/2) * speed;
	float strfSin = sinf(angle_in_radians + PI/2) * speed;
	float new_player_x = 	player.x + strfCos;
	float new_player_y = 	player.y + strfSin;
	if(!worldMap[(int)new_player_y][(int)new_player_x].type){
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
  // SetConfigFlags(FLAG_FULLSCREEN_MODE);
  InitWindow(screen.width, screen.height, "Raycasting");
  DisableCursor();
  SetTargetFPS(60);
  project.width = screen.width / screen.proj_scale;
  project.height = screen.height / screen.proj_scale;
  project.half_width = project.width / 2;
  project.half_height = project.height / 2;
  rc_data.increment_angle = player.fov / project.width;
}

void load_textures(){
  /* Image img_skybox = LoadImage("betterbox.png"); */
  /* skybox.height = img_skybox.height; */
  /* skybox.width = img_skybox.width; */
  /* skybox.contents = convert_image_to_colum_major(img_skybox); */
  /* skybox_y_inc = ((float)project.half_height / (float)skybox.height); */
  /* skybox.contents = calloc(skybox.width * skybox.height, sizeof(Color)); */
  /* for(int i = 0; i < skybox.width * skybox.height;i++){ */
  /* 	skybox.contents[i] = GRAY; */
  /* } */
  /* Image test_text = LoadImage("brick128.png"); */
  /* loaded_textures[0].contents = convert_image_to_colum_major(test_text); */
  /* loaded_textures[0].width = test_text.width; */
  /* loaded_textures[0].height = test_text.height; */
  skybox = LoadTexture("skyboxtest.png");
  loaded_textures[0] = LoadTexture("brick128.png");
  floor_text = LoadTexture("bricks64.png");
}


int main()
{
  init_screen();
 
  load_textures();
  while(!WindowShouldClose()){
	render();
	player_input();
  }
  CloseWindow();
}

