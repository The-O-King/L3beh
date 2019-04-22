#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include "../system.h"

class PhysicsSystem : public System{
    private:
        
    public:
        PhysicsSystem(World* w);
        void init() override;
        void update(float deltaTime) override;
};

#endif