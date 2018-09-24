#include "world.h"

World::World(){
    currID = 0;
    mSystems.push_back(RenderSystem(this));
}

std::vector<System> World::getSystems(){
    return mSystems;
}

int World::createEntity(componentSignature needed){
    int newID = currID++;
    liveEntities[newID] = needed;
    return newID;
}

void World::addComponentToEntity(int entityID, componentID ID){
    liveEntities[entityID] = liveEntities[entityID].flip(ID);

    for (System s : mSystems){
        if ((s.getNeededComponents() & liveEntities[entityID]) == s.getNeededComponents())
            s.addEntity(entityID);
    }

    if (ID == TRANSFORM)
        TransformData.addComponent(entityID);
    else if (ID == RENDER)
        RenderData.addComponent(entityID);
    else if (ID == MOVEMENT)
        MovementData.addComponent(entityID);
}

void World::addComponentToEntity(int entityID, componentID ID, void* componentData){
    liveEntities[entityID] = liveEntities[entityID].flip(ID);

    for (System s : mSystems){
        if ((s.getNeededComponents() & liveEntities[entityID]) == s.getNeededComponents())
            s.addEntity(entityID);
    }

    if (ID == TRANSFORM)
        TransformData.addComponent(entityID, *(TransformComponent*)componentData);
    else if (ID == RENDER)
        RenderData.addComponent(entityID, *(RenderComponent*)componentData);
    else if (ID == MOVEMENT)
        MovementData.addComponent(entityID, *(MovementComponent*)componentData);
}

void World::removeComponentFromEntity(int entityID, componentID ID){
    liveEntities[entityID] = liveEntities[entityID].flip(ID);

    for (System s : mSystems){
        if ((s.getNeededComponents() & liveEntities[entityID]) == s.getNeededComponents())
            s.addEntity(entityID);
        else{
            s.removeEntity(entityID);
        }
    }

    if (ID == TRANSFORM)
        TransformData.removeComponent(entityID);
    else if (ID == RENDER)
        RenderData.removeComponent(entityID);
    else if (ID == MOVEMENT)
        MovementData.removeComponent(entityID);
}

void World::destroyEntity(int entityID){
    componentSignature toDestroy = liveEntities[entityID];

    for (System s : mSystems){
        s.removeEntity(entityID);
    }

    if (toDestroy[TRANSFORM])
        TransformData.removeComponent(entityID);
    if (toDestroy[RENDER])
        RenderData.removeComponent(entityID);
    if (toDestroy[MOVEMENT])
        MovementData.removeComponent(entityID);
    
    liveEntities.erase(entityID);
}