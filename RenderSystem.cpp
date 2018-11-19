#define GLFW_DLL
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "core/glm/glm.hpp"
#include "core/glm/gtc/matrix_transform.hpp"

#include "RenderSystem.h"
#include "CustomComponents.hpp"
#include "core/components.h"
#include "core/world.h"
#include "RenderSystemUtils.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

RenderSystem::RenderSystem(World* w){
    mWorld = w;
    componentSignature objRenderer;
    objRenderer[type_id<TransformComponent>()] = 1;
    objRenderer[type_id<RenderComponent>()] = 1;
    neededComponentSignatures.push_back(objRenderer);
    componentSignature cameras;
    cameras[type_id<TransformComponent>()] = 1;
    cameras[type_id<CameraComponent>()] = 1;
    neededComponentSignatures.push_back(cameras);
}

void RenderSystem::init(){
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glViewport(0, 0, 1280, 720);
    glEnable(GL_DEPTH_TEST);
    glClearColor(.2f, .5f, .3f, 1.0);
    program = loadShaders("cube.vert", "cube.frag");
}

void RenderSystem::addEntity(int entityID, componentSignature sig){
    if (sig == neededComponentSignatures[0]){
        renderableEntities.insert(entityID);
        loadModel(mWorld->getComponent<RenderComponent>(entityID));
    }
    else
        cameraEntities.insert(entityID);        
}

void RenderSystem::removeEntity(int entityID){
    cameraEntities.erase(entityID);
    renderableEntities.erase(entityID);
}

void RenderSystem::update(float deltaTime){
    glm::mat4 view;
    glm::mat4 projection;
    for (int e : cameraEntities){
        CameraComponent cc = mWorld->getComponent<CameraComponent>(e);
        if (cc.isActive){
            TransformComponent tc = mWorld->getComponent<TransformComponent>(e);
            view = glm::translate(glm::mat4(1.0), tc.worldPosition);
            view = glm::scale(view, tc.worldScale);
            view = glm::rotate(view, (float)(glm::radians(tc.worldRotation.x)), glm::vec3(1.0, 0.0, 0.0));
            view = glm::rotate(view, (float)(glm::radians(tc.worldRotation.y)), glm::vec3(0.0, 1.0, 0.0));
            view = glm::rotate(view, (float)(glm::radians(tc.worldRotation.z)), glm::vec3(0.0, 0.0, 1.0));   
            view = glm::inverse(view);
            projection = glm::perspective(glm::radians(cc.fov), (float)1280 / (float)720, 0.1f, 100.0f);
        }
    }

    glUseProgram(program);
    GLuint mvpID = glGetUniformLocation(program, "MVP");
    GLuint invTID = glGetUniformLocation(program, "invTranspose");
    for (int e : renderableEntities){
        RenderComponent& rc = mWorld->getComponent<RenderComponent>(e);
        TransformComponent& tc = mWorld->getComponent<TransformComponent>(e);
        
        glm::mat4 model = glm::translate(glm::mat4(1.0), tc.worldPosition);
        model = glm::scale(model, tc.worldScale);
        model = glm::rotate(model, (float)(glm::radians(tc.worldRotation.x)), glm::vec3(1.0, 0.0, 0.0));
        model = glm::rotate(model, (float)(glm::radians(tc.worldRotation.y)), glm::vec3(0.0, 1.0, 0.0));
        model = glm::rotate(model, (float)(glm::radians(tc.worldRotation.z)), glm::vec3(0.0, 0.0, 1.0));

        glm::mat4 mvp = projection * view * model;
        glm::mat3 invT = glm::transpose(glm::inverse(model));

        glBindTexture(GL_TEXTURE_2D, rc.texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindBuffer(GL_ARRAY_BUFFER, rc.vertex_vbo);  
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,0, (void*)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, rc.texCoord_vbo);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, rc.normal_vbo);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(2);
        glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvp[0][0]);
        glUniformMatrix3fv(invTID, 1, GL_FALSE, &invT[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, rc.numVert);
    }
}

void RenderSystem::loadModel(RenderComponent& rc){
    if (loadedVertexBuffers.find(rc.modelFileName) == loadedVertexBuffers.end()){
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> texCoords;
        std::vector<glm::vec3> normals;
        loadOBJ(rc.modelFileName.c_str(), vertices, texCoords, normals);

        GLuint VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
        loadedVertexBuffers[rc.modelFileName] = VBO;
        loadedVertexBuffersTris[rc.modelFileName] = vertices.size();
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * texCoords.size(), texCoords.data(), GL_STATIC_DRAW);
        loadedTexCoordBuffers[rc.modelFileName] = VBO;
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
        loadedNormalBuffers[rc.modelFileName] = VBO;
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    rc.vertex_vbo = loadedVertexBuffers[rc.modelFileName];
    rc.texCoord_vbo = loadedTexCoordBuffers[rc.modelFileName];
    rc.normal_vbo = loadedNormalBuffers[rc.modelFileName];
    rc.numVert = loadedVertexBuffersTris[rc.modelFileName];

    if (loadedTextures.find(rc.textureName) == loadedTextures.end()){
        GLuint texID;
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);
        int w, h, channels;
        unsigned char *data = stbi_load(rc.textureName.c_str(), &w, &h, &channels, 0);
        if (data){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        loadedTextures[rc.textureName] = texID;
        stbi_image_free(data);
    }
    rc.texture = loadedTextures[rc.textureName];
}