#ifndef SYSTEM_H
#define SYSTEM_H
 
#include "componentManager.h"
#include <vector>
#include <set>
class World;

class System{
    protected:
        World *mWorld;
        // Treat this as saying "this system requires entities to have AT LEAST 
        // one of these sets of components to be considered part of this system"
        std::vector<componentSignature> neededComponentSignatures;
        std::set<int> entities;
    
    public:
        System(){}
        virtual void init() = 0;
        virtual void update(float deltaTime) = 0; 
        std::vector<componentSignature> getNeededComponents() { return neededComponentSignatures; }
        virtual void addEntity(int entityID, componentSignature sig) { entities.emplace(entityID); }
        virtual void removeEntity(int entityID) { entities.erase(entityID); }
};

#endif