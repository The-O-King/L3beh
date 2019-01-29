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
        std::vector<int> entitiesToDestroy;
        
    public:
        World();
        bool baseWorldGen(std::string worldConfigFile);
        virtual bool customWorldGen(int entityID, std::string command, std::istringstream& data);
        int createEntity();
        int createEntity(std::string entityConfig);
        int createEntity(std::istream& entityConfig);
        void destroyEntity(int entityID);
        void destroyEntities();
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
    mComponents.addComponent<T>(entityID);

    for (System* s : mSystems){
        std::vector<componentSignature> currSigs = s->getNeededComponents();
        for (componentSignature sig : currSigs){
            if ((sig & liveEntities[entityID]) == sig){
                s->addEntity(entityID, sig);
                break;
            }
        }
    }
}

template <class T>
void World::addComponentToEntity(int entityID, T componentData){
    int ID = type_id<T>();
    liveEntities[entityID].flip(ID);
    mComponents.addComponent<T>(entityID, componentData);

    for (System* s : mSystems){
        std::vector<componentSignature> currSigs = s->getNeededComponents();
        for (componentSignature sig : currSigs){
            if ((sig & liveEntities[entityID]) == sig){
                s->addEntity(entityID, sig);
                break;
            }
        }
    }
}

template <class T>
void World::removeComponentFromEntity(int entityID){
    int ID = type_id<T>();
    liveEntities[entityID].reset(ID);
    mComponents.removeComponent<T>(entityID);

    for (System* s : mSystems){
        std::vector<componentSignature> currSigs = s->getNeededComponents();
        for (componentSignature sig : currSigs){
            if (((sig & liveEntities[entityID]) == sig) && sig.test(ID)){
                s->removeEntity(entityID);
                break;
            }
        }
    }
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