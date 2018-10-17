#include "world.h"

World::World(){
    currID = 0;
}

std::vector<System*>& World::getSystems(){
    return mSystems;
}

bool World::baseWorldGen(std::string worldConfigFile){
    int worldRoot = createEntity();
    TransformComponent worldRootTC;
    addComponentToEntity<TransformComponent>(worldRoot, worldRootTC);

    std::ifstream inputFile;
    inputFile.open(worldConfigFile, std::ifstream::in);

    std::string line;
    std::string tag;
    std::string command;
    while(getline(inputFile, line)){
        std::istringstream currLine(line);
        currLine >> command;
        if (command == "Entity"){
            int baby = createEntity();
            currLine >> tag;
            getline(inputFile, line);
            while (line != "/Entity"){
                currLine.str(line);
                currLine.seekg(0);
                currLine >> command;
                if (command == "Transform"){
                    TransformComponent tc;
                    float parent, x, y, z, xx, yy, zz, xs, ys, zs;
                    currLine >> parent >> x >> y >> z >> xx >> yy >> zz >> xs >> ys >> zs;
                    tc.position = glm::vec3(x, y, z);
                    tc.rotation = glm::vec3(xx, yy, zz);
                    tc.scale = glm::vec3(xs, ys, zs);
                    tc.parentEntity = parent;
                    TransformComponent& parentTransform = getComponent<TransformComponent>(parent);
                    parentTransform.childEntities.push_back(baby);
                    addComponentToEntity<TransformComponent>(baby, tc);
                }
                else{
                    customWorldGen(baby, command, currLine);
                }
                getline(inputFile, line);
            }
        }
        command = "";
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

void World::destroyEntity(int entityID){
    vector<int> destroyList;
    destroyList.push_back(entityID);
    while (!destroyList.empty()){
        int currDestroy = destroyList.back(); destroyList.pop_back();
        vector<int> toAdd = getComponent<TransformComponent>(currDestroy).childEntities;
        destroyList.insert(destroyList.end(), toAdd.begin(), toAdd.end());

        componentSignature toDestroy = liveEntities[currDestroy];
        for (System* s : mSystems){
            s->removeEntity(currDestroy);
        }
        mComponents.destroyEntity(entityID, toDestroy);
        liveEntities.erase(currDestroy);
    }
}