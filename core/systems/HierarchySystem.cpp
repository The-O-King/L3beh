#include "HierarchySystem.h"
#include <core/components/HierarchyParentComponent.h>
#include <core/components/HierarchyChildrenComponent.h>
#include <core/world.h>
#include <glm/vec3.hpp>
#include <vector>


HierarchySystem::HierarchySystem(World* w){
    mWorld = w;
    componentSignature main;
    main[type_id<HierarchyParentComponent>()] = 1;
    neededComponentSignatures.push_back(main);
}

void HierarchySystem::init() {
    
}

void HierarchySystem::update(float deltaTime){
    for (auto e : entities) {
        HierarchyParentComponent pc = mWorld->getComponent<HierarchyParentComponent>(e);
        int parent = pc.parent;
        if (!mWorld->has<HierarchyChildrenComponent>(parent)){
            mWorld->addComponent<HierarchyChildrenComponent>(parent);
        }
        mWorld->getComponent<HierarchyChildrenComponent>(parent).children.insert(e);
    }
}