#include "TransformSystem.h"
#include <core/components/TransformComponent.h>
#include <core/components/TransformGlobalComponent.h>
#include <core/components/HierarchyChildrenComponent.h>
#include <core/components/HierarchyParentComponent.h>
#include <core/world.h>
#include <glm/vec3.hpp>
#include <vector>


TransformSystem::TransformSystem(World* w){
    mWorld = w;
    componentSignature main;
    main[type_id<TransformGlobalComponent>()] = 1;
    main[type_id<TransformComponent>()] = 1;
    neededComponentSignatures.push_back(main);
}

void TransformSystem::init(){

}

void TransformSystem::addEntity(int entity, componentSignature sig) {
    if (!sig[type_id<HierarchyParentComponent>()])
        entities.insert(entity);
    else
        entities.erase(entity);
}

void TransformSystem::update(float deltaTime){
    for (auto e : entities) {
        TransformGlobalComponent& parentTGC = mWorld->getComponent<TransformGlobalComponent>(e);
        TransformComponent& parentTC = mWorld->getComponent<TransformComponent>(e);
        parentTGC.position = parentTC.position;
        parentTGC.rotation = parentTC.rotation;
        parentTGC.scale = parentTC.scale;
        
        int currParent = e;
        std::vector<int> toUpdate;
        if (mWorld->has<HierarchyChildrenComponent>(e)) {
            HierarchyChildrenComponent parentHC = mWorld->getComponent<HierarchyChildrenComponent>(e);
            toUpdate.insert(toUpdate.end(), parentHC.children.begin(), parentHC.children.end());
        }

        while (!toUpdate.empty()){
            int currUpdate = toUpdate.back(); toUpdate.pop_back();
            
            if (mWorld->has<HierarchyChildrenComponent>(currUpdate)) {
                HierarchyChildrenComponent& currHC = mWorld->getComponent<HierarchyChildrenComponent>(currUpdate);
                toUpdate.insert(toUpdate.end(), currHC.children.begin(), currHC.children.end());
            }
            
            if (mWorld->getComponent<HierarchyParentComponent>(currUpdate).parent != currParent) {
                int newParent = mWorld->getComponent<HierarchyParentComponent>(currUpdate).parent;
                parentTGC = mWorld->getComponent<TransformGlobalComponent>(newParent);
                parentTC = mWorld->getComponent<TransformComponent>(newParent);
            }

            TransformGlobalComponent& currTGC = mWorld->getComponent<TransformGlobalComponent>(currUpdate);
            TransformComponent& currTC = mWorld->getComponent<TransformComponent>(currUpdate);
            currTGC.position = parentTGC.position + parentTGC.rotation * (parentTGC.scale * currTC.position);
            currTGC.rotation = parentTGC.rotation * currTC.rotation;
            currTGC.scale = parentTGC.scale * currTC.scale;
        }
    }
}