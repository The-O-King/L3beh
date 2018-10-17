#ifndef WORLD_H
#define WORLD_H

#define GLFW_DLL
#include "system.h"
#include "componentManager.h"
#include "components.h"
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <fstream>
#include <sstream> 
#include <vector>
#include <set>

class World{
    protected:
        int currID;
        GLFWwindow* mWindow;
        std::vector<System*> mSystems;
        ComponentManager mComponents;
        std::unordered_map<int, componentSignature> liveEntities;
        
    public:
        World();
        bool baseWorldGen(std::string worldConfigFile);
        virtual bool customWorldGen(int entityID, std::string command, std::istringstream& data);
        int createEntity();
        void destroyEntity(int entityID);
        std::vector<System*>& getSystems();

        template <class T>
        void addComponentToEntity(int entityID);
        template <class T>
        void addComponentToEntity(int entityID, T Component);
        template <class T>
        void removeComponentFromEntity(int entityID);
        template <class T>
        T& getComponent(int entityID);
        template <class T>
        bool setComponent(int entityID, T comp);
};




template <class T>
void World::addComponentToEntity(int entityID){
    int ID = type_id<T>();
    liveEntities[entityID].flip(ID);

    for (System* s : mSystems){
        if ((s->getNeededComponents() & liveEntities[entityID]) == s->getNeededComponents())
            s->addEntity(entityID);
    }

    mComponents.addComponent<T>(entityID);
}

template <class T>
void World::addComponentToEntity(int entityID, T componentData){
    int ID = type_id<T>();
    liveEntities[entityID].flip(ID);

    for (System* s : mSystems){
        if ((s->getNeededComponents() & liveEntities[entityID]) == s->getNeededComponents())
            s->addEntity(entityID);
    }

    mComponents.addComponent<T>(entityID, componentData);
}

template <class T>
void World::removeComponentFromEntity(int entityID){
    int ID = type_id<T>();
    liveEntities[entityID].reset(ID);

    for (System* s : mSystems){
        if ((s->getNeededComponents() & liveEntities[entityID]) == s->getNeededComponents())
            s->addEntity(entityID);
        else
            s->removeEntity(entityID);
    }

    mComponents.removeComponent<T>(entityID);
}

template <class T>
T& World::getComponent(int entityID){
    return mComponents.getComponent<T>(entityID);
}

template <class T>
bool World::setComponent(int entityID, T comp){
    return mComponents.setComponent<T>(entityID, comp);
}

#endif