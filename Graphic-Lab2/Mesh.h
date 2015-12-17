#pragma once 
#include <iostream>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "maths_funcs.cpp"

// Assimp includes
#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations


//image loading
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

char* vertex_shader = "../Shaders/simpleVertexShader.txt";
char* fragment_shader = "../Shaders/simpleFragmentShader.txt";

int m_width = 800;
int m_height = 600;



class Mesh{

  public:
	Mesh();
	Mesh(const char*,const char*);

	//shader things
	GLuint CompileShader();
	char* readShaderSource(const char*);
	void AddShader(GLuint, const char*, GLenum);

	//loading and binding vao and tex
	void generateObjectBufferMesh(GLuint& curr_vao, const char* mesh_name);
	bool load_mesh (const char* file_name);
	void bindtexture(GLuint& texture,const char* filename);
	
	//whats called from main
	void activate();
	void init(const GLuint& shaderid, const char* mesh_file, const char* tex_file,const char* spec_file, const char* normal_file, const char* diff_file);

	GLuint programID;
	GLuint vao;
	GLuint tex;
	GLuint spec_tex;
	GLuint norm_tex;
	GLuint diff_tex;
	GLuint loc1, loc2, loc3, loc4;
	std::vector<float> g_vp, g_vn, g_vt, g_vtans;
	int g_point_count;
	int testvalue;
	int tex_type[5];
	bool frame_buff_status;
};


Mesh::Mesh()
{
	//shaderProgramID = Mesh::CompileShader();
	std::cout << "Creating Mesh object" <<std::endl;
	spec_tex = NULL;
	tex = NULL;
	diff_tex = NULL;
	norm_tex = NULL;
	frame_buff_status = false;
	for(int i=0; i<5; i++)
		tex_type[i] = 0;
}
//maybe useless function?
Mesh::Mesh(const char* object_loc, const char* tex_loc){

	//shaderProgramID = CompileShader();
	generateObjectBufferMesh(vao, object_loc);
	bindtexture(tex, tex_loc);
	
}

void Mesh::activate(){

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray (vao);	
	if(tex_type[1]  == 1){
		glActiveTexture (GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
	}
	if(tex_type[2]  == 1){
		glActiveTexture (GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, spec_tex);
	}
	if(tex_type[3]  == 1){
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, norm_tex);
	}
	if(tex_type[4]  == 1){
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, diff_tex);
	}

}

void Mesh::init(const GLuint& shaderid, const char* mesh_file, const char* tex_file = NULL, const char* spec_file = NULL, const char* normal_file = NULL, const char* diff_file = NULL){

	programID = shaderid;
	generateObjectBufferMesh(vao, mesh_file);

	if( tex_file != NULL){
		glGenTextures(1, &tex);
		glActiveTexture(GL_TEXTURE0);
		bindtexture(tex, tex_file);
		tex_type[1] = 1;
	}
	if( spec_file != NULL){
		glGenTextures(1, &spec_tex);
		glActiveTexture(GL_TEXTURE1);
		bindtexture(spec_tex, spec_file);
		tex_type[2] = 1;
	}
	if( normal_file != NULL){
		glGenTextures(1, &norm_tex);
		glActiveTexture(GL_TEXTURE2);
		bindtexture(norm_tex, normal_file);
		tex_type[3] = 1;
	}
	if( diff_file != NULL){
		glGenTextures(1, &diff_tex);
		glActiveTexture(GL_TEXTURE3);
		bindtexture(norm_tex, diff_file);
		tex_type[4] = 1;
	}


}

#pragma region MESH LOADING
/*----------------------------------------------------------------------------
                   MESH LOADING FUNCTION
  ----------------------------------------------------------------------------*/

bool Mesh::load_mesh (const char* file_name) {
  const aiScene* scene = aiImportFile (file_name, aiProcess_Triangulate |  aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace); // TRIANGLES!
       
  if (!scene) {
	//cout<<aiGetErrorString()<<endl;
    fprintf (stderr, "ERROR: reading mesh %s\n", file_name);
	//aiGetErrorString;
    return false;
  }
  printf ("  %i animations\n", scene->mNumAnimations);
  printf ("  %i cameras\n", scene->mNumCameras);
  printf ("  %i lights\n", scene->mNumLights);
  printf ("  %i materials\n", scene->mNumMaterials);
  printf ("  %i meshes\n", scene->mNumMeshes);
  printf ("  %i textures\n", scene->mNumTextures);
  
  
  for (unsigned int m_i = 0; m_i < scene->mNumMeshes; m_i++) {
    const aiMesh* mesh = scene->mMeshes[m_i];
    printf ("    %i vertices in mesh\n", mesh->mNumVertices);
	
	
	g_vp.clear();
	g_vn.clear();
	g_vt.clear();



		g_point_count = mesh->mNumVertices;
    for (unsigned int v_i = 0; v_i < mesh->mNumVertices; v_i++) {
      if (mesh->HasPositions ()) {
        const aiVector3D* vp = &(mesh->mVertices[v_i]);
      //  printf ("      vp %i (%f,%f,%f)\n", v_i, vp->x, vp->y, vp->z);
        g_vp.push_back (vp->x);
        g_vp.push_back (vp->y);
        g_vp.push_back (vp->z);
      }
      if (mesh->HasNormals ()) {
        const aiVector3D* vn = &(mesh->mNormals[v_i]);
       // printf ("      vn %i (%f,%f,%f)\n", v_i, vn->x, vn->y, vn->z);
        g_vn.push_back (vn->x);
        g_vn.push_back (vn->y);
        g_vn.push_back (vn->z);
      }
      if (mesh->HasTextureCoords (0)) {
        const aiVector3D* vt = &(mesh->mTextureCoords[0][v_i]);
        //printf ("      vt %i (%f,%f)\n", v_i, vt->x, vt->y);
        g_vt.push_back (vt->x);
        g_vt.push_back (vt->y);
      }
      if (mesh->HasTangentsAndBitangents ()) {
        // NB: could store/print tangents here
		  const aiVector3D* tangent = &(mesh->mTangents[v_i]);
		  const aiVector3D* bitangent = &(mesh->mBitangents[v_i]);
		  const aiVector3D* normal = &(mesh->mNormals[v_i]);

		  // put the three vectors into vec3 struct format for doing maths
		 vec3 t (tangent->x, tangent->y, tangent->z);
		 vec3 n (normal->x, normal->y, normal->z);
		 vec3 b (bitangent->x, bitangent->y, bitangent->z);
		  // orthogonalise and normalise the tangent so we can use it in something
		 // approximating a T,N,B inverse matrix
		 vec3 t_i = normalise (t - n * dot (n, t));

		 // get determinant of T,B,N 3x3 matrix by dot*cross method
		 float det = (dot (cross (n, t), b));
		 if (det < 0.0f) {
				det = -1.0f;
		 } else {
				det = 1.0f;
		 }
		 g_vtans.push_back(t_i.v[0]);
		 g_vtans.push_back(t_i.v[1]);
		 g_vtans.push_back(t_i.v[2]);
		 g_vtans.push_back(det);

      }
    }
  }
  
  aiReleaseImport (scene);
  return true;
}

#pragma endregion MESH LOADING

/*
#pragma region SHADER_FUNCTIONS
// Create a NULL-terminated string by reading the provided file
char* Mesh::readShaderSource(const char* shaderFile) {   
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


void Mesh::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	// create a shader object
    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
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


GLuint Mesh::CompileShader(){

	//Start the process of setting up our shaders by creating a program ID
	//Note: we will link all the shaders together into this ID
    shaderProgramID = glCreateProgram();
    if (shaderProgramID == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

	// Create two shader objects, one for the vertex, and one for the fragment shader
    AddShader(shaderProgramID, "../Shaders/simpleVertexShader.txt", GL_VERTEX_SHADER);
    AddShader(shaderProgramID, "../Shaders/simpleFragmentShader.txt", GL_FRAGMENT_SHADER);

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
    glUseProgram(shaderProgramID);
	return shaderProgramID;

}
#pragma endregion SHADER_FUNCTIONS

*/


#pragma region VBO_FUNCTIONS

void Mesh::generateObjectBufferMesh(GLuint& curr_vao, const char* mesh_name) {
/*----------------------------------------------------------------------------
                   LOAD MESH HERE AND COPY INTO BUFFERS
  ----------------------------------------------------------------------------*/

	//Note: you may get an error "vector subscript out of range" if you are using this code for a mesh that doesnt have positions and normals
	//Might be an idea to do a check for that before generating and binding the buffer.

	load_mesh (mesh_name);
	
	loc1 = glGetAttribLocation(programID, "vertex_position");
	loc2 = glGetAttribLocation(programID, "vertex_normal");
	loc3 = glGetAttribLocation(programID, "vertex_texture");
	loc4 = glGetAttribLocation(programID, "vtangent");
	//cout<< " loc1 " << loc1 << " loc2 " <<loc2 <<" loc3 " <<loc3 <<endl;

	unsigned int vp_vbo = 0;
	glGenBuffers (1, &vp_vbo);
	glBindBuffer (GL_ARRAY_BUFFER, vp_vbo);
	glBufferData (GL_ARRAY_BUFFER, g_point_count * 3 * sizeof (float), &g_vp[0], GL_STATIC_DRAW);

	unsigned int vn_vbo = 0;
	glGenBuffers (1, &vn_vbo);
	glBindBuffer (GL_ARRAY_BUFFER, vn_vbo);
	glBufferData (GL_ARRAY_BUFFER, g_point_count * 3 * sizeof (float), &g_vn[0], GL_STATIC_DRAW);

	unsigned int vt_vbo = 0;
	if(g_vt.size() >0){
	glGenBuffers (1, &vt_vbo);
	glBindBuffer (GL_ARRAY_BUFFER, vt_vbo);
	glBufferData (GL_ARRAY_BUFFER, g_point_count * 2 * sizeof (float), &g_vt[0], GL_STATIC_DRAW);
	}

	unsigned int tangents_vbo = 0;
	if(g_vtans.size() >0){
	glGenBuffers (1, &tangents_vbo);
	glBindBuffer (GL_ARRAY_BUFFER, tangents_vbo);
	glBufferData (GL_ARRAY_BUFFER, g_point_count * 4 * sizeof (float), &g_vtans[0], GL_STATIC_DRAW);
	}
	
	
	glGenVertexArrays(1,&curr_vao);
	glBindVertexArray (curr_vao);

	glEnableVertexAttribArray (loc1);
	glBindBuffer (GL_ARRAY_BUFFER, vp_vbo);
	glVertexAttribPointer (loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray (loc2);
	glBindBuffer (GL_ARRAY_BUFFER, vn_vbo);
	glVertexAttribPointer (loc2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	if(g_vt.size() >0){
		glEnableVertexAttribArray (loc3);
		glBindBuffer (GL_ARRAY_BUFFER, vt_vbo);
		glVertexAttribPointer (loc3, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	}
	if(g_vtans.size() > 0){
		glEnableVertexAttribArray(loc4);
		glBindBuffer(GL_ARRAY_BUFFER, tangents_vbo);
		glVertexAttribPointer(loc4, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	}
	glBindVertexArray(0);
	

}


void Mesh::bindtexture(GLuint& texture,const char* filename){

	int w;
	int h;
	int comp;
	unsigned char* image = stbi_load(filename, &w, &h, &comp, 4);

	if(image == NULL)
		cout<< "Failed to load image"<<endl;
	else
		cout<<" width of image " <<w << " height of image " << h <<endl;
	

	glBindTexture(GL_TEXTURE_2D,texture); 
	glTexImage2D (GL_TEXTURE_2D , 0 ,GL_RGBA,w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glBindTexture(GL_TEXTURE_2D, 0);
}


#pragma endregion VBO_FUNCTIONS
