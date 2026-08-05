

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>
#include <math.h>
const float  PI_DIV_180 = PI / 180.0; 
#define DEGREE_TO_RADIANS(X)(X * PI_DIV_180)
#define DEFAULT_SCREEN_WIDTH 1280
#define DEFAULT_SCREEN_HEIGHT 720
#define DEFAULT_PROJECTION_SCALE 1
#define mapWidth 10
#define mapHeight 10
#define DEFAULT_MOVE_SPEED  2.5
#define DEFAULT_MOUSE_SENS 0.50
#define TEXTURE_WALL 1000
#define MIN_BRIGHTNESS 0.10f
#define MAX_BRIGHTNESS 0.70f
/*
  Raycasting engine template -- this can be modified for whatever use I need.
  I plan on making this a library or something I can plop in and just use maybe.
  I am not sure on how to do that exactly but I will find a way.
*/
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
  Vector3 pos;
  float fov;
  float half_fov;
  float pa;
  float mv_speed;
  float mse_sens;
}player_t;
player_t player = {(Vector3){.x = 1.5,1.5,0.5},90,45,0,DEFAULT_MOVE_SPEED,DEFAULT_MOUSE_SENS};
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

float *brightness_lookup_table;

cell_t worldMap[20][20] =
{
    { {TYP_WALL,1},{TYP_WALL,1},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0} },

    { {TYP_WALL,1},{TYP_FLOOR,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,0},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,0},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_FLOOR,0},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,1},{TYP_FLOOR,0},{TYP_WALL,0} },

    { {TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0},{TYP_WALL,0} }
};


struct{
  cell_t *data;
  size_t height;
  size_t width;
}map_data = {&worldMap[0][0],20,20};

sprite_t sprites[10];

float *zbuffer;

// TODO -- This shit Isn't working some I am just going to make a buffer than just write that to the screen;
Image FrameBuffer;
Texture FrameText;
float clampf(float val,float min,float max){
  if(val < min){
	return min;
  }
  if(val > max){
	return max;
  }
  return val;
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

cell_t *get_map_cell(int x,int y)
{
  return &map_data.data[x * map_data.height + y];
}

void draw_texture(Vector2 wall_pos,int texture_x,double wall_height,texture_t texture,float brightness){
  float y_inc = (wall_height * 2) / texture.height;
  int col = (texture_x * texture.height);
  for(int i = 0;i < texture.height;i++){
	Vector2 line_start = {.x = wall_pos.x, .y = wall_pos.y};
	Vector2 line_end = {.x = wall_pos.x, .y = wall_pos.y += y_inc};
	Color text_color = texture.contents[col + i];
	Color augmented_color = {.r = text_color.r * brightness,.g = text_color.g * brightness, .b = text_color.b * brightness,.a = text_color.a};
	ImageDrawLineV(&FrameBuffer,line_start, line_end,augmented_color);
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
	float dist = project.height / (project.height - 2  * y);
	/* float brightness = 1; */
	float brightness = 1 / dist;
	brightness = clampf(brightness, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
	/* float brightness = brightness_lookup_table[(int)dist]; */
	float tilex = dist * rayCos;
	float tiley = dist * raySin;
	tilex += player.pos.x;
	tiley += player.pos.y;
	int text_x = (int)(tilex * ceiling_text.width) & (ceiling_text.width - 1);
	int text_y = (int)(tiley * ceiling_text.height) & (ceiling_text.height - 1);
	Color text_color = ceiling_text.contents[text_x * ceiling_text.height + text_y];
	Color augmented_color = {.r = text_color.r * brightness,.g = text_color.g * brightness, .b = text_color.b * brightness,.a = text_color.a};
	ImageDrawPixelV(&FrameBuffer,(Vector2){.x = x,.y = y},augmented_color);
	y++;
  }
}

void draw_floor(int x,float wall_height,float rayCos,float raySin){
  float start = project.half_height + wall_height;
  float y = start;
  for(;y < project.height;){
	float dist = project.height / (2 * y - project.height);
	/* float brightness = 1; */
	float brightness  = 1 / dist;
	brightness = clampf(brightness, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
	//float brightness = brightness_lookup_table[(int)dist];
	float tilex = dist * rayCos;
	float tiley = dist * raySin;
	tilex += player.pos.x;
	tiley += player.pos.y;
	cell_t *floor_cell = get_map_cell((int)tilex, (int)tiley);
	texture_t floor_text = loaded_textures[floor_cell->texture_id];
	int text_x = (int)(tilex * floor_text.width) & (floor_text.width - 1);
	int text_y = (int)(tiley * floor_text.height) & (floor_text.height - 1);
	Color text_color = floor_text.contents[text_x * floor_text.height + text_y];
	Color augmented_color = {.r = text_color.r * brightness,.g = text_color.g * brightness, .b = text_color.b * brightness,.a = text_color.a};
	ImageDrawPixelV(&FrameBuffer,(Vector2){.x = x,.y = y},augmented_color);
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
	int mapx = (int)player.pos.x;
	int mapy = (int)player.pos.y;
	int side;
	if(rayCos < 0){
	  step_x = -1;
	  side_dist_x = (player.pos.x - (float)mapx) *delta_dist_x;
	}else{
	  step_x = 1;
	  side_dist_x = ((float)mapx + 1.0 - player.pos.x) * delta_dist_x;
	}
	if(raySin < 0){
	  step_y = -1;
	  side_dist_y = (player.pos.y - (float)mapy) *delta_dist_y;
	}else{
	  step_y = 1;
	  side_dist_y = ((float)mapy + 1.0 - player.pos.y) * delta_dist_y;
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
	  wall = get_map_cell(mapx, mapy)->type;
	}
	cell_t wall_cell = *get_map_cell(mapx, mapy);
	float dist;
	float hitx;
	if(side == 0){
	  dist = (side_dist_x - delta_dist_x);
	  hitx = player.pos.y + (dist * raySin);
	}
	else{
	  dist = (side_dist_y - delta_dist_y);
	  hitx = player.pos.x + (dist * rayCos);
	}
	//printf("dist = %f\n", dist);
	hitx -= floorf(hitx);
	float wall_height = (float)project.half_height / dist;
	Vector2 wall_pos = {.x = ray_cnt, .y = project.half_height - wall_height};
	texture_t texture = loaded_textures[wall_cell.texture_id];
	int texture_pos = ((int)(texture.width * hitx) & (texture.width - 1));
	if(!enable_skybox){
	  draw_ceiling(ray_cnt, wall_height, rayCos, raySin);
	}
	/* float strip_brightness = 1; */
	float strip_brightness = 1.0 / dist;
	/* float strip_brightness = brightness_lookup_table[(int)dist]; */
	strip_brightness = clampf(strip_brightness, MIN_BRIGHTNESS,MAX_BRIGHTNESS);
	draw_texture(wall_pos,texture_pos,wall_height,texture,strip_brightness);
	draw_floor(ray_cnt, wall_height, rayCos, raySin);
	float new_cos = rayCos * cosDelta - raySin * sinDelta;
	raySin = raySin * cosDelta +  rayCos  * sinDelta;
	rayCos = new_cos;
	ray_angle += rc_data.increment_angle;
	zbuffer[ray_cnt] = dist;
  }
  
}

void draw_sprites(void)
{
  float angle = DEGREE_TO_RADIANS(player.pa);  
  float pa_sin = sinf(angle);
  float pa_cos = cosf(angle);
  for(int i = 0; i < 3;i++){
    sprite_t sprite = sprites[i];
    // Sprite position relative to player.
    float dx = sprite.x - player.pos.x;
    float dy = sprite.y - player.pos.y;
    // Transform world position into camera space.
    float depth =  dx * pa_cos + dy * pa_sin;
    float side  = -dx * pa_sin + dy * pa_cos;
	float brightness = 1 / depth;
	//float brightness = brightness_lookup_table[(int)depth];
	brightness = clampf(brightness, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
	/* float brightness = 1; */
	#ifdef DEBUG
	if(i == 1){
	  printf("sprite[1] dx = %f dy = %f depth = %f, side = %f\n",dx,dy,depth,side);
    }
	#endif
	// Behind the camera or too close to project safely.
	if (depth <= 0.001f) {
	  continue;
	}
	/* Outside horizontal FOV. */
    /* if (fabsf(side) > depth) { */
	/*   return; */
    /* } */
    float focal_length = (float)project.half_width;
    float screen_x = (float)project.half_width + (side / depth) * focal_length;
    float sprite_bottom_z = sprite.z;
    float sprite_top_z = sprite.z + sprite.wrld_height;
    float screen_bottom =(float)project.half_height -((sprite_bottom_z - player.pos.z) / depth) * focal_length;
    float screen_top = (float)project.half_height -((sprite_top_z - player.pos.z) / depth) * focal_length;
    float screen_width = (sprite.wrld_width / depth) * focal_length;
    float screen_height = screen_bottom - screen_top;
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
		  Color text_color = sprite.spr_txt.contents[col + texture_y];
		  Color augmented_color = {.r = text_color.r * brightness,.g = text_color.g * brightness, .b = text_color.b * brightness,.a = text_color.a};
		  ImageDrawPixel(&FrameBuffer,strip,y,augmented_color);
		}
	  }
	}   
  }
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
	float new_player_x = 	player.pos.x + paCos;
	float new_player_y = 	player.pos.y + paSin;
	if(!(get_map_cell(new_player_x,new_player_y)->type)){
	  player.pos.x = new_player_x;
	  player.pos.y = new_player_y;
	}
  }
  if(IsKeyDown(KEY_S)){
	float new_player_x = 	player.pos.x - paCos;
	float new_player_y = 	player.pos.y - paSin;
	if(!(get_map_cell(new_player_x,new_player_y)->type)){
	  player.pos.x = new_player_x;
	  player.pos.y = new_player_y;
	}
  }
  if(IsKeyDown(KEY_A)){
	float angle_in_radians = DEGREE_TO_RADIANS(player.pa);
	float strfCos = cosf( angle_in_radians - PI/2) * speed;
	float strfSin = sinf(angle_in_radians - PI/2) * speed;
	float new_player_x = 	player.pos.x + strfCos;
	float new_player_y = 	player.pos.y + strfSin;
	if(!(get_map_cell(new_player_x,new_player_y)->type)){
	  player.pos.x = new_player_x;
	  player.pos.y = new_player_y;
	}
  }
  if(IsKeyDown(KEY_D)){
	float angle_in_radians = DEGREE_TO_RADIANS(player.pa);
	float strfCos = cosf(angle_in_radians + PI/2) * speed;
	float strfSin = sinf(angle_in_radians + PI/2) * speed;
	float new_player_x = 	player.pos.x + strfCos;
	float new_player_y = 	player.pos.y + strfSin;
	if(!(get_map_cell(new_player_x,new_player_y)->type)){
	  player.pos.x = new_player_x;
	  player.pos.y = new_player_y;
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
  InitWindow(screen.width, screen.height, "Raycasting");
  DisableCursor();
  project.width = screen.width / screen.proj_scale;
  project.height = screen.height / screen.proj_scale;
  project.half_width = project.width / 2;
  project.half_height = project.height / 2;
  rc_data.increment_angle = player.fov / project.width;
  FrameBuffer = GenImageColor(project.width, project.height, BLANK);
  FrameText = LoadTextureFromImage(FrameBuffer);
  zbuffer = calloc(project.width, sizeof(float));
}

void setup_brightness_lookup_table()
{
  int longest_dist;
  if(map_data.height > map_data.width){
	longest_dist = map_data.height;
  }else{
	longest_dist = map_data.width;
  }
  brightness_lookup_table = calloc(longest_dist,sizeof(float));
  for(int i = 0;i < longest_dist; i++){
	float brightness = 1.0f / (float)(i);
	brightness = clampf(brightness, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
	brightness_lookup_table[i] = brightness;
  }
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
  sprites[1].x = 13.5f;
  sprites[1].y = 13.5f;
  sprites[1].z = -0.1f;
  sprites[1].wrld_height = 1.0f;
  sprites[1].wrld_width = 1.0f;
  sprites[1].spr_txt = load_engine_texture("folkcat.png");
  sprites[2].x = 14.5f;
  sprites[2].y = 14.5f;
  sprites[2].wrld_height = 1.0f;
  sprites[2].wrld_width = 0.5f;
  sprites[2].spr_txt = load_engine_texture("slenderman.png");
}


int main()
{
  init_screen();
  setup_brightness_lookup_table();
  load_textures();
  while(!WindowShouldClose()){
	render();
	player_input();
#ifdef DEBUG
	printf("player x = %f player y = %f, player.pa = %f\n\r",player.pos.x,player.pos.y,player.pa);
	/* printf("sprite x = %f sprite y = %f sprite_z = %f\n",sprites[0].x,sprites[0].y,sprites[0].z); */
#endif
  }
  CloseWindow();
}

