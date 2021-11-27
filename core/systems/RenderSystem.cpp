#include "RenderSystem.h"
#include <core/components/TransformComponent.h>
#include <core/components/RenderComponent.h>
#include <core/components/CameraComponent.h>
#include <core/components/LightComponents.h>
#include <core/world.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

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

    componentSignature pointLightEntities;
    pointLightEntities[type_id<PointLightComponent>()] = 1;
    pointLightEntities[type_id<TransformComponent>()] = 1;
    neededComponentSignatures.push_back(pointLightEntities);


    componentSignature dirLightEntities;
    dirLightEntities[type_id<DirectionalLightComponent>()] = 1;
    neededComponentSignatures.push_back(dirLightEntities);
}

void RenderSystem::init(){
    glViewport(0, 0, 1280, 720);
    glEnable(GL_DEPTH_TEST);
    glClearColor(.2f, .5f, .3f, 1.0);
    program = Shader("graphics/cube.vert", "graphics/cube.frag");
}

void RenderSystem::addEntity(int entityID, componentSignature sig){
    if (sig == neededComponentSignatures[0]){
        renderableEntities.insert(entityID);
        loadModel(mWorld->getComponent<RenderComponent>(entityID));
    }
    else if (sig == neededComponentSignatures[1])
        cameraEntities.insert(entityID);
    else if (sig == neededComponentSignatures[2])
        pointLightEntities.insert(entityID);
    else if (sig == neededComponentSignatures[3])
        dirLightEntity = entityID;
}

void RenderSystem::removeEntity(int entityID){
    cameraEntities.erase(entityID);
    renderableEntities.erase(entityID);
    pointLightEntities.erase(entityID);
    if (entityID == dirLightEntity)
        dirLightEntity = 0;

}

void RenderSystem::update(float deltaTime){
    glm::vec3 cameraPos;
    glm::mat4 view;
    glm::mat4 projection;
    for (int e : cameraEntities){
        CameraComponent cc = mWorld->getComponent<CameraComponent>(e);
        if (cc.isActive){
            TransformComponent tc = mWorld->getComponent<TransformComponent>(e);
            cameraPos = tc.getWorldPosition(mWorld);  
            view = glm::inverse(tc.getWorldTransformation(mWorld));
            projection = glm::perspective(glm::radians(cc.fov), (float)1280 / (float)720, 0.1f, 200.0f);
        }
    }

    glUseProgram(program.program);
    glUniform3f(program.cameraPosID, cameraPos.x, cameraPos.y, cameraPos.z);
    int count = 0;
    for (int e : pointLightEntities) {
        glm::vec3 pos = mWorld->getComponent<TransformComponent>(e).getWorldPosition(mWorld);
        PointLightComponent &pc = mWorld->getComponent<PointLightComponent>(e);
        glUniform3f(program.positionLightsPosition[count], pos.x, pos.y, pos.z);
        glUniform3f(program.positionLightsColor[count], pc.color.r, pc.color.g, pc.color.b);
        glUniform1f(program.positionLightsIntensity[count], pc.intensity);
        count++;
    }
    glUniform1i(program.numPointLightID, count);

    glUniform1i(program.hasDirLightID, dirLightEntity);
    if (dirLightEntity != 0){
        DirectionalLightComponent &dc = mWorld->getComponent<DirectionalLightComponent>(dirLightEntity);
        glUniform3f(program.directionLightDirectionID, dc.direction.x, dc.direction.y, dc.direction.z);
        glUniform3f(program.directionLightColorID, dc.color.r, dc.color.g, dc.color.b);
    }

    for (int e : renderableEntities){
        RenderComponent& rc = mWorld->getComponent<RenderComponent>(e);
        TransformComponent& tc = mWorld->getComponent<TransformComponent>(e);
        
        glm::mat4 model = tc.getWorldTransformation(mWorld);

        glm::mat4 mvp = projection * view * model;
        glm::mat3 invT = glm::transpose(glm::inverse(model));

        glBindTexture(GL_TEXTURE_2D, loadedTextures[rc.textureName]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindVertexArray(loadedVertexBuffers[rc.modelFileName].VAO);
        glUniformMatrix4fv(program.mvpID, 1, GL_FALSE, &mvp[0][0]);
        glUniformMatrix4fv(program.modelID, 1, GL_FALSE, &model[0][0]);
        glUniformMatrix3fv(program.invTID, 1, GL_FALSE, &invT[0][0]);
        glUniform3f(program.materialSpecID, rc.specular.r, rc.specular.g, rc.specular.b);
        glUniform3f(program.materialDiffID, rc.diffuse.r, rc.diffuse.g, rc.diffuse.b);
        glUniform1f(program.materialShineID, rc.shininess);
        glDrawElements(GL_TRIANGLES, loadedVertexBuffers[rc.modelFileName].index_count, GL_UNSIGNED_INT, 0);
    }
}

void RenderSystem::loadModel(RenderComponent& rc){
    if (loadedVertexBuffers.find(rc.modelFileName) == loadedVertexBuffers.end()){
        loadedVertexBuffers[rc.modelFileName] = MeshGLData();
        loadOBJ(("graphics/" + rc.modelFileName).c_str(), loadedVertexBuffers[rc.modelFileName]);
    }

    if (loadedTextures.find(rc.textureName) == loadedTextures.end()){
        GLuint texID;
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);
        int w, h, channels;
        unsigned char *data = stbi_load(("graphics/" + rc.textureName).c_str(), &w, &h, &channels, 0);
        if (data){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        loadedTextures[rc.textureName] = texID;
        stbi_image_free(data);
    }
}