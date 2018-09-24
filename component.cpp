#include "component.h"

template <class T>
bool ComponentManager<T>::addComponent(int entityID){
    if (entityDict.find(entityID) == entityDict.end()){
        componentHolder.push_back(T());
        entityDict[entityID] = componentHolder.size() - 1;
        return true;
    }
    return false;
}

template <class T>
bool ComponentManager<T>::addComponent(int entityID, T initialComp){
    if (entityDict.find(entityID) == entityDict.end()){
        componentHolder.push_back(initialComp);
        entityDict[entityID] = componentHolder.size() - 1;
        return true;
    }
    return false;
}

template <class T>
bool ComponentManager<T>::setComponent(int entityID, T initialComp){
    if (entityDict.find(entityID) != entityDict.end()){
        componentHolder[entityDict[entityID]] = initialComp;
        return true;
    }
    return false;
}

template <class T>
bool ComponentManager<T>::removeComponent(int entityID){
    if (entityDict.find(entityID) != entityDict.end()){
        int entityIndex = entityDict[entityID];
        for(auto& p : entityDict) {
            if (p.second > entityIndex)
                p.second--;
        }
        entityDict.erase(entityID);
        componentHolder.erase(componentHolder.begin() + entityIndex);
        return true;
    }
    return false;
}

template <class T>
T ComponentManager<T>::getComponent(int entityID){
    return componentHolder[entityDict[entityID]];
}