#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "core/system.h"
#include "CustomComponents.hpp"
#include <unordered_map>

class RenderSystem : public System{
    private:
        std::unordered_map<std::string, GLuint> loadedShaderPrograms;
        std::unordered_map<std::string, GLuint> loadedVertexBuffers;
        std::unordered_map<std::string, GLuint> loadedTexCoordBuffers;
        std::unordered_map<std::string, GLuint> loadedNormalBuffers;
        std::unordered_map<std::string, GLuint> loadedTextures;
        GLuint vao;
        
    public:
        RenderSystem(World* w);
        void init() override;
        void update(float deltaTime) override;
        void loadModel(RenderComponent& tc);
};

#endif