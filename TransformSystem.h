#ifndef TRANSFORMSYSTEM_H
#define TRANSFORMSYSTEM_H

#include "core/system.h"
#include "CustomComponents.hpp"

class TransformSystem : public System{
    private:
        
    public:
        TransformSystem(World* w);
        void init() override;
        void update(float deltaTime) override;
};

#endif