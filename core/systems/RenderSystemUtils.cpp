#include <cstring>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
using namespace std;

#include <stdlib.h>

#include <glm/glm.hpp>

#include "RenderSystemUtils.hpp"
#include "mikktspace_helpers.h"
#include <mikktspace/weldmesh.h>

// Very, VERY simple OBJ loader.
// Here is a short list of features a real function would provide : 
// - Binary files. Reading a model should be just a few memcpy's away, not parsing a file at runtime. In short : OBJ is not very great.
// - Animations & bones (includes bones weights)
// - Multiple UVs
// - All attributes should be optional, not "forced"
// - More stable. Change a line in the OBJ file and it crashes.
// - More secure. Change another line and you can inject code.
// - Loading from memory, stream, etc

bool loadOBJ(
	const char * path, 
	MeshGLData &mesh_data

){
	printf("Loading OBJ file %s...\n", path);

	std::vector<glm::vec3> temp_vertices; 
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;
	std::vector<float> vertex_buffer;


	FILE * file = fopen(path, "r");
	if( file == NULL ){
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while( 1 ){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		
		if ( strcmp( lineHeader, "v" ) == 0 ){
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			temp_vertices.push_back(vertex);
		}else if ( strcmp( lineHeader, "vt" ) == 0 ){
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y );
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}else if ( strcmp( lineHeader, "vn" ) == 0 ){
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
			if (matches != 9){
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				fclose(file);
				return false;
			}

			for (int x = 0; x < 3; x++) {
				vertex_buffer.push_back(temp_vertices[vertexIndex[x] - 1].x);
				vertex_buffer.push_back(temp_vertices[vertexIndex[x] - 1].y);
				vertex_buffer.push_back(temp_vertices[vertexIndex[x] - 1].z);
				vertex_buffer.push_back(temp_uvs[uvIndex[x] - 1].x);
				vertex_buffer.push_back(temp_uvs[uvIndex[x] - 1].y);
				vertex_buffer.push_back(temp_normals[normalIndex[x] - 1].x);
				vertex_buffer.push_back(temp_normals[normalIndex[x] - 1].y);
				vertex_buffer.push_back(temp_normals[normalIndex[x] - 1].z);
				vertex_buffer.push_back(0); //tangent placeholder
				vertex_buffer.push_back(0); //tangent placeholder
				vertex_buffer.push_back(0); //tangent placeholder
				vertex_buffer.push_back(0); //tangent placeholder
			}
		}else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}

	int raw_vertex_count = vertex_buffer.size() / 12;

	SMikkTSpaceContext context;
	MyMikkTSpaceInferface interface;
	context.m_pInterface = &interface;
	context.m_pUserData = &vertex_buffer;
	genTangSpaceDefault(&context);

	std::vector<int> index_buffer(raw_vertex_count);
	std::vector<float> vertex_buffer_indexed (vertex_buffer.size());
	int unique_vertex_count = WeldMesh(index_buffer.data(), vertex_buffer_indexed.data(), vertex_buffer.data(), raw_vertex_count, 12);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	mesh_data.VAO = vao;
	mesh_data.index_count = raw_vertex_count;
	glBindVertexArray(vao);
	
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * unique_vertex_count * 12, vertex_buffer_indexed.data(), GL_STATIC_DRAW);
	mesh_data.vertex_buffer = VBO;
	glEnableVertexAttribArray(0); //position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 12, 0);
	glEnableVertexAttribArray(1); //texture
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 12, (void *)(sizeof(float) * 3));
	glEnableVertexAttribArray(2); //normal
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 12, (void *) (sizeof(float) * 5));
	glEnableVertexAttribArray(3); //tangent
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 12, (void *)(sizeof(float) * 8));

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * raw_vertex_count, index_buffer.data(), GL_STATIC_DRAW);
	mesh_data.index_buffer = VBO;

	return true;
}

GLuint loadShaders(const char * vertex_file_path,const char * fragment_file_path){

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	
	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

Shader::Shader() { };

Shader::Shader(std::string vert, std::string frag) {
	program = loadShaders(vert.c_str(), frag.c_str());
	mvpID = glGetUniformLocation(program, "MVP");
	invTID = glGetUniformLocation(program, "invTranspose");
	modelID = glGetUniformLocation(program, "model");
	materialSpecID = glGetUniformLocation(program, "mat.specular");
	materialDiffID = glGetUniformLocation(program, "mat.diffuse");
	materialShineID = glGetUniformLocation(program, "mat.shininess");
	cameraPosID = glGetUniformLocation(program, "cameraPos");
	numPointLightID = glGetUniformLocation(program, "numPointLight");
	directionLightDirectionID = glGetUniformLocation(program, "directionLight.direction");
	directionLightColorID = glGetUniformLocation(program, "directionLight.color");
	hasDirLightID = glGetUniformLocation(program, "hasDirLight");
	
	positionLightsPosition.reserve(MAX_LIGHTS);
	positionLightsColor.reserve(MAX_LIGHTS);
	positionLightsIntensity.reserve(MAX_LIGHTS);
	for (int x = 0; x < MAX_LIGHTS; x++){
		positionLightsPosition.emplace_back(glGetUniformLocation(program, ("pointLights[" + to_string(x) + "].position").c_str()));
		positionLightsColor.emplace_back(glGetUniformLocation(program, ("pointLights[" + to_string(x) + "].color").c_str()));
		positionLightsIntensity.emplace_back(glGetUniformLocation(program, ("pointLights[" + to_string(x) + "].intensity").c_str()));
	}
}