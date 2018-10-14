#ifndef SYSTEM_H
#define SYSTEM_H
 
#include "component.h"
#include <set>
class World;

class System{
    protected:
        World *mWorld;
        componentSignature neededComponents;
        std::set<int> entities;
    public:
        System();
        virtual void init();
        virtual void update(float deltaTime); 
        componentSignature getNeededComponents() { return neededComponents; }
        void addEntity(int entityID) { entities.emplace(entityID); }
        void removeEntity(int entityID) { entities.erase(entityID); }
};

#endif