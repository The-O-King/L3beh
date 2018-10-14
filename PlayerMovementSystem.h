#ifndef PLAYERMOVEMENTSYSTEM_H
#define PLAYERMOVEMENTSYSTEM_H

#include "core/system.h"
#include "CustomComponents.hpp"

class PlayerMovementSystem : public System{
    private:
        
    public:
        PlayerMovementSystem(World* w);
        void init() override;
        void update(float deltaTime) override;
};

#endif