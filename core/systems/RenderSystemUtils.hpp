/*
    Gotten header and source from
    https://github.com/opengl-tutorials/ogl/tree/master/common
*/

#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <vector>
#include "../glm/glm.hpp"

#define GLEW_STATIC
#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

#define MAX_LIGHTS 100

bool loadOBJ(
	const char * path, 
	std::vector<glm::vec3> & out_vertices, 
	std::vector<glm::vec2> & out_uvs, 
	std::vector<glm::vec3> & out_normals
);

GLuint loadShaders(const char * vertex_file_path,const char * fragment_file_path);

struct Shader {
    GLuint program;
    GLuint mvpID;
    GLuint invTID;
    GLuint modelID;
    GLuint materialSpecID;
    GLuint materialDiffID;
    GLuint materialShineID;
    GLuint cameraPosID;

    GLuint numPointLightID;
    std::vector<GLuint> positionLightsPosition;
    std::vector<GLuint> positionLightsColor;
    std::vector<GLuint> positionLightsIntensity;
    GLuint hasDirLightID;
    GLuint directionLightDirectionID;
    GLuint directionLightColorID;

    Shader();
    Shader(std::string vert, std::string frag);
};


#endif