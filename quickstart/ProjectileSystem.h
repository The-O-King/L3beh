#ifndef PROJECTILESYSTEM_H
#define PROJECTILESYSTEM_H

#include "core/system.h"
#include "CustomComponents.hpp"

class ProjectileSystem : public System{
    private:
        
    public:
        ProjectileSystem(World* w);
        void init() override;
        void update(float deltaTime) override;
};

#endif