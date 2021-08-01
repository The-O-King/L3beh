#include "componentManager.h"

ComponentManager::ComponentManager(){
    componentHolder.resize(MAX_COMPONENT);
}

bool ComponentManager::destroyEntity(int entityID, componentSignature toDestroy){
    for (int x = 0; x < MAX_COMPONENT; x++){
        if (toDestroy[x]){
            componentHolder[x]->removeComponent(entityID);               
        }
    }
    return true;
}