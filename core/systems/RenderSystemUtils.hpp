/*
    Gotten header and source from
    https://github.com/opengl-tutorials/ogl/tree/master/common
*/

#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <vector>
#include <glm/glm.hpp>

#define GLEW_STATIC
#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

#define MAX_LIGHTS 100

struct MeshGLData {
    GLuint vertex_buffer;
    GLuint index_buffer;
    GLuint VAO;
    int index_count;
};

bool loadOBJ(
    const char * path, 
    MeshGLData &mesh_data
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