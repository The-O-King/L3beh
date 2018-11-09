#ifndef SYSTEM_H
#define SYSTEM_H
 
#include "component_utils.h"
#include <vector>
#include <set>
class World;

class System{
    protected:
        World *mWorld;
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