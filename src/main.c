

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
#define DEFAULT_MOVE_SPEED  2.5
#define DEFAULT_MOUSE_SENS 0.50
#define TEXTURE_WALL 1000
typedef struct{
  int width;
  int height;
  Color *contents;
}texture_t;
typedef struct{
  Texture text;
  bool loaded;
}gpu_loaded_texture;
typedef enum{
  TYP_FLOOR = 0,
  TYP_WALL
}grid_type;

typedef struct{
  texture_t spr_txt;
  float x;
  float y;
  float z;
  float wrld_width;
  float wrld_height;
}sprite_t;
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
player_t player = {1.5,1.5,90,45,0,DEFAULT_MOVE_SPEED,DEFAULT_MOUSE_SENS};
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
bool enable_skybox = false;
float skybox_y_inc;
texture_t  ceiling_text = {0};
texture_t loaded_textures[100];
gpu_loaded_texture gpu_loaded_textures[100];


// TODO -- WORK ON SPRITE RENDERING!

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

/* cell_t worldMap[10][10] = */
/* { */
/*     { {TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0} }, */

/*     { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_WALL,0} }, */

/*     { {TYP_WALL,0},{TYP_FLOOR,0},{TYP_WALL,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_WALL,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_WALL,0} }, */

/*     { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_WALL,0} }, */

/*     { {TYP_WALL,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_WALL,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_WALL,0} }, */

/*     { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_WALL,0} }, */

/*     { {TYP_WALL,0},{TYP_FLOOR,0},{TYP_WALL,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_WALL,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_WALL,0} }, */

/*     { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_WALL,0} }, */

/*     { {TYP_WALL,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_FLOOR,0},{TYP_WALL,0} }, */

/*     { {TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0} } */
/* }; */

cell_t worldMap[20][20] =
{
    { {TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0} }
};

sprite_t sprites[10];
// used to map the depth for each col of the screen will just be allocated to screen_width
float *zbuffer;

// TODO -- This shit Isn't working some I am just going to make a buffer than just write that to the screen;

Image FrameBuffer;
Texture FrameText;
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
  UnloadImageColors(contents);
  return ret;
}
texture_t load_engine_texture(char *filename){
  texture_t ret = {0};
  Image img = LoadImage(filename);
  ret.contents  =  convert_image_to_colum_major(img);
  ret.width = img.width;
  ret.height = img.height;
  UnloadImage(img);
  return ret;
}
// using this because I am going to attempt to off load more to the gpu to see if that will be faster not sure if it will be
// after thinking about it, it will honestly end up being the same amount of work, I will still have to manually draw pixels strip
// by strip with this, I can't even offload the wall drawing purely to the CP
void draw_wall_strip(Vector2 wall_pos,int texture_x,double wall_height,int texture_id){
  // have to do this due to the way I have to software render some textures I am too dumb to figure out how to do this on gpu
  if(!gpu_loaded_textures[texture_id].loaded){
	texture_t engine_text = loaded_textures[texture_id];
	Image texture_image = {.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,.data = engine_text.contents,.width = engine_text.width,.height = engine_text.height,.mipmaps = 1 };
	gpu_loaded_textures[texture_id].text = LoadTextureFromImage(texture_image);
	gpu_loaded_textures[texture_id].loaded = true;
  }
  Texture texture = gpu_loaded_textures[texture_id].text;
  Rectangle texture_rect = {.x = texture_x,.y = 0,.height = texture.height,.width = 1};
  Rectangle screen_rect = {.x = wall_pos.x,.y = wall_pos.y,.height = wall_height * 2,.width = 1};
  DrawTexturePro(texture, texture_rect, screen_rect, (Vector2){0.0f,0.0f},0.0f, WHITE);
}
void draw_texture(Vector2 wall_pos,int texture_x,double wall_height,texture_t texture){
  float y_inc = (wall_height * 2) / texture.height;
  int col = (texture_x * texture.height);
  for(int i = 0;i < texture.height;i++){
	Vector2 line_start = {.x = wall_pos.x, .y = wall_pos.y};
	Vector2 line_end = {.x = wall_pos.x, .y = wall_pos.y += y_inc};
	ImageDrawLineV(&FrameBuffer,line_start, line_end,texture.contents[col + i]);
  }
}

// just use the gpu to draw this lol. DrawTexturePro
void draw_skybox(unsigned int skybox_pos){
  Rectangle skybox_rect = {.x = skybox_pos,.y = 0,.width = skybox.width, .height = skybox.height};
  Rectangle screen_rect = {.x = 0,.y = 0,.height = screen.half_height,.width = screen.width};
  DrawTexturePro(skybox, skybox_rect, screen_rect, (Vector2){0.0}, 0.0f, WHITE);
}

void draw_ceiling(int x,float wall_height,float rayCos,float raySin){
  float start = 0;
  float y = start;
  for(;y < project.height - wall_height;){
	float dist = project.height / (project.height - 2 * y);
	float tilex = dist * rayCos;
	float tiley = dist * raySin;
	tilex += player.x;
	tiley += player.y;
	int text_x = (int)(tilex * ceiling_text.width) & (ceiling_text.width - 1);
	int text_y = (int)(tiley * ceiling_text.height) & (ceiling_text.height - 1);
	ImageDrawPixelV(&FrameBuffer,(Vector2){.x = x,.y = y},ceiling_text.contents[text_x * ceiling_text.height + text_y]);
	y++;
  }
}

void draw_floor(int x,float wall_height,float rayCos,float raySin){
  float start = project.half_height + wall_height;
  float y = start;
  for(;y < project.height;){
	float dist = project.height / (2 * y - project.height);
	float tilex = dist * rayCos;
	float tiley = dist * raySin;
	tilex += player.x;
	tiley += player.y;
	cell_t floor_cell = worldMap[(int)tiley][(int)tilex];
	texture_t floor_text = loaded_textures[floor_cell.texture_id];
	int text_x = (int)(tilex * floor_text.width) & (floor_text.width - 1);
	int text_y = (int)(tiley * floor_text.height) & (floor_text.height - 1);
	ImageDrawPixelV(&FrameBuffer,(Vector2){.x = x,.y = y},floor_text.contents[text_x * floor_text.height + text_y]);
	y++;
  }
}

void raycast(float ray_angle,float raySin, float rayCos,float cosDelta, float sinDelta){
  for(int ray_cnt = 0;ray_cnt < project.width;ray_cnt++){
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
	cell_t wall_cell = worldMap[mapy][mapx];
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
	Vector2 wall_pos = {.x = ray_cnt, .y = project.half_height - wall_height};
	texture_t texture = loaded_textures[wall_cell.texture_id];
	int texture_pos = ((int)(texture.width * hitx) & (texture.width - 1));
	if(!enable_skybox){
	  draw_ceiling(ray_cnt, wall_height, rayCos, raySin);
	}
	draw_texture(wall_pos,texture_pos,wall_height,texture);
	//draw_wall_strip(wall_pos, texture_pos, wall_height,wall_cell.texture_id);
	draw_floor(ray_cnt, wall_height, rayCos, raySin);
	float new_cos = rayCos * cosDelta - raySin * sinDelta;
	raySin = raySin * cosDelta +  rayCos  * sinDelta;
	rayCos = new_cos;
	ray_angle += rc_data.increment_angle;
	zbuffer[ray_cnt] = dist;
  }
  
}
// for now it's just one sprite
/* void draw_sprites(){ */
/*   sprite_t sprite = sprites[0]; */
/*   float dx = sprite.x - player.x; */
/*   float dy = sprite.y - player.y; */
/*   float dz = sprite.z; */
/*   float pa_radians = DEGREE_TO_RADIANS(player.pa); */
/*   float pa_sin = sinf(pa_radians); */
/*   float pa_cos = cosf(pa_radians); */
/*   // rotated around players angle */
/*   float a=dy*pa_cos+dx*pa_sin; */
/*   float b=dx*pa_cos-dy*pa_sin; */
/*   // 3d projection onto the screen */
/*   float screen_x = (a /b)+(project.half_width); */
/*   float screen_y = (dz/ b)+(project.half_height); */
/*   #ifdef DEBUG */
/*   printf("dx = %f, dy = %f   a = %f b = %f, screen_y = %f, screen_x %f\n",dx,dy,a,b,screen_y,screen_x); */
/*   #endif */
/*   if(b < 0) */
/* 	return; */
/*   ImageDrawRectangleV(&FrameBuffer, (Vector2){.x = screen_x,.y = screen_y}, (Vector2){.x = 100,.y = 100}, YELLOW); */
/* } */

void draw_sprites(void)
{
    sprite_t sprite = sprites[0];

    // Sprite position relative to player.
    float dx = sprite.x - player.x;
    float dy = sprite.y - player.y;

    float angle = DEGREE_TO_RADIANS(player.pa);
    float pa_sin = sinf(angle);
    float pa_cos = cosf(angle);

    // Transform world position into camera space.
    float depth =  dx * pa_cos + dy * pa_sin;
    float side  = -dx * pa_sin + dy * pa_cos;

    // Behind the camera or too close to project safely.
    /* if (depth <= 0.001f) { */
    /*     return; */
    /* } */

    // Outside horizontal FOV.
    if (fabsf(side) > depth) {
        return;
    }

    float focal_length =
        (float)project.half_width;

    // Horizontal screen coordinate of the sprite's center.
    float screen_x =
        (float)project.half_width +
        (side / depth) * focal_length;

    /*
     * Example vertical setup:
     *
     * z = 0 is the floor.
     * Camera is 0.5 world units above the floor.
     * sprite.z is the sprite's floor/base position.
     */
    const float camera_z = 0.5f;
    const float sprite_world_width = 0.5f;
    const float sprite_world_height = 0.5f;

    float sprite_bottom_z = sprite.z;
    float sprite_top_z = sprite.z + sprite.wrld_height;

  
    float screen_bottom =(float)project.half_height -((sprite_bottom_z - camera_z) / depth) * focal_length;

    float screen_top = (float)project.half_height -((sprite_top_z - camera_z) / depth) * focal_length;

    float screen_width = (sprite.wrld_width / depth) * focal_length;

    float screen_height = screen_bottom - screen_top;

    // screen_x represents the center, while ImageDrawRectangleV receives
    // a rectangle position and size. :contentReference[oaicite:1]{index=1}
	float draw_start_x = screen_x - screen_width * 0.5f;
	float draw_end_x = screen_x + screen_width;
	float draw_start_y = screen_top;
	float draw_end_y = screen_top + screen_height;
	for(int strip = (int)draw_start_x;strip < draw_end_x;strip++){
	  float u = (strip - draw_start_x)  / screen_width;
	  int texture_x = u * sprite.spr_txt.width;
	  if(texture_x >= sprite.spr_txt.width){
		texture_x = sprite.spr_txt.width - 1;
	  }
	  int col = texture_x * sprite.spr_txt.height;
	  if(strip < project.width && depth <= zbuffer[strip]){
		for(int y = draw_start_y;y < draw_end_y;y++){
		  float v =  (y - draw_start_y) / screen_height;
		  int texture_y = v * sprite.spr_txt.height;
		  if(texture_y >= sprite.spr_txt.height){
				texture_y = sprite.spr_txt.height - 1;
		  }
		  if(sprite.spr_txt.contents[col + texture_y].a == 0){
			continue;
		  }
		  ImageDrawPixel(&FrameBuffer,strip,y,sprite.spr_txt.contents[col + texture_y]);
		}
	  }
	}
    //ImageDrawRectangleV(&FrameBuffer, position, size, YELLOW);
}

void render(){
  // clear horizon if using skybox
  if(enable_skybox){
	//	ImageDrawRectangle(&FrameBuffer, 0, 0, project.width, project.half_height, BLANK);
	ImageClearBackground(&FrameBuffer, BLANK);
  }
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
  if(enable_skybox){
	draw_skybox(skybox_pos);
  }
  //floor_cast();
  raycast(ray_angle, raySin, rayCos,cosDelta,sinDelta);
  draw_sprites();
  UpdateTexture(FrameText, FrameBuffer.data);
  // DrawTexture(FrameText, 0, 0, WHITE);
  Rectangle buffer_rect = {.x = 0,.y = 0,.width = FrameText.width,.height = FrameText.height};
  Rectangle screen_rect = {.x = 0,.y = 0,.width = screen.width,screen.height};
  DrawTexturePro(FrameText, buffer_rect, screen_rect, (Vector2){0.0f,0.0f}, 0.0f, WHITE);
  DrawFPS(0,0);
  EndDrawing();
}


void player_input(){
  float speed = player.mv_speed * GetFrameTime();
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
  if(IsKeyPressed(KEY_F3)){
	enable_skybox  = !enable_skybox;
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
  //SetTargetFPS(60);
  project.width = screen.width / screen.proj_scale;
  project.height = screen.height / screen.proj_scale;
  project.half_width = project.width / 2;
  project.half_height = project.height / 2;
  rc_data.increment_angle = player.fov / project.width;
  FrameBuffer = GenImageColor(project.width, project.height, BLANK);
  FrameText = LoadTextureFromImage(FrameBuffer);
  zbuffer = calloc(project.width, sizeof(float));
}

void load_textures(){
  skybox = LoadTexture("nightsky.png");
  enable_skybox = true;
  loaded_textures[0] = load_engine_texture("brick128.png");
  loaded_textures[1] = load_engine_texture("grass128x128.png");
  ceiling_text = load_engine_texture("brick128.png");
  sprites[0].spr_txt = load_engine_texture("tree2.png");
  sprites[0].x = 2.5f;
  sprites[0].y = 1.5f;
  sprites[0].z = -0.1f;
  sprites[0].wrld_height = 1.0f;
  sprites[0].wrld_width = 1.0f;
}


int main()
{
  init_screen();
 
  load_textures();
  while(!WindowShouldClose()){
	render();
	player_input();
#ifdef DEBUG
	printf("player x = %f player y = %f, player.pa = %f\n\r",player.x,player.y,player.pa);
	printf("sprite x = %f sprite y = %f sprite_z = %f\n",sprites[0].x,sprites[0].y,sprites[0].z);
#endif
  }
  CloseWindow();
}

