#ifndef WORLD_H
#define WORLD_H

#define GLEW_STATIC
#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

#include "system.h"
#include "componentManager.h"
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
        std::vector<ComponentListInterface*> mComponents;
        std::unordered_map<int, componentSignature> liveEntities;
        std::set<int> entitiesToDestroy;
        
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
        int registerComponent();

        template <class T>
        T& addComponent(int entityID);

        template <class T>
        void removeComponent(int entityID);
        
        template <class T>
        T& getComponent(int entityID);
        
        template <class T>
        bool has(int entityID);

        componentSignature getComponentSignature(int entity);
};




template <class T>
T& World::addComponent(int entityID){
    int ID = type_id<T>();
    liveEntities[entityID].flip(ID);
    T* newComponent = std::any_cast<T*>(mComponents[ID]->addComponent(entityID));

    for (System* s : mSystems){
        std::vector<componentSignature> currSigs = s->getNeededComponents();
        for (componentSignature sig : currSigs){
            if ((sig & liveEntities[entityID]) == sig){
                s->addEntity(entityID, liveEntities[entityID]);
                break;
            }
        }
    }

    return *newComponent;
}

template <class T>
void World::removeComponent(int entityID){
    int ID = type_id<T>();
    liveEntities[entityID].reset(ID);
    mComponents[ID]->removeComponent(entityID);

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
    return *std::any_cast<T*>(mComponents[type_id<T>()]->getComponent(entityID));
}

template <class T>
bool World::has(int entityID) {
    return liveEntities[entityID][type_id<T>()];
}

template <class T>
int World::registerComponent() {
    int compID = type_id<T>();
    mComponents[compID] = new ComponentList<T>;
    return compID;
}

#endif