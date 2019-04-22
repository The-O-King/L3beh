#ifndef TRANSFORMSYSTEM_H
#define TRANSFORMSYSTEM_H

#include "../system.h"

class TransformSystem : public System{
    private:
        
    public:
        TransformSystem(World* w);
        void init() override;
        void update(float deltaTime) override;
};

#endif