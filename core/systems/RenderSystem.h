#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "../system.h"
#include "../components.h"
#include <unordered_map>

class RenderSystem : public System{
    private:
        std::unordered_map<std::string, GLuint> loadedVertexBuffers;
        std::unordered_map<std::string, int> loadedVertexBuffersTris;
        std::unordered_map<std::string, GLuint> loadedTexCoordBuffers;
        std::unordered_map<std::string, GLuint> loadedNormalBuffers;
        std::unordered_map<std::string, GLuint> loadedTextures;
        std::unordered_map<std::string, GLuint> loadedVAO;
        GLuint program;

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