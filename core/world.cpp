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

int World::createEntity(std::istream& entityConfig){
    int baby = -1;
    std::string line;
    std::string command;
    getline(entityConfig, line);
    std::istringstream currLine(line);
    currLine >> command;
    if (command == "Entity"){
        baby = createEntity();
        std::string tag;
        currLine >> tag;
        getline(entityConfig, line);
        while (line != "/Entity"){
            currLine.str(line);
            currLine.seekg(0);
            currLine >> command;
            if (command == "Transform"){
                TransformComponent& tc = addComponent<TransformComponent>(baby);;
                addComponent<TransformGlobalComponent>(baby);
                float x, y, z, xx, yy, zz, xs, ys, zs;
                currLine >> x >> y >> z >> xx >> yy >> zz >> xs >> ys >> zs;
                tc.position = glm::vec3(x, y, z);
                tc.rotation = glm::vec3(glm::radians(xx), glm::radians(yy), glm::radians(zz));
                tc.scale = glm::vec3(xs, ys, zs);
            }
            if (command == "Parent"){
                HierarchyParentComponent& hc = addComponent<HierarchyParentComponent>(baby);
                currLine >> hc.parent;
            }
            else if (command == "Render"){
                RenderComponent& rc = addComponent<RenderComponent>(baby);
                currLine >> rc.modelFileName >> rc.textureName >> rc.diffuse.r >> rc.diffuse.g >> rc.diffuse.b >> rc.specular.r >> rc.specular.g >> rc.specular.b >> rc.shininess;
            }
            else if (command == "Physics"){
                PhysicsComponent& pc = addComponent<PhysicsComponent>(baby);
                currLine >> pc.mass >> pc.gravityScale >> pc.restitutionCoefficient >> pc.friction >> pc.lockRotation.x >> pc.lockRotation.y >> pc.lockRotation.z >> pc.lockPosition.x >> pc.lockPosition.y >> pc.lockPosition.z;
                pc.lockRotation.x = pc.lockRotation.x ? 0 : 1; pc.lockRotation.y = pc.lockRotation.y ? 0 : 1; pc.lockRotation.z = pc.lockRotation.z ? 0 : 1;
                pc.lockPosition.x = pc.lockPosition.x ? 0 : 1; pc.lockPosition.y = pc.lockPosition.y ? 0 : 1; pc.lockPosition.z = pc.lockPosition.z ? 0 : 1;
                if (pc.mass < 0)  pc.mass = 0;
                pc.mass == 0 ? pc.invMass = 0 : pc.invMass = 1/pc.mass;
            }
            else if (command == "BoxCollider"){
                ColliderComponent& cc = addComponent<ColliderComponent>(baby);
                BoxColliderComponent& bc = addComponent<BoxColliderComponent>(baby);
                cc.type = ColliderType::BOX;
                currLine >> cc.isTrigger >> cc.offset.x >> cc.offset.y >> cc.offset.z;
                currLine >> bc.halfSize.x >> bc.halfSize.y >> bc.halfSize.z;
            }
            else if (command == "SphereCollider"){
                ColliderComponent& cc = addComponent<ColliderComponent>(baby);
                SphereColliderComponent& sc = addComponent<SphereColliderComponent>(baby);
                cc.type = ColliderType::SPHERE;
                currLine >> cc.isTrigger >> cc.offset.x >> cc.offset.y >> cc.offset.z;
                currLine >> sc.radius;
            }
            else if (command == "Camera"){
                CameraComponent& cc = addComponent<CameraComponent>(baby);
                currLine >> cc.isActive >> cc.fov;
            }
            else if (command == "PointLight"){
                PointLightComponent& pc = addComponent<PointLightComponent>(baby);
                currLine >> pc.intensity >> pc.color.r >> pc.color.g >> pc.color.b;
            }
            else if (command == "DirectionalLight"){
                DirectionalLightComponent& dc = addComponent<DirectionalLightComponent>(baby);
                currLine >> dc.color.r >> dc.color.g >> dc.color.b >> dc.direction.x >> dc.direction.y >> dc.direction.z;
            }            
            else{
                customWorldGen(baby, command, currLine);
            }
            getline(entityConfig, line);
        }
    }
    return baby;
}

int World::createEntity(std::string entityConfig){
    std::istringstream f(entityConfig);
    return createEntity(f);
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
            
            auto components = entity["components"];
            for (auto& component : components.items()) {
                int compID = mComponentNameToTypeID[component.key()];
                addComponent(id, compID, component.value());
            }
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

bool World::baseWorldGen(std::string worldConfigFile){
    std::ifstream inputFile;
    inputFile.open(worldConfigFile, std::ifstream::in);
    if (!inputFile.is_open())
        return false;

    while(!inputFile.eof()){
        createEntity(inputFile);
    }
    return true;
}

bool World::customWorldGen(int entityID, std::string command, std::istringstream& data){    
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
