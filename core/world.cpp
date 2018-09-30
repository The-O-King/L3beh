#include "world.h"

World::World(){
    currID = 0;
}

std::vector<System*>& World::getSystems(){
    return mSystems;
}

bool World::baseWorldGen(std::string worldConfigFile){
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
                customWorldGen(baby, command, currLine);
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
    componentSignature toDestroy = liveEntities[entityID];

    for (System* s : mSystems){
        s->removeEntity(entityID);
    }
    
    mComponents.destroyEntity(entityID, toDestroy);
    liveEntities.erase(entityID);
}