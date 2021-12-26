#ifndef TRANSFORMSYSTEM_H
#define TRANSFORMSYSTEM_H

#include <core/system.h>

class TransformSystem : public System {
    public:
        TransformSystem(World* w);
        void init() override;
        void update(float deltaTime) override;
        void addEntity(int entity, componentSignature sig) override;
};

#endif