#include "core/runner.h"
#include "PlayerMovementSystem.h"
#include "RotateSystem.h"
#include "RenderSystem.h"
#include "MyWorld.h"

MyWorld::MyWorld(){
    mSystems.push_back(new PlayerMovementSystem(this));
    mSystems.push_back(new RotateSystem(this));
    mSystems.push_back(new RenderSystem(this));
    mComponents.registerComponent<TransformComponent>();
    mComponents.registerComponent<RenderComponent>();
    mComponents.registerComponent<PlayerMovementComponent>();
}

bool MyWorld::customWorldGen(int entityID, std::string command, std::istringstream& data){
    if (command == "Transform"){
        TransformComponent tc;
        float x, y, z, xx, yy, zz, xs, ys, zs;
        data >> x >> y >> z >> xx >> yy >> zz >> xs >> ys >> zs;
        tc.position = glm::vec3(x, y, z);
        tc.rotation = glm::vec3(xx, yy, zz);
        tc.scale = glm::vec3(xs, ys, zs);
        addComponentToEntity<TransformComponent>(entityID, tc);
    }
    else if (command == "Render"){
        RenderComponent rc;
        std::string model;
        data >> model;
        rc.modelFileName = model;
        data >> model;
        rc.vertShaderFileName = model;
        data >> model;
        rc.fragShaderFileName = model;
        addComponentToEntity<RenderComponent>(entityID, rc);
    }
    else if (command == "PlayerMovement"){
        PlayerMovementComponent pc;
        int active;
        data >> active;
        pc.active = active;
        addComponentToEntity<PlayerMovementComponent>(entityID, pc);
    }
    return true;
}