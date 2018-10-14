#include "core/runner.h"
#include "PlayerMovementSystem.h"
#include "RotateSystem.h"
#include "PhysicsSystem.h"
#include "RenderSystem.h"
#include "MyWorld.h"

MyWorld::MyWorld(){
    mSystems.push_back(new PlayerMovementSystem(this));
    mSystems.push_back(new RotateSystem(this));
    mSystems.push_back(new PhysicsSystem(this));
    mSystems.push_back(new RenderSystem(this));
    mComponents.registerComponent<TransformComponent>();
    mComponents.registerComponent<RenderComponent>();
    mComponents.registerComponent<PlayerMovementComponent>();
    mComponents.registerComponent<PhysicsComponent>();
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
        data >> rc.modelFileName >> rc.vertShaderFileName >> rc.fragShaderFileName;
        addComponentToEntity<RenderComponent>(entityID, rc);
    }
    else if (command == "PlayerMovement"){
        PlayerMovementComponent pc;
        data >> pc.active;
        addComponentToEntity<PlayerMovementComponent>(entityID, pc);
    }
    else if (command == "Physics"){
        PhysicsComponent pc;
        data >> pc.useGravity >> pc.isKinematic >> pc.mass;
        addComponentToEntity<PhysicsComponent>(entityID, pc);
    }
    return true;
}