#ifndef PLAYERMOVEMENTSYSTEM_H
#define PLAYERMOVEMENTSYSTEM_H

#include "core/system.h"
#include "CustomComponents.hpp"
#include "core/glm/glm.hpp"
#include "core/components.h"
#include "core/world.h"
#include "core/input.h"

class PlayerMovementSystem : public System{
    private:
        
    public:
        PlayerMovementSystem(World* w);
        void init() override;
        void update(float deltaTime) override;
};

#endif