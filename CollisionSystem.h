#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "core/system.h"
#include "CustomComponents.hpp"
#include "core/glm/glm.hpp"

struct collisionInfo{
    glm::vec3 normal;
    float penetrationDist;
};

class CollisionSystem : public System{
    private:
        
    public:
        CollisionSystem(World* w);
        void init() override;
        void update(float deltaTime) override;
};

#endif