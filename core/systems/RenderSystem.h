#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <core/system.h>
#include <core/components/RenderComponent.h>
#include "RenderSystemUtils.hpp"
#include <unordered_map>

class RenderSystem : public System{
    private:
        std::unordered_map<std::string, MeshGLData> loadedVertexBuffers;
        std::unordered_map<std::string, GLuint> loadedTextures;
        Shader program;

        std::set<int> renderableEntities;
        std::set<int> pointLightEntities;
        std::set<int> cameraEntities;
        int dirLightEntity = 0;
        
    public:
        RenderSystem(World* w);
        void init() override;
        void update(float deltaTime) override;
        void addEntity(int entityID, componentSignature sig) override;
        void removeEntity(int entityID) override;
        void loadModel(RenderComponent& tc);
};

#endif