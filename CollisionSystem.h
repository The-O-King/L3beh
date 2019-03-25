#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "core/system.h"
#include <map>

struct collisionInfo;
struct collisionInfoKey;

class CollisionSystem : public System{
    private:
        std::map<collisionInfoKey, collisionInfo> persistentCollisionData;
    public:
        CollisionSystem(World* w);
        void init() override;
        void update(float deltaTime) override;
        void addEntity(int entityID, componentSignature sig) override;
};

#endif