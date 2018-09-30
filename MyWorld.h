#ifndef MYWORLD_H
#define MYWORLD_H

#include "core/world.h"

class MyWorld : public World{
    public:
        MyWorld();
        bool customWorldGen(int entityID, std::string command, std::istringstream& data) override;    
};

#endif