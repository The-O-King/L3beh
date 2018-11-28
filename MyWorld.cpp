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
    mComponents.registerComponent<CameraComponent>();
    mComponents.registerComponent<PointLightComponent>();
}

bool MyWorld::customWorldGen(int entityID, std::string command, std::istringstream& data){
    if (command == "Render"){
        RenderComponent rc;
        data >> rc.modelFileName >> rc.textureName >> rc.diffuse.r >> rc.diffuse.g >> rc.diffuse.b >> rc.specular.r >> rc.specular.g >> rc.specular.b >> rc.shininess;
        addComponentToEntity<RenderComponent>(entityID, rc);
    }
    else if (command == "PlayerMovement"){
        PlayerMovementComponent pc;
        data >> pc.active;
        addComponentToEntity<PlayerMovementComponent>(entityID, pc);
    }
    else if (command == "Physics"){
        PhysicsComponent pc;
        data >> pc.type >> pc.gravityScale >> pc.mass >> pc.restitutionCoefficient;
        if (pc.mass <= 0)  pc.mass = 0.000001;
        pc.invMass = 1/pc.mass;
        addComponentToEntity<PhysicsComponent>(entityID, pc);
    }
    else if (command == "Collider"){
        ColliderComponent cc;
        data >> cc.isTrigger >> cc.boxMin.x >> cc.boxMin.y >> cc.boxMin.z >> cc.boxMax.x >> cc.boxMax.y >> cc.boxMax.z >> cc.sphereRadius;
        addComponentToEntity<ColliderComponent>(entityID, cc);
    }
    else if (command == "Camera"){
        CameraComponent cc;
        data >> cc.isActive >> cc.fov;
        addComponentToEntity<CameraComponent>(entityID, cc);
    }
    else if (command == "PointLight"){
        PointLightComponent pc;
        data >> pc.intensity >> pc.color.r >> pc.color.g >> pc.color.b;
        addComponentToEntity<PointLightComponent>(entityID, pc);
    }
    return true;
}