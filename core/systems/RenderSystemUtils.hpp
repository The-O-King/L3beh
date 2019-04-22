/*
    Gotten header and source from
    https://github.com/opengl-tutorials/ogl/tree/master/common
*/

#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <vector>
#include "../glm/glm.hpp"

bool loadOBJ(
	const char * path, 
	std::vector<glm::vec3> & out_vertices, 
	std::vector<glm::vec2> & out_uvs, 
	std::vector<glm::vec3> & out_normals
);

GLuint loadShaders(const char * vertex_file_path,const char * fragment_file_path);


#endif