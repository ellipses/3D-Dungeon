
//Some Windows Headers (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include "Mesh.h"
#include <stdio.h>
#include <math.h>
#include "text.h"
#include <string>
#include <sstream>
#include <vector> // STL dynamic memory.

using namespace std;

/*----------------------------------------------------------------------------
                   MES, TEXTURES AND SHADERS TO LOAD
  ----------------------------------------------------------------------------*/

#define WOLF_MESH "../Mesh/wolf.obj"
#define BEAR_MESH "../Mesh/bear.obj"
#define LAND_MESH "../Mesh/land.obj"
#define CUBE_MESH "../Mesh/cube.obj"
#define FLOOR_MESH "../Mesh/floor.obj"
#define TREE_MESH "../Mesh/tree.obj"
#define LAMP_MESH "../Mesh/lamp.obj"
#define SCREEN_MESH "../Mesh/screen.obj"
#define SPIKE_MESH "../Mesh/spike.obj"
#define BED_MESH "../Mesh/Bed.obj"
#define THRONE_MESH "../Mesh/throne.obj"
#define BOTTLE_MESH "../Mesh/potion.obj"
#define PILLAR_MESH "../Mesh/pillar.obj"
#define SHURIKEN_MESH "../Mesh/shuriken.obj"
#define L_ARM_SLUG "../Mesh/l_arm_slug.obj"
#define R_ARM_SLUG "../Mesh/r_arm_slug.obj"
#define SLUG "../Mesh/slug.obj"
#define SLUG_F2 "../Mesh/slug_f2.obj"
#define SWORD "../Mesh/sword.obj"
#define AXE  "../Mesh/Axe.obj"

//texture location
//the map
char *map_file = "../Textures/maptest.png";
char *you_died_file = "../Textures/you_died.jpg";
char *win_file = "../Textures/win_screen.png";
//floor
char *floor_file = "../Textures/floor_texture.png";
char *floor_normal = "../Textures/floor_texture_normal.png";
//lantern
char *lamp_file = "../Textures/lamp.bmp";
//spikes
char *spike_file = "../Textures/spike.jpg";
//throne
char *throne_file = "../Textures/throne.tga"; 
char *throne_spec = "../Textures/throne_spec.tga"; 
char *throne_normal = "../Textures/throne_normal.tga"; 

char *potion_file = "../Textures/bottle_health.jpg";
char *potion_normal = "../Textures/bottle_health_normal.jpg";
char *mana_file = "../Textures/bottle_mana.jpg"; 

//spinning pillar
char *pillar_file = "../Textures/pillar_file.jpg";
//spinning blade
char *shuriken_file = "../Textures/shuriken.jpg";
char *shuriken_diff = "../Textures/shuriken_diffuse.jpg";
char *shuriken_spec = "../Textures/shuriken_spec.jpg";
char *shuriken_normal = "../Textures/shuriken_normal.jpg";
//enemy
char *slug_file  =	"../Textures/slug.tga";
char *slughit_file  =	"../Textures/slug_hit.tga";
char *slug_norm_file  =	"../Textures/SLUG_normal.tga";
char *slug_spec_file  =	"../Textures/slug_spec.tga";
//axe
char *axe_file = "../Textures/axe_file.jpg";
char *axe_spec = "../Textures/axe_spec_jpg";
char *axe_normal = "../Textures/axe_normal.jpg";
//walls
char *brick_file = "../Textures/blue_tile.jpg";
char *brick_file_normal = "../Textures/blue_tile_normal.jpg";

//blood splatter
char *red_file = "../Textures/blood.png";

//shader location
char *simple_vertex =  "../Shaders/simpleVertexShader.txt";
char *simple_fragment = "../Shaders/simpleFragmentShader.txt";
char *second_vertex =  "../Shaders/secondVertexShader.txt";
char *second_fragment = "../Shaders/secondFragmentShader.txt";
char *post_vertex = "../Shaders/postVertexShader.txt"; 
char *post_fragment = "../Shaders/postFragmentShader.txt"; 
/*----------------------------------------------------------------------------
  ----------------------------------------------------------------------------*/

// Macro for indexing vertex buffer
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace std;
GLuint shaderProgramID; //default shader with option to enable normal mapping
GLuint shaderProgramID2; //shader for enemies with normal mapping and specular and diffuse mapping
GLuint post_program; //post processing shader

int width = 800;
int height = 600;

GLfloat rotate_y = 0.0f;

//camera variables
bool firstmouse = true; //for handling the suddent jump on first movement
vec3 camerapos = vec3(0.0f,0.0f, -10.0f);
vec3 initialpos; //starting position on the map
vec3 camerafront = vec3(0.0f, 0.0f, 1.0f);
vec3 cameraup = vec3(0.0f, 1.0f, 0.0f);


//Map values
int map[32*32];
int map_height = 32;
int map_width = 32;
float scale_factor = 1; 
double collision_radius = 1.5;
int agro_radius = 65;

//frame rate
static int frame_count = 0;
char title[128];


mat4 view;
mat4 lantern;
float num_of_tex = 0.0;
int tex_num_loc;
int test = 4;

//input stuff
GLfloat lastx = width/2;
GLfloat lasty = height/2; 
GLfloat yaw = 90.0f;  //yaw set to -90 because yaw of 0.0 points to right or something wierd happens in euler angles
GLfloat pitch = 0.0f;
GLfloat mouse_sensitivity  = 0.5f; //increase to increase sensitivity
GLfloat keyboard_sensitivity  = 0.09;
GLfloat rotation_sensitivity = 2.5;
GLfloat camera_object = 0.0f;

//Object declaration
Mesh cube_mesh;
Mesh flat_mesh;
Mesh tree_mesh;
Mesh lamp_mesh;
Mesh game_over_mesh;
Mesh game_win_mesh;
Mesh spike_mesh;
Mesh bed_mesh;
Mesh throne_mesh;
Mesh potion_mesh;
Mesh pillar_mesh;
Mesh shuriken_mesh;
Mesh slug_mesh;
Mesh slug_f2_mesh;
Mesh slug_hit_mesh;
Mesh slug_hit_f2_mesh;
Mesh sword_mesh;
Mesh axe_mesh;
Mesh red_mesh;

//collision with maze wall
bool collision_status = true; //turn collisions on/off for debugging

//key fucntions
bool keys[256] = {false};
bool skeys[256] = {false};

//game values
int life_text;	//key to the life text
int score_text;	//key to score text
int life = 100; //current life
float score = 0; 
int game_status = 1;// 1 if game is played, 0 if lose, 2 if win
int start_row;
int start_col;

//player stuff
vec3 default_pos = vec3(-0.4, -0.25, 0.8);
vec3 weapon_pos = vec3(-0.4, -0.25, 0.8);
vec3 wep_atk_pos = vec3(-0.4, 0.5, 0.8);
bool player_attack_status = false;
int attack_time = 15;   //num of frames of attack animation
int hit = 0;

//enemy things
const int num_of_enemies = 70;	//max number of enemies
int enemy_life[num_of_enemies] = {0};	//life
float enemy_rot[num_of_enemies] = {0}; //to turn the enemy towards the player
float enemy_atk[num_of_enemies] = {0}; //which attack frame to show
bool attack_status[num_of_enemies] = {false}; //if they perfomed an attack
int delay_time = 90; //time in frames between each attack
int attack_delay[num_of_enemies] = {delay_time}; //delay between attacks
vec3 enemy_pos[num_of_enemies] = {vec3(0,0,0)}; //array of enemy positions
vec3 enemy_direction = vec3(0, 0, 1); //direction faced by model on loading, towards positive z
bool enemy_hitstun[num_of_enemies] = {false};


//item things
const int num_of_items = 35; //maximum number of possible items, probably make it dynamic array
bool item_status[num_of_items] = {false};
vec3 item_pos[num_of_items] = {vec3(0,0,0)};


//frame buffer stuff
GLuint frame_tex;
GLuint rb;
GLuint fb;
GLuint quad_vao;

// Shader Functions
#pragma region SHADER_FUNCTIONS

// Create a NULL-terminated string by reading the provided file
char* readShaderSource(const char* shaderFile) {   
    FILE* fp = fopen(shaderFile, "rb"); //!->Why does binary flag "RB" work and not "R"... wierd msvc thing?

    if ( fp == NULL ) { return NULL; }

    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);

    fseek(fp, 0L, SEEK_SET);
    char* buf = new char[size + 1];
    fread(buf, 1, size, fp);
    buf[size] = '\0';

    fclose(fp);

    return buf;
}


static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	// create a shader object
    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(1);
    }
	const char* pShaderSource = readShaderSource( pShaderText);

	// Bind the source code to the shader, this happens before compilation
	glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderSource, NULL);
	// compile the shader and check for errors
    glCompileShader(ShaderObj);
    GLint success;
	// check for shader related errors using glGetShaderiv
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }
	// Attach the compiled shader object to the program object
    glAttachShader(ShaderProgram, ShaderObj);
}

GLuint CompileShaders(char* vertex_shader, char* fragment_shader)
{
	//Start the process of setting up our shaders by creating a program ID
	//Note: we will link all the shaders together into this ID
    shaderProgramID = glCreateProgram();
    if (shaderProgramID == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

	// Create two shader objects, one for the vertex, and one for the fragment shader
	AddShader(shaderProgramID, vertex_shader, GL_VERTEX_SHADER);
	AddShader(shaderProgramID, fragment_shader, GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };
	// After compiling all shader objects and attaching them to the program, we can finally link it
    glLinkProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
    glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
	}

	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
    glValidateProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
    glGetProgramiv(shaderProgramID, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }
	// Finally, use the linked shader program
	// Note: this program will stay in effect for all draw calls until you replace it with another or explicitly disable its use
    glUseProgram(0);
	return shaderProgramID;
}
#pragma endregion SHADER_FUNCTIONS

#pragma region map_functions
mat4 draw_wall(const int& matrix_location, mat4& current){
	
	glUniform1f(tex_num_loc, 2);
	cube_mesh.activate();
	mat4 model = translate(current, vec3(-scale_factor*2, 0, 0)); //translate to the right of current model
	glUniformMatrix4fv (matrix_location, 1, GL_FALSE, model.m);	
	glDrawArrays (GL_TRIANGLES, 0, cube_mesh.g_point_count);	

	glUniform1f(tex_num_loc, 1);
	return model;

}
mat4 draw_floor(const int& matrix_location, mat4& current){

	glUniform1f(tex_num_loc, 2);
	flat_mesh.activate();
	mat4 model = rotate_z_deg(identity_mat4(), 180);  //to draw even with backface culling
	model = translate(model, vec3(-scale_factor*2, scale_factor*2, 0)); //top
	model = current*model;
	glUniformMatrix4fv (matrix_location, 1, GL_FALSE, model.m);	
	glDrawArrays (GL_TRIANGLES, 0, flat_mesh.g_point_count);

	model = translate(current, vec3(-scale_factor*2, 0, 0)); //bottom
	glUniformMatrix4fv (matrix_location, 1, GL_FALSE, model.m);	
	glDrawArrays (GL_TRIANGLES, 0, flat_mesh.g_point_count);

	glUniform1f(tex_num_loc, 1);
	return model;
}
mat4 draw_lantern(const int& matrix_location, mat4& current){

	lamp_mesh.activate();
	mat4 model = translate(current, vec3(-scale_factor*2, 1.69*scale_factor, 0)); //translate to the right of current model and up to place it on roof
	glUniformMatrix4fv (matrix_location, 1, GL_FALSE, model.m);	
	glDrawArrays (GL_TRIANGLES, 0, lamp_mesh.g_point_count);	
	return model;
}
mat4 draw_spike(const int& matrix_location, mat4& current){

	spike_mesh.activate();
	mat4 model = translate(current, vec3(-scale_factor*2, -scale_factor*0.6, 0)); //translate to the right of current model
	glUniformMatrix4fv (matrix_location, 1, GL_FALSE, model.m);	
	glDrawArrays (GL_TRIANGLES, 0, spike_mesh.g_point_count);	
	return model;
}

void draw_map(mat4 origin, mat4 current, int matrix_location)
{
	//draw the map
	mat4 temp = identity_mat4();
	mat4 model = identity_mat4(); 

	int item_index = 0; //used to check if item exits
	int layer_num = 0;

	//make sure that the last model assignment before current assigment doesnt change y value 
	//or the rest of the map with be floating
	
	for(int val = 0; val < (map_height*map_width); val++){
		
		//make adjusment to create new layer
		if((val)%map_width == 0){
	
			layer_num++;
			current = translate(origin, vec3(0, 0, -scale_factor*2*layer_num));
			//cout<<endl;
		}
		//if 0 draw wall, white
		if(map[val] == 0){	

			temp = draw_wall(matrix_location, current);
			current = temp;
		}
		//else if 255 draw floor and ceiling, black
		else if(map[val] == 255){
			
			temp = draw_floor(matrix_location, current);	
			current = temp;
		}	
		//else if 2 draw floor and ceiling, camera appears here, light blue
		else if(map[val] == 2){

			temp = draw_floor(matrix_location, current);	
			current = temp;
		 }	
		//else if 3 draw floor, ceiling and spike trap, green
		else if(map[val] == 3){

			temp = draw_spike(matrix_location, current);
			temp = draw_floor(matrix_location, current);			
			current = temp;
		}
		else if(map[val] == 50){
		//else if 50, draw pillar and blade
			temp = draw_floor(matrix_location, current);	
			
			//draw the pillar			
			pillar_mesh.activate();
			model = rotate_y_deg(identity_mat4(), rotate_y);
			model = temp*model;
			mat4 pillar = model;
			glUniformMatrix4fv (matrix_location, 1, GL_FALSE, model.m);
			glDrawArrays (GL_TRIANGLES, 0, pillar_mesh.g_point_count);

			glUniform1f(tex_num_loc, 4); // 4 texture files
			shuriken_mesh.activate();
			model = translate(identity_mat4(), vec3(0.0, 1, 0.0));
			model = pillar*model;
			glUniformMatrix4fv (matrix_location, 1, GL_FALSE, model.m);
			glDrawArrays (GL_TRIANGLES, 0, shuriken_mesh.g_point_count);
			glUniform1f(tex_num_loc, 1);
			current = temp;			
		}
		else if(map[val] == 200){
		//else if 200 draw ceiling and potion if it exists
			temp = draw_floor(matrix_location, current);	
			
			glUniform1f(tex_num_loc, 2);
			glDisable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			//draw potion if it exists
			if(item_status[item_index]){
				potion_mesh.activate();
				model = translate(current, vec3(-scale_factor*2, 0, 0));
				glUniformMatrix4fv (matrix_location, 1, GL_FALSE, model.m);
				glDrawArrays (GL_TRIANGLES, 0, potion_mesh.g_point_count);
				temp = model;
			}
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glUniform1f(tex_num_loc, 1);
			item_index++;
			current = temp;			
		}
		//else if 20, draw throne, and ceiling, dark blue, win spot
		else if(map[val] == 20){
					
			temp = draw_floor(matrix_location, current);

			//draw throne
			glUniform1f(tex_num_loc, 3);
			throne_mesh.activate();
			model = translate(current, vec3(-scale_factor*2, 0, 0));
			glUniformMatrix4fv (matrix_location, 1, GL_FALSE, model.m);
			glDrawArrays (GL_TRIANGLES, 0, throne_mesh.g_point_count);

			glUniform1f(tex_num_loc, 1);
			current = temp;			
		}
		else if(map[val] == 75){
			//draw the enemy	
			temp = draw_floor(matrix_location, current);
			current = temp;			
		}
	}
}
#pragma endregion map_functions

#pragma region collision_utility
void cause_damage(int damage){
	
	if(collision_status){
		life-=damage;
		hit = 2;
		if(damage == 5){
			//camerapos -= camerafront*keyboard_sensitivity*5;	
		}
	}
}

//finds distance only using the x and z values, 2d distance
double find_distance(vec3 first, vec3 second){
	
	return sqrt(pow((second.v[0]-first.v[0])*(second.v[0]-first.v[0]),2) + pow((second.v[2]-first.v[2])*(second.v[2]-first.v[2]),2));
}

//convert row and col number to middle of cube vertex
int convert_to_center_position(int index){
	//adjust by scalefactor*2, then adjust by offset, add scale factor to bring it to center and finally multiply by -1 because map is negative
	return (((index*(scale_factor * 2))+(2*scale_factor)) + scale_factor)*-1;
}
//convert x and z vertex to row and column number
int convert_to_index(int position, bool h_or_w){
	if(h_or_w)
		return -1*(int)(((float)position/(scale_factor*2)) + 1)%height;
	return -1*(int)(((float)position/(scale_factor*2)) + 1)%width;
}

void apply_item_affect(int item_index){

	item_status[item_index] = false;
	life+=10;

}

bool trap_collision(vec3 future_pos, bool player){

	float x_pos = future_pos.v[0];
	float z_pos = future_pos.v[2];
	//adjust by scale factor*2, add(because map is in minus) 1 because of origin offset, find mod to get pos
	int row = convert_to_index(z_pos, true);
	int col = convert_to_index(x_pos, false); 

	//convert from 2D to 1D  
	int pos = col + row*map_width;

	if(map[pos] == 3 || map[pos] == 50){
		if(player)
			cause_damage(5);
	return true;
	}
	else return false;
}

bool map_collision(vec3 future_pos, bool player){

	float x_pos = future_pos.v[0];
	float z_pos = future_pos.v[2];
	//adjust by scale factor*2, add(because map is in minus) 1 because of origin offset, find mod to get pos
	int row = convert_to_index(z_pos, true);
	int col = convert_to_index(x_pos, false); 
	
	//cout << " Future X position " << x_pos;
	//cout << " Future Z position " << z_pos << endl;
	//cout << " Future Row value " << row;
	//cout << " Future Col Value " << col <<endl <<endl;

	
	if(player){
		for(int i = 0; i < num_of_items;i++){

			if(item_status[i] == true){
				if(find_distance(future_pos, item_pos[i]) < collision_radius){
					apply_item_affect(i);
					break;
				}
			}
		}

		//collision check for enemies
		for(int i = 0; i < num_of_enemies; i++){
		
			if(enemy_life[i] > 0){
				if(find_distance(future_pos, enemy_pos[i]) < collision_radius*0.7){
					//cout << " enemy collision" ;
					return true;
				}
			}
		}
	}

	
	//convert from 2D to 1D  
	int pos = col + row*map_width;
	if(map[pos] == 0){
		return true;
	}
	else if(map[pos] == 20){
		cout << endl << " Yay Won";
		game_status = 2;
		return true;
	}
	else {		
		return false;
	}

}
#pragma endregion collision_utility

#pragma region enemy_functions
void move_enemy(int index){
	
	//get vector between enemy and player
	vec3 ground_cam = vec3(camerapos.v[0], 0.0, camerapos.v[2]);
	vec3 difference = ground_cam - enemy_pos[index];

	vec3 norm_difference = normalise(difference);
	float det = (enemy_direction.v[0]* difference.v[2])-(enemy_direction.v[2]* difference.v[0]);

	enemy_rot[index] = -ONE_RAD_IN_DEG*atan2(det, (dot(enemy_direction, difference) )); //roate to face the player
		
	//increasing the multipler for keyboard sensitity increases distance they can move before checking for collision with wall
	if(!map_collision(enemy_pos[index] + norm_difference * 10 * keyboard_sensitivity, false)&&(!trap_collision(enemy_pos[index] + norm_difference * 10 * keyboard_sensitivity, false))){
		if(!enemy_hitstun[index])
		enemy_pos[index] += norm_difference * keyboard_sensitivity*0.7;
	}
}

void enemy_attack(int index){

	//do damage on the first frame
	if( attack_delay[index] == delay_time)
		cause_damage(10);

	//animation stuff
	if( attack_delay[index] >= delay_time*0.9){
		enemy_atk[index] = 1;
	}
	else{
		enemy_atk[index] = 0;				
	}

	attack_delay[index]--;

	if(attack_delay[index] <= 0)
		attack_delay[index] = delay_time;
}


void draw_enemy(const int& matrix_location){
	
	for(int i=0; i < num_of_enemies; i++){
		
		if(enemy_life[i] >0){

			if(enemy_atk[i] == 0){
				if(enemy_hitstun[i])
					slug_hit_mesh.activate();
				else
					slug_mesh.activate();
				//print(enemy_pos[i]);
				mat4 slug_b_model = scale(identity_mat4(), vec3(0.3, scale_factor/3, scale_factor/3));
				slug_b_model = rotate_y_deg(slug_b_model, enemy_rot[i]);
				slug_b_model = translate(slug_b_model, enemy_pos[i]);
				glUniformMatrix4fv (matrix_location, 1, GL_FALSE, slug_b_model.m);
				glDrawArrays (GL_TRIANGLES, 0, slug_mesh.g_point_count);
			}
			else {
				if(enemy_hitstun[i])
					slug_hit_f2_mesh.activate();
				else
					slug_f2_mesh.activate();
				mat4 slug_b_model = scale(identity_mat4(), vec3(scale_factor/3, scale_factor/3, scale_factor/3));
				slug_b_model = rotate_y_deg(slug_b_model, enemy_rot[i]);
				slug_b_model = translate(slug_b_model, enemy_pos[i]);
				glUniformMatrix4fv (matrix_location, 1, GL_FALSE, slug_b_model.m);
				glDrawArrays (GL_TRIANGLES, 0, slug_f2_mesh.g_point_count);
			}
		}
	  }
}

void enemy_movement(){
	//if enemy exists, check if in agro range and move them
	for(int i = 0; i< num_of_enemies; i++){
	
		if(enemy_life[i] > 0){
			int distance = find_distance(camerapos, enemy_pos[i]);
			if(distance < agro_radius && distance > collision_radius ){
				move_enemy(i);
			}
			else if(distance <= collision_radius*1.5){
				enemy_attack(i);
			}
		}
	}
}
#pragma endregion enemy_functions

#pragma region player_functions
void weapon_position(){
	//default position = vec3(-0.4,-0.25,1.45);
	//if weapon at default position then attack status is false
	if((weapon_pos.v[0] <= default_pos.v[0]) && (weapon_pos.v[1] <=  default_pos.v[1]) && (weapon_pos.v[2] >=  default_pos.v[2])){
		player_attack_status = false;
	}
	//otherwise interpolate the weapons position
	else{
		weapon_pos.v[1] -= (wep_atk_pos.v[1] - default_pos.v[1])/attack_time;
		weapon_pos.v[2] -= (wep_atk_pos.v[2] - default_pos.v[2])/attack_time;
	}
}

void player_attack(){
	//perform attack if player not in middle of attack
	if(player_attack_status == false){
		player_attack_status = true;
		weapon_pos.v[1] = wep_atk_pos.v[1];
		weapon_pos.v[2] = wep_atk_pos.v[2];

		//perform damage
		for(int i = 0; i <num_of_enemies; i++){
			if(enemy_life[i] > 0){
				int distance = find_distance(camerapos, enemy_pos[i]);
				if(distance < collision_radius*5 ){
					//cout << " The distance is " << distance << endl;
					enemy_life[i] -= 40;
					if(enemy_life[i] < 0) //if you killed an enemy increase score by 100 points
						score+=100;
					enemy_hitstun[i] = true;
				}
			}
		}
	}	
}
#pragma endregion player_functions

void display_screens(int matrix_location, int view_mat_location, int light_position, int screen_val){

	//bring the came to the centre -10z to make it easier to display things
	camerapos = vec3(0.0f,0.0f, -10.0f);
	camerafront = vec3(0.0f, 0.0f, 1.0f);
	cameraup = vec3(0.0f, 1.0f, 0.0f);
	view = look_at(camerapos, camerapos + camerafront, cameraup);
	glUniformMatrix4fv (view_mat_location, 1, GL_FALSE, view.m);
	
	//place the game winning text on plane
	if(screen_val == 2)
		game_win_mesh.activate();
	else if(screen_val == 0)
		game_over_mesh.activate();

	mat4 model = rotate_z_deg(identity_mat4(), -90);
	model = translate(model, vec3(-1.0, 0.0, -7.2));		
	glUniformMatrix4fv (matrix_location, 1, GL_FALSE, model.m);
	glDrawArrays (GL_TRIANGLES, 0, game_win_mesh.g_point_count);	
	
	//place light source w
	vec4 light = vec4(camerapos - vec3(0, -100, 100), 1.0);
	glUniform4fv(light_position, 1, light.v);	
	glUniformMatrix4fv (matrix_location, 1, GL_FALSE, model.m);

}

void display(){

	glViewport(0, 0, width, height);
	//bind the second(render to texture) framebuffer
	glBindFramebuffer (GL_FRAMEBUFFER, fb);
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//NORMAL RENDERING STUFF//

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable (GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"

	//glEnable (GL_FRAMEBUFFER_SRGB);

	//seems to increase fps by 4-5 frames? make sure to turn off for potion because of different winding order
	glEnable(GL_CULL_FACE);  //cull back face
	glCullFace(GL_BACK);

	
	glUseProgram (shaderProgramID);

	//Declare your uniform variables that will be used in your shader
	int matrix_location = glGetUniformLocation (shaderProgramID, "model");
	int view_mat_location = glGetUniformLocation (shaderProgramID, "view");
	int proj_mat_location = glGetUniformLocation (shaderProgramID, "proj");
	int light_position = glGetUniformLocation (shaderProgramID, "LightPosition");
	int scale_position = glGetUniformLocation (shaderProgramID, "scale_factor");
	tex_num_loc = glGetUniformLocation (shaderProgramID, "tex_num");

	int first_sampler_location = glGetUniformLocation (shaderProgramID, "basic_texture");
	int second_sampler_location = glGetUniformLocation (shaderProgramID, "spec_texture");
	int third_sampler_location = glGetUniformLocation (shaderProgramID, "normal_texture");
	int fourth_sampler_location = glGetUniformLocation (shaderProgramID, "diffuse_texture");

	glUniform1i (first_sampler_location, 0); // read from active texture 0
	glUniform1i (second_sampler_location, 1); // read from active texture 1
	glUniform1i (third_sampler_location, 2); //read from active texture 2
	glUniform1i (fourth_sampler_location, 3);

	glUniform1f(scale_position, scale_factor);
	glUniform1f(tex_num_loc, 1);
	
	//camera stuff
	GLfloat rpitch =pitch * ONE_DEG_IN_RAD;
	GLfloat ryaw   =yaw  * ONE_DEG_IN_RAD;

	vec3 front; 
	front.v[0] = cos(ryaw) * cos(rpitch);
	front.v[1] = sin(rpitch);
	front.v[2] = sin(ryaw) * cos(rpitch);
	vec3 normalised_direction = normalise(front);
	camerafront.v[0] = normalised_direction.v[0];
	camerafront.v[2] = normalised_direction.v[2];

	view = look_at(camerapos, camerapos + camerafront, cameraup);

	mat4 persp_proj = perspective(45.0, (float)width/(float)height, 0.001, 100.0);
	mat4 model = identity_mat4();
	glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv (view_mat_location, 1, GL_FALSE, view.m);

	//make origin and traslate it so that top left corner is 0,0
	cube_mesh.activate();
	model = scale(identity_mat4(), vec3(scale_factor,scale_factor, scale_factor));
	model = translate(model, vec3(-scale_factor, 0, -scale_factor));
	mat4 origin = model;
	mat4 current = origin;
	//glUniformMatrix4fv (matrix_location, 1, GL_FALSE, model.m);
	//glDrawArrays (GL_TRIANGLES, 0, cube_mesh.g_point_count);

	
	//if won
	if(game_status == 2){

		display_screens(matrix_location, view_mat_location, light_position, game_status);

	}
	//if lose
	else if(game_status == 0){	 		

		display_screens(matrix_location, view_mat_location, light_position, game_status);

	}
	//if the game is being played
	else if(game_status == 1){

		//draw the map
		draw_map(origin, current, matrix_location);	

		//place lantern in front of camera
		//lamp_mesh.activate();
		model = translate(identity_mat4(), vec3(0.6,-0.5,1.5)); //postion it to the bottom left corner
		model = rotate_y_deg(model, camera_object); //roate it
		model = translate(model, (camerapos));	//position it at camera	
			
		glUniformMatrix4fv (matrix_location, 1, GL_FALSE, model.m);
		//glDrawArrays (GL_TRIANGLES, 0, lamp_mesh.g_point_count);
					
		//place light source where lantern is
		vec4 light = vec4(camerapos, 1.0);
		glUniform4fv(light_position, 1, light.v);	
		glUniformMatrix4fv (matrix_location, 1, GL_FALSE, model.m);	
		

		//place axe in front of camera
		glDisable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glUniform1f(tex_num_loc, 3);
		axe_mesh.activate();
		model = rotate_x_deg(identity_mat4(), -45);
		weapon_position();
		model = translate(model, weapon_pos); //postion it to the bottom left corner
		model = rotate_y_deg(model, camera_object); //roate it
		model = translate(model, (camerapos));	//position it at camera	
			
		glUniformMatrix4fv (matrix_location, 1, GL_FALSE, model.m);
		glDrawArrays (GL_TRIANGLES, 0, axe_mesh.g_point_count);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		//using second program for enemy
		glUseProgram (shaderProgramID2);

		//Declare your uniform variables that will be used in your shader
		matrix_location = glGetUniformLocation (shaderProgramID2, "model");
		view_mat_location = glGetUniformLocation (shaderProgramID2, "view");
		proj_mat_location = glGetUniformLocation (shaderProgramID2, "proj");
		light_position = glGetUniformLocation (shaderProgramID2, "LightPosition");
		scale_position = glGetUniformLocation (shaderProgramID2, "scale_factor");
		tex_num_loc = glGetUniformLocation (shaderProgramID2, "tex_num");

		first_sampler_location = glGetUniformLocation (shaderProgramID2, "basic_texture");
		second_sampler_location = glGetUniformLocation (shaderProgramID2, "spec_texture");
		third_sampler_location = glGetUniformLocation (shaderProgramID2, "normal_texture");
		fourth_sampler_location = glGetUniformLocation (shaderProgramID2, "diffuse_texture");

		glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, persp_proj.m);
		glUniformMatrix4fv (view_mat_location, 1, GL_FALSE, view.m);
		glUniform4fv(light_position, 1, light.v);	
		glUniform1f(scale_position, scale_factor);

		glUniform1i (first_sampler_location, 0); // read from active texture 0
		glUniform1i (second_sampler_location, 1); // read from active texture 1
		glUniform1i (third_sampler_location, 2); //read from active texture 2
		glUniform1i (fourth_sampler_location, 3);	

		//move the enemy
		enemy_movement();

		//draw the enemy
		glUniform1f(tex_num_loc, 3);
		draw_enemy(matrix_location);
		glUniform1f(tex_num_loc, 1);
		
	}
	
	//draw the current life
	stringstream life_value;
	life_value << "Life : ";
	life_value << life;
	string curr_life = life_value.str();
	update_text(life_text, curr_life.c_str());
	
	
	stringstream score_value;
	score_value << "Total Score : ";
	score_value << score;
	string curr_score_value = score_value.str();
	update_text(score_text, curr_score_value.c_str());
	
	draw_texts ();

	//END OF NORMAL RENDERING STUFF AND REMAINDER OF FRAMEBUFFER STUFF//
	glBindFramebuffer (GL_FRAMEBUFFER, 0);
	//clear the framebuffers colour and depth buffers 
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	////post processing shader for screen space quad
	glUseProgram (post_program);
	//bind the quads vao 
	glBindVertexArray (quad_vao);
	//active first texture slot and put texture from previous pass in it
	int first_tex_location = glGetUniformLocation (post_program, "tex");
	int second_tex_location = glGetUniformLocation (post_program, "second_tex");
	int hit_status_loc = glGetUniformLocation (post_program, "hit_status");

	glUniform1i (first_tex_location, 0); // read from active texture 0
	glUniform1i (second_tex_location, 1); // read from active texture 1

	glUniform1f (hit_status_loc, hit);

	//enable blending and turn off depth test so blood splash can be displayed
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	//draw the quad	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_2D, frame_tex);
	glDrawArrays (GL_TRIANGLES, 0, 6);

	//draw blood splatter if player was hit
	if( hit > 0){

		glActiveTexture(GL_TEXTURE0);
		glBindTexture (GL_TEXTURE_2D, red_mesh.tex);
		glDrawArrays (GL_TRIANGLES, 0, 6);

	}

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	//flip the drawn framebuffer onto the display
	glutSwapBuffers();
}

#pragma region input functions
void apply_inputs(){

	int wall_offset = 7;
	if(keys['w'] == true){
		//check if collision status is turned on
		if(!collision_status){ 
			//if false adjust the position without collision
			camerapos += (camerafront)*keyboard_sensitivity;
		}
		else{
			//else check for wall collision with offset to prevent clipping then check for collision with traps
			if(!map_collision(camerapos + camerafront*(wall_offset*keyboard_sensitivity), true)){
				if(!trap_collision(camerapos + camerafront*(keyboard_sensitivity), true))
					camerapos += camerafront*keyboard_sensitivity;
			}
		}				
	}
	if(keys['s'] == true || skeys[GLUT_KEY_DOWN] == true){
		if(!collision_status){
	
			camerapos -= camerafront*keyboard_sensitivity;
		}
		else{
			//else check if future position will cause collision
			if(!map_collision(camerapos - camerafront*(wall_offset*keyboard_sensitivity), true)){
				if(!trap_collision(camerapos - camerafront*(keyboard_sensitivity), true))
					camerapos -= camerafront*keyboard_sensitivity;			
			}
		}		
	}
	if(keys['a']==true){
		if(!collision_status){
			//normalise the resulting cross product
			//cross product of front vector and up to find left/right direction
			camerapos -= normalise(cross(camerafront,cameraup))*keyboard_sensitivity;
		}
		//else check if future position will cause collision
		else if(!map_collision(camerapos - normalise(cross(camerafront,cameraup))*(wall_offset*keyboard_sensitivity), true)){
			if(!trap_collision(camerapos - normalise(cross(camerafront,cameraup))*(keyboard_sensitivity), true))
				camerapos -= normalise(cross(camerafront,cameraup))*keyboard_sensitivity;			
		}			
	}
	if(keys['d'] == true){
		if(!collision_status){
	
			//cross product of front vector and up to find left/right direction
			camerapos += normalise(cross(camerafront,cameraup))*keyboard_sensitivity;
	
		}
		//else check if future position will cause collision
		else if(!map_collision(camerapos + normalise(cross(camerafront,cameraup))*(wall_offset*keyboard_sensitivity), true)){
			if(!trap_collision(camerapos + normalise(cross(camerafront,cameraup))*(keyboard_sensitivity), true))
				camerapos += normalise(cross(camerafront,cameraup))*keyboard_sensitivity;			
		}
	}
	if(keys['q'] == true || keys['Q'] == true || skeys [GLUT_KEY_UP] == true){
		player_attack();
	}
	//for testing purposes
	if(keys['p'] == true){
		
		float x_pos = camerapos.v[0];
		float y_pos = camerapos.v[1];
		float z_pos = camerapos.v[2];
		//adjust by scale factor*2, add(because map is in minus) 1 because of origin offset, find mod to get pos
		int row = convert_to_index(z_pos, true);
		int col = convert_to_index(x_pos, false); 

		//cout << " X position " << x_pos;
		//cout << " Y position " << y_pos;
		//cout << " Z position " << z_pos << endl;
		//cout << " Row value " << row;
		//cout << " Col Value " << col <<endl;

		//cout << " The distance between camera and throne " << find_distance(camerapos, item_pos[0]);
		//int pos = col + row*map_width;
		//if(map[pos] == 1)
		//	cout<< " Collision " <<endl;
		//else cout << " No Collision " << endl;	
		//enemy_attack(0);
		//if(test == 4.0)
		//	test = 3.0;
		//else test = 4.0;
		//cause_damage(1);
				 
	}


	if(skeys[GLUT_KEY_LEFT] == true){
		if(!collision_status)
			camerapos -= normalise(cross(camerafront,cameraup))*keyboard_sensitivity;
		
		else{
			yaw -= rotation_sensitivity;
			camera_object += rotation_sensitivity;
		}
	}
	if(skeys[GLUT_KEY_RIGHT] == true){
		if(!collision_status)
			camerapos += normalise(cross(camerafront,cameraup))*keyboard_sensitivity;
		
		else {
			yaw += rotation_sensitivity;
			camera_object -= rotation_sensitivity;
		}
	}	
}

void keypress(unsigned char key, int x, int y) {

	keys[key] = true;
	if(key == 'c'){
		collision_status = !collision_status;	
	}
}

void keypress_up(unsigned char key, int x, int y) {

	keys[key] = false;	
}

void directional_press(int key, int x, int y){	
	skeys[key] = true;
}

void directional_press_up(int key, int x, int y){
	skeys[key] = false;
}

void (mouse)(int x, int y)
{
	if(!collision_status){
		//handling the jump to corner
		 if (firstmouse)
		{
		    lastx = x;
		    lasty = y;
		    firstmouse = false;
		}

		GLfloat diffx = x - lastx;
		GLfloat diffy = lasty - y; //reverse this to invert controls
			
		diffx *= mouse_sensitivity;
		diffy *= mouse_sensitivity;
		 
		pitch += diffy;
		yaw   += diffx;
		//cout<< " yaw " <<yaw <<endl<< " pitch " <<pitch <<endl;
		
		GLfloat rpitch =pitch * ONE_DEG_IN_RAD;
		GLfloat ryaw   =yaw  * ONE_DEG_IN_RAD;
			
		vec3 front; 
		front.v[0] = cos(ryaw) * cos(rpitch);
		front.v[1] = sin(rpitch);
		front.v[2] = sin(ryaw) * cos(rpitch);
		//get direction to look at and normalise it to make it a unit vector
		vec3 normalised_direction = normalise(front);

		camerafront.v[0] = normalised_direction.v[0];
		camerafront.v[2] = normalised_direction.v[2];

		//only change y if collision status is off
		if(!collision_status)
		camerafront.v[1] = normalised_direction.v[1];

		lastx = x;
		lasty = y;
		
	}
}
#pragma endregion input functions

void updateScene() {	
	// Placeholder code, if you want to work with framerate
	// Wait until at least 16ms passed since start of last frame (Effectively caps framerate at ~60fps)
	static DWORD  last_time = 0;
	DWORD  curr_time = timeGetTime();
	
	float  delta = (curr_time - last_time) * 0.001f;
	
	//text update 4 times a second
	if(delta > 0.33){
		last_time = curr_time;
		//double fps = (double)frame_count / delta;
		//char tmp[128];
		//sprintf (title, "@ fps: %.2f", fps); 
		//glutSetWindowTitle(title);
		//frame_count = 0;
		hit--;

		for(int i = 0;i< num_of_enemies; i++){
			enemy_hitstun[i] = false;
		}
	}
	frame_count++;


	
	if( life<0 )
		game_status = 0;
	rotate_y +=5.0;
	apply_inputs();
	// Draw the next frame
	glutPostRedisplay();
}

void load_map(){

	int load_width;
	int load_height;
	int comp;
	unsigned char* image = stbi_load(map_file, &load_width, &load_height, &comp, STBI_rgb);

	if(image == NULL)
		cout<< " Failed to load Map "<<endl;
	else
		cout<<" width of Map " << load_width << " height of Map " << load_height <<endl;
	
	int item_row;
	int item_col;
	int item_counter = 0;

	int enemy_row;
	int enemy_col;
	int enemy_counter = 0;

	int offset = 3; //because of rgb
	
	//load map
	cout << endl << "Loading Map" <<endl;
	for(int val = 0; val<(load_height*load_width); val++){

		//to process item location
		if((int)image[val*3] == 200){
			item_row = (val) / map_height;
			item_col = (val) % map_width;
			item_pos[item_counter] = vec3(convert_to_center_position(item_col), 0.0, convert_to_center_position(item_row));
			item_status[item_counter] = true;
			item_counter++;			
		}

		//to process enemy location
		if((int)image[val*3] == 75){
			enemy_row = (val) / map_height;
			enemy_col = (val) % map_width;
			enemy_pos[enemy_counter] = vec3(convert_to_center_position(enemy_col), 0.0, convert_to_center_position(enemy_row));
			enemy_life[enemy_counter] = 100;
			enemy_counter++;			
		}

		//to find starting location
		if((int)image[val*3] == 2){
			start_row = (val) / map_height;
			start_col = (val) % map_width;		
		}
		map[val] = (int)image[val*3];				
	}

	cout << endl << "item counter " << item_counter;
	print(item_pos[0]);

	//cout << endl <<endl;
	//cout << "Starting row " << start_row << " Starting col" << start_col << endl<<endl;
	//find the row and col of the starting point
	int starting_camera_x = convert_to_center_position(start_col);
	int starting_camera_z = convert_to_center_position(start_row);

	//cout << " Camera starting pos X " << starting_camera_x;
	//cout << " Camera starting pos Z "<< starting_camera_z << endl;

	//adjust camera to position it at starting spot.
	camerapos.v[0] = starting_camera_x;
	camerapos.v[1] = scale_factor;
	camerapos.v[2] = starting_camera_z;
	initialpos = camerapos;
	//start the game
	game_status = true;
}

void bind_framebuffer(){

	glGenFramebuffers(1, &fb);

	//make the texture that will be attached to the fb
	glGenTextures(1, &frame_tex);
	//change texture slot?

	glBindTexture(GL_TEXTURE_2D, frame_tex); 
	glTexImage2D (GL_TEXTURE_2D , 0 , GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindFramebuffer (GL_FRAMEBUFFER, fb);
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame_tex, 0);
	
	glGenRenderbuffers (1, &rb);
	glBindRenderbuffer (GL_RENDERBUFFER, rb); 
	glRenderbufferStorage ( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height );
	
	//attach depth texture to framebuffer
	glFramebufferRenderbuffer ( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rb);

	GLenum draw_bufs[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers (1, draw_bufs);

	GLenum status = glCheckFramebufferStatus (GL_FRAMEBUFFER);
	if(GL_FRAMEBUFFER_COMPLETE != status){
		fprintf (stderr, "ERROR: incomplete framebuffer\n");
	}

	glBindFramebuffer (GL_FRAMEBUFFER, 0);
}

void init_ss_quad () {
	// x,y vertex positions
	GLfloat ss_quad_pos[] = {
		-1.0, -1.0,
		 1.0, -1.0,
		 1.0,  1.0,
		 1.0,  1.0,
		-1.0,  1.0,
		-1.0, -1.0
	};

	// create VBOs and VAO in the usual way
	glGenVertexArrays (1, &quad_vao);
	glBindVertexArray (quad_vao);
	
	GLuint vbo;
	glGenBuffers (1, &vbo);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);
	glBufferData ( GL_ARRAY_BUFFER,	sizeof (ss_quad_pos), ss_quad_pos, GL_STATIC_DRAW);
	glVertexAttribPointer (0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray (0);
}

void init(){

	//post process shader
	post_program = CompileShaders(post_vertex, post_fragment);
	bind_framebuffer();
	init_ss_quad();
	
	//special shader with difference light colour to give better effects on the enemies
	shaderProgramID2 =  CompileShaders(second_vertex, second_fragment);
	slug_mesh.init(shaderProgramID2, SLUG, slug_file, slug_spec_file, slug_norm_file);
	slug_f2_mesh.init(shaderProgramID2, SLUG_F2, slug_file,  slug_spec_file, slug_norm_file);
	slug_hit_mesh.init(shaderProgramID2, SLUG, slughit_file, slug_spec_file, slug_norm_file);
	slug_hit_f2_mesh.init(shaderProgramID2, SLUG, slughit_file, slug_spec_file, slug_norm_file);
	


	//Default shader 
	shaderProgramID = CompileShaders(simple_vertex, simple_fragment);

	cube_mesh.init(shaderProgramID, CUBE_MESH, brick_file, NULL, brick_file_normal);

	flat_mesh.init(shaderProgramID, FLOOR_MESH, floor_file, NULL, floor_normal);

	lamp_mesh.init(shaderProgramID, LAMP_MESH, lamp_file);

	game_over_mesh.init(shaderProgramID, CUBE_MESH, you_died_file);

	game_win_mesh.init(shaderProgramID, CUBE_MESH, win_file);

	spike_mesh.init(shaderProgramID, SPIKE_MESH, spike_file);

	throne_mesh.init(shaderProgramID, THRONE_MESH, throne_file, throne_spec, throne_normal);

	potion_mesh.init(shaderProgramID, BOTTLE_MESH, potion_file, NULL, potion_normal);
	
	pillar_mesh.init(shaderProgramID, PILLAR_MESH, pillar_file);

	axe_mesh.init(shaderProgramID, AXE, axe_file, axe_spec,axe_normal );

	shuriken_mesh.init(shaderProgramID, SHURIKEN_MESH, shuriken_file, shuriken_spec, shuriken_normal, shuriken_diff);

	red_mesh.init(shaderProgramID, FLOOR_MESH, red_file);
	
	//load the map and start the game
	load_map();
}


int main(int argc, char** argv){

	// Set up the window
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(width, height);
    glutCreateWindow(" Hello Game "); 

	cout << "Supports OpenGL Version " << glGetString(GL_VERSION) << endl;

	// Tell glut where the display function is
	glutWarpPointer(width/2, height/2);
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	glutKeyboardFunc(keypress);
	glutKeyboardUpFunc(keypress_up);
	glutSpecialFunc(directional_press);
	glutSpecialUpFunc(directional_press_up);
	glutPassiveMotionFunc(mouse);

	

	 // A call to glewInit() must be done after glut is initialized!
    GLenum res = glewInit();
	// Check for any errors
    if (res != GLEW_OK) {
      fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
      return 1;
    }
	// Set up your objects and shaders
	init();

	init_text_rendering ("freemono.png", "freemono.meta", width, height);
	// x and y are -1 to 1
	// size_px is the maximum glyph size in pixels (try 100.0f)
	// r,g,b,a are red,blue,green,opacity values between 0.0 and 1.0
	// if you want to change the text later you will use the returned integer as a parameter
	life_text = add_text ("Life: 100",-1.0, 0.95, 30.0, 1, 1, 1, 1);
	score_text = add_text ("Total Score: 0", -1.0, 0.85, 30.0, 1, 1, 1, 1);

	// Begin infinite event loop
	glutMainLoop();
    return 0;
}











