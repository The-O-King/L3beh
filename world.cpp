#include "world.h"

World::World(){
    currID = 0;
    mSystems.push_back(RenderSystem(this));
    mComponents.registerComponent<TransformComponent>();
    mComponents.registerComponent<RenderComponent>();
    mComponents.registerComponent<MovementComponent>();
}

std::vector<System>& World::getSystems(){
    return mSystems;
}

bool World::baseWorldGen(std::string worldConfigFile){
    //To be implemented
    return true;
}

bool World::customWorldGen(int entityID, std::string currLine){
    //To be implemented
    return true;
}

int World::createEntity(){
    int newID = currID++;
    componentSignature empty;
    liveEntities[newID] = empty;
    return newID;
}

void World::destroyEntity(int entityID){
    componentSignature toDestroy = liveEntities[entityID];

    for (System s : mSystems){
        s.removeEntity(entityID);
    }
    
    mComponents.destroyEntity(entityID, toDestroy);
    liveEntities.erase(entityID);
}