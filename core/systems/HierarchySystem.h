#ifndef HIERARCHYSYSTEM_H
#define HIERARCHYSYSTEM_H

#include <core/system.h>

class HierarchySystem : public System {
    public:
        HierarchySystem(World* w);
        void init() override;
        void update(float deltaTime) override;
};

#endif