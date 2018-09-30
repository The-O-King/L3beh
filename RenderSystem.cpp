#define GLFW_DLL
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "core/glm/glm.hpp"
#include "core/glm/gtc/matrix_transform.hpp"

#include "RenderSystem.h"
#include "CustomComponents.hpp"
#include "core/world.h"
#include "RenderSystemUtils.hpp"

RenderSystem::RenderSystem(World* w){
    mWorld = w;
    neededComponents[type_id<TransformComponent>()] = 1;
    neededComponents[type_id<RenderComponent>()] = 1;
}

void RenderSystem::init(){
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glViewport(0, 0, 640, 480);
    glEnable(GL_DEPTH_TEST);
    glClearColor(.2f, .5f, .3f, 1.0);
}

void RenderSystem::update(double deltaTime){
    for (int e : entities){
        RenderComponent& rc = mWorld->getComponent<RenderComponent>(e);
        TransformComponent& tc = mWorld->getComponent<TransformComponent>(e);
        if (!rc.initialized){
            loadModel(rc);
        }
        
        glm::mat4 model = glm::translate(glm::mat4(1.0), tc.position);
        model = glm::scale(model, tc.scale);
        model = glm::rotate(model, (float)(glm::radians(tc.rotation.x)), glm::vec3(1.0, 0.0, 0.0));
        model = glm::rotate(model, (float)(glm::radians(tc.rotation.y)), glm::vec3(0.0, 1.0, 0.0));
        model = glm::rotate(model, (float)(glm::radians(tc.rotation.z)), glm::vec3(0.0, 0.0, 1.0));

        glm::mat4 view = glm::translate(glm::mat4(1.0), glm::vec3(0,0,-2));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)640 / (float)480, 0.1f, 100.0f);
        glm::mat4 mvp = projection * view * model;

        glBindBuffer(GL_ARRAY_BUFFER, rc.vbo);  
        glUseProgram(rc.program);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        GLuint MatrixID = glGetUniformLocation(rc.program, "MVP");
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, rc.numVert);
    }
}

void RenderSystem::loadModel(RenderComponent& rc){
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
    loadOBJ(rc.modelFileName.c_str(), vertices, texCoords, normals);

    if (loadedVertexBuffers.find(rc.modelFileName) == loadedVertexBuffers.end()){
        GLuint VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
        loadedVertexBuffers[rc.modelFileName] = VBO;
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    rc.vbo = loadedVertexBuffers[rc.modelFileName];
    rc.numVert = vertices.size();

    if (loadedShaderPrograms.find(rc.vertShaderFileName+rc.fragShaderFileName) == loadedShaderPrograms.end()){
        GLuint program = loadShaders(rc.vertShaderFileName.c_str(), rc.fragShaderFileName.c_str());
        loadedShaderPrograms[rc.vertShaderFileName+rc.fragShaderFileName] = program;
    }
    rc.program = loadedShaderPrograms[rc.vertShaderFileName+rc.fragShaderFileName];
    rc.initialized = true;
}