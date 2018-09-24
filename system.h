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
        virtual void update(double deltaTime); 
        componentSignature getNeededComponents() { return neededComponents; }
        void addEntity(int entityID) { entities.emplace(entityID); }
        void removeEntity(int entityID) { entities.erase(entityID); }
};

class RenderSystem : public System{
    public:
        RenderSystem(World* w);
        void update(double deltaTime) override;
};

#endif