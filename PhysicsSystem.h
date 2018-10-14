#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include "core/system.h"
#include "CustomComponents.hpp"

class PhysicsSystem : public System{
    private:
        
    public:
        PhysicsSystem(World* w);
        void init() override;
        void update(float deltaTime) override;
};

#endif