#ifndef COMPONENT_H
#define COMPONENT_H
#include <unordered_map>
#include <vector>
#include <bitset>
#include "component_utils.h"

class ComponentManager{
    private:
        std::vector<std::unordered_map<int, int>> entityDict;
        std::vector<void*> componentHolder;
        std::vector<invoker<int>> componentEraser;

    public:
        bool destroyEntity(int entityID, componentSignature toDestroy);

        template <class T>
        int registerComponent();
        template <class T>
        bool addComponent(int entity);
        template <class T>
        bool addComponent(int entity, T initialComp);
        template <class T>
        bool setComponent(int entityID, T initialComp);
        template <class T>
        bool removeComponent(int entity);
        template <class T>
        T& getComponent(int entity);
};

template <class T>
bool ComponentManager::addComponent(int entityID){
    int compID = type_id<T>();
    std::unordered_map<int, int>& compEntityID = entityDict[compID];
    std::vector<T>* allComponents = (std::vector<T>*)componentHolder[compID];

    if (compEntityID.find(entityID) == compEntityID.end()){
        (*allComponents).push_back(T());
        compEntityID[entityID] = (*allComponents).size() - 1;
        return true;
    }
    return false;
}

template <class T>
bool ComponentManager::addComponent(int entityID, T initialComp){
    int compID = type_id<T>();
    std::unordered_map<int, int>& compEntityID = entityDict[compID];
    std::vector<T>* allComponents = (std::vector<T>*)componentHolder[compID];

    if (compEntityID.find(entityID) == compEntityID.end()){
        (*allComponents).push_back(initialComp);
        compEntityID[entityID] = (*allComponents).size() - 1;
        return true;
    }
    return false;
}

template <class T>
bool ComponentManager::setComponent(int entityID, T initialComp){
    int compID = type_id<T>();
    std::unordered_map<int, int>& compEntityID = entityDict[compID];
    std::vector<T>* allComponents = (std::vector<T>*)componentHolder[compID];

    if (compEntityID.find(entityID) != compEntityID.end()){
        (*allComponents)[compID][entityID] = initialComp;
        return true;
    }
    return false;
}

template <class T>
bool ComponentManager::removeComponent(int entityID){
    int compID = type_id<T>();
    std::unordered_map<int, int>& compEntityID = entityDict[compID];
    std::vector<T>* allComponents = (std::vector<T>*)componentHolder[compID];

    if (compEntityID.find(entityID) != compEntityID.end()){
        int entityIndex = compEntityID[entityID];
        for(auto& p : compEntityID) {
            if (p.second > entityIndex)
                p.second--;
        }
        compEntityID.erase(entityID);
        (*allComponents).erase((*allComponents).begin() + entityIndex);
        return true;
    }
    return false;
}

template <class T>
T& ComponentManager::getComponent(int entityID){
    int compID = type_id<T>();
    std::vector<T>* allComponents = (std::vector<T>*)componentHolder[compID];
    return (*allComponents)[entityDict[compID].at(entityID)];
}

template <class T>
int ComponentManager::registerComponent(){
    componentEraser.push_back(erase_at_index<T>());
    componentHolder.push_back(new std::vector<T>);
    entityDict.push_back(std::unordered_map<int, int>());
    return type_id<T>();
}

#endif