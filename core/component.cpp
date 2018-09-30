#include "component.h"

bool ComponentManager::destroyEntity(int entityID, componentSignature toDestroy){
    for (int x = 0; x < MAX_COMPONENT; x++){
        if (toDestroy[x]){
            std::unordered_map<int, int>& compEntityID = entityDict[x];
            if (compEntityID.find(entityID) != compEntityID.end()){
                int entityIndex = compEntityID[entityID];
                for(auto& p : compEntityID) {
                    if (p.second > entityIndex)
                        p.second--;
                }
                compEntityID.erase(entityID);
                componentEraser[x](componentHolder[x], entityIndex);             
            }               
        }
    }
    return true;
}