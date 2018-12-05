#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "core/system.h"
#include "CustomComponents.hpp"
#include "core/glm/glm.hpp"

struct collisionInfo{
    glm::vec3 normal;
    float penetrationDist;
};

struct OBB{
    glm::vec3 AxisX, AxisY, AxisZ, Half_size;
};


class CollisionSystem : public System{
    private:
        float separatingDistance(const glm::vec3& RPos, const glm::vec3& Plane, const OBB& box1, const OBB&box2);
    public:
        CollisionSystem(World* w);
        void init() override;
        void update(float deltaTime) override;
};

#endif