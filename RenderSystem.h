#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "core/system.h"
#include "CustomComponents.hpp"
#include <unordered_map>

class RenderSystem : public System{
    private:
        std::unordered_map<std::string, GLuint> loadedShaderPrograms;
        std::unordered_map<std::string, GLuint> loadedVertexBuffers;
        GLuint vao;
        
    public:
        RenderSystem(World* w);
        void init() override;
        void update(double deltaTime) override;
        void loadModel(RenderComponent& tc);
};

#endif