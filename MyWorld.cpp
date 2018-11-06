#include "core/runner.h"
#include "core/components.h"
#include "PlayerMovementSystem.h"
#include "TransformSystem.h"
#include "PhysicsSystem.h"
#include "CollisionSystem.h"
#include "RenderSystem.h"
#include "MyWorld.h"

MyWorld::MyWorld(){
    mSystems.push_back(new TransformSystem(this));
    mSystems.push_back(new RenderSystem(this));
    mSystems.push_back(new PlayerMovementSystem(this));
    mSystems.push_back(new PhysicsSystem(this));
    mSystems.push_back(new CollisionSystem(this));
    mComponents.registerComponent<TransformComponent>();
    mComponents.registerComponent<RenderComponent>();
    mComponents.registerComponent<PlayerMovementComponent>();
    mComponents.registerComponent<PhysicsComponent>();
    mComponents.registerComponent<ColliderComponent>();
}

bool MyWorld::customWorldGen(int entityID, std::string command, std::istringstream& data){
    if (command == "Render"){
        RenderComponent rc;
        data >> rc.modelFileName >> rc.vertShaderFileName >> rc.fragShaderFileName >> rc.textureName;
        addComponentToEntity<RenderComponent>(entityID, rc);
    }
    else if (command == "PlayerMovement"){
        PlayerMovementComponent pc;
        data >> pc.active;
        addComponentToEntity<PlayerMovementComponent>(entityID, pc);
    }
    else if (command == "Physics"){
        PhysicsComponent pc;
        data >> pc.useGravity >> pc.isKinematic >> pc.mass >> pc.restitutionCoefficient;
        if (pc.mass <= 0)  pc.mass = 0.000001;
        pc.invMass = 1/pc.mass;
        addComponentToEntity<PhysicsComponent>(entityID, pc);
    }
    else if (command == "Collider"){
        ColliderComponent cc;
        data >> cc.isTrigger >> cc.boxMin.x >> cc.boxMin.y >> cc.boxMin.z >> cc.boxMax.x >> cc.boxMax.y >> cc.boxMax.z >> cc.sphereRadius;
        addComponentToEntity<ColliderComponent>(entityID, cc);
    }
    return true;
}