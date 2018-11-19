#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "core/system.h"
#include "CustomComponents.hpp"
#include <unordered_map>

class RenderSystem : public System{
    private:
        std::unordered_map<std::string, GLuint> loadedVertexBuffers;
        std::unordered_map<std::string, int> loadedVertexBuffersTris;
        std::unordered_map<std::string, GLuint> loadedTexCoordBuffers;
        std::unordered_map<std::string, GLuint> loadedNormalBuffers;
        std::unordered_map<std::string, GLuint> loadedTextures;
        GLuint vao;
        GLuint program;

        std::set<int> renderableEntities;
        std::set<int> cameraEntities;
        
    public:
        RenderSystem(World* w);
        void init() override;
        void update(float deltaTime) override;
        void addEntity(int entityID, componentSignature sig) override;
        void removeEntity(int entityID) override;
        void loadModel(RenderComponent& tc);
};

#endif