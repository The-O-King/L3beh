#include "world.h"
#include <iostream>
#include <core/components/HierarchyParentComponent.h>
#include <core/components/HierarchyChildrenComponent.h>
#include <core/components/TransformComponent.h>
#include <core/components/TransformGlobalComponent.h>
#include <core/components/PhysicsComponent.h>
#include <core/components/RenderComponent.h>
#include <core/components/LightComponents.h>
#include <core/components/CameraComponent.h>

std::string& debugString (const char* s)
{
    return *(new std::string(s));
}

World::World(){
    currID = 0;
    mComponents.resize(MAX_COMPONENT);
}

std::vector<System*>& World::getSystems(){
    return mSystems;
}

void World::registerSystem(System* newSystem) {
    mSystems.emplace_back(newSystem);
}

void World::createComponentsFromJSON(int entityID, const nlohmann::json& entity) {
    auto components = entity["components"];
    for (auto& component : components.items()) {
        int compID = mComponentNameToTypeID[component.key()];
        addComponent(entityID, compID, component.value());
    }
}

int World::createEntityPrefab(const std::string& fileName) {
    std::ifstream inputFile;
    inputFile.open(fileName, std::ifstream::in);
    if (!inputFile.is_open())
        return -1;

    nlohmann::json j;
    inputFile >> j;

    int id = createEntity();
    createComponentsFromJSON(id, j);
    return id;
}

bool World::loadWorld(const std::string& fileName) {
    std::ifstream inputFile;
    inputFile.open(fileName, std::ifstream::in);
    if (!inputFile.is_open())
        return false;

    nlohmann::json j;
    inputFile >> j;

    for (int w = 0; w < j.size(); w++) {
        auto world = j[w];
        auto entities = world["entities"];
        for (int e = 0; e < entities.size(); e++) {
            auto entity = entities[e];
            int id = createEntity((int)entity["id"]);
            createComponentsFromJSON(id, entity);
        }
    }

    return true;
}

bool World::saveWorld(const std::string& fileName) const {
    std::ofstream outputFile(fileName);
    if (!outputFile.is_open())
        return false;

    nlohmann::json j = nlohmann::json::array();    
    auto& world = j.emplace_back(nlohmann::json::object());
    auto& entities = world.emplace("entities", nlohmann::json::array()).first.value();
    for (auto e : liveEntities) {
        int id = e.first;
        componentSignature sig = e.second;
        auto& currEntity = entities.emplace_back(nlohmann::json::object());
        currEntity.emplace("id", id);
        auto& components = currEntity.emplace("components", nlohmann::json::object()).first.value();
        for (int componentID = 0; componentID < mComponentCount; componentID++) {
            if (sig[componentID]) {
                Component* currComp = mComponents[componentID]->getComponentAsComponent(id);
                currComp->serialize(components.emplace(mComponents[componentID]->getName(), nlohmann::json::object()).first.value());
            }
        }
    }
    auto& componentIDs = world.emplace("component_id", nlohmann::json::object()).first.value();
    for (auto component : mComponentNameToTypeID) {
        componentIDs.emplace(component.first, component.second);
    }

    outputFile << j;
    outputFile.close();
    return true;
}

int World::createEntity(){
    int newID = currID++;
    componentSignature empty;
    liveEntities[newID] = empty;
    return newID;
}

int World::createEntity(int ID){
    if (liveEntities.find(ID) != liveEntities.end())
        throw std::runtime_error("Requested Entity ID " + std::to_string(ID) + " already exists");
    
    componentSignature empty;
    liveEntities[ID] = empty;
    currID = std::max(currID, ID + 1);
    return ID;
}

void World::destroyEntities(){
    for (int entityID : entitiesToDestroy){
        std::vector<int> destroyList;
        destroyList.push_back(entityID);
        if (has<HierarchyParentComponent>(entityID)) {
            int parent = getComponent<HierarchyParentComponent>(entityID).parent;
            HierarchyChildrenComponent& parentHC = getComponent<HierarchyChildrenComponent>(parent);
            parentHC.children.erase(entityID);
        }

        while (!destroyList.empty()){
            int currDestroy = destroyList.back(); destroyList.pop_back();
            if (has<HierarchyChildrenComponent>(entityID)){
                std::set<int> toAdd = getComponent<HierarchyChildrenComponent>(currDestroy).children;
                destroyList.insert(destroyList.end(), toAdd.begin(), toAdd.end());
            }

            componentSignature toDestroy = liveEntities[currDestroy];
            for (System* s : mSystems){
                s->removeEntity(currDestroy);
            }

            for (int x = 0; x < mComponentCount; x++){
                if (toDestroy[x]){
                    mComponents[x]->removeComponent(entityID);               
                }
            }
            
            liveEntities.erase(currDestroy);
        }
    }
    entitiesToDestroy.clear();
}

void World::destroyEntity(int entityID){
    if (liveEntities.find(entityID) != liveEntities.end())
        entitiesToDestroy.insert(entityID);
}

componentSignature World::getComponentSignature(int entity) {
    return liveEntities[entity];
}

void World::addComponent(int entityID, int componentID, const nlohmann::json &componentJSON) {
    liveEntities[entityID].flip(componentID);
    mComponents[componentID]->addComponentAsComponent(entityID)->deserialize(componentJSON);

    for (System* s : mSystems){
        std::vector<componentSignature> currSigs = s->getNeededComponents();
        for (componentSignature sig : currSigs){
            if ((sig & liveEntities[entityID]) == sig){
                s->addEntity(entityID, liveEntities[entityID]);
                break;
            }
        }
    }
}
