#ifndef ROTATESYSTEM_H
#define ROTATESYSTEM_H

#include "core/system.h"
#include "CustomComponents.hpp"

class RotateSystem : public System{
    private:
        
    public:
        RotateSystem(World* w);
        void init() override;
        void update(float deltaTime) override;
};

#endif