#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "core/system.h"
#include "core/components.h"
#include "CustomComponents.hpp"
#include "core/glm/glm.hpp"
#include <vector>

struct contactPoint{
    float penetrationDist;
    glm::vec3 point;
};

struct collisionInfo{
    float distBetweenObjects;
    glm::vec3 normal;
    std::vector<contactPoint> points;
};

struct OBB{
    glm::vec3 AxisX, AxisY, AxisZ, Half_size;
};

typedef bool (*collisionFunction) (int, TransformComponent&, int, TransformComponent&, collisionInfo&, World*);
bool BoxVsBox(int c1, TransformComponent &tc1, int c2, TransformComponent &tc2, collisionInfo &res, World* mWorld);
bool BoxVsSphere(int c1, TransformComponent &tc1, int c2, TransformComponent &tc2, collisionInfo &res, World* mWorld);
bool SphereVsBox(int c1, TransformComponent &tc1, int c2, TransformComponent &tc2, collisionInfo &res, World* mWorld);
bool SphereVsSphere(int c1, TransformComponent &tc1, int c2, TransformComponent &tc2, collisionInfo &res, World* mWorld);

class CollisionSystem : public System{
    private:
    
    public:
        CollisionSystem(World* w);
        void init() override;
        void update(float deltaTime) override;
};

#endif