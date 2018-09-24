#ifndef WORLD_H
#define WORLD_H

#include "system.h"
#include "component.h"
#include <unordered_map>
#include <vector>
#include <set>

class World{
    private:
        int currID;
        std::vector<System> mSystems;
        std::unordered_map<int, componentSignature> liveEntities;

    public:
        ComponentManager<TransformComponent> TransformData;
        ComponentManager<RenderComponent> RenderData;
        ComponentManager<MovementComponent> MovementData;

        World();
        int createEntity(componentSignature needed);
        void destroyEntity(int entityID);
        void addComponentToEntity(int entityID, componentID ID);
        void addComponentToEntity(int entityID, componentID ID, void* Component);
        void removeComponentFromEntity(int entityID, componentID);
        std::vector<System> getSystems();
};
#endif