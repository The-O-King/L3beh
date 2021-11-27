#include "core/runner.h"
#include "core/systems/PhysicsSystem.h"
#include "core/systems/CollisionSystem.h"
#include "core/systems/RenderSystem.h"
#include <core/components/TransformComponent.h>
#include <core/components/RenderComponent.h>
#include <core/components/PhysicsComponent.h>
#include <core/components/LightComponents.h>
#include <core/components/CameraComponent.h>
#include "PlayerMovementSystem.h"
#include "ProjectileSystem.h"
#include "MyWorld.h"

MyWorld::MyWorld(){
    mSystems.push_back(new RenderSystem(this));
    // Add custom Systems here
    mSystems.push_back(new PlayerMovementSystem(this));
    mSystems.push_back(new ProjectileSystem(this));
    // Stop adding custom Systems here
    mSystems.push_back(new CollisionSystem(this));
    mSystems.push_back(new PhysicsSystem(this));
    mComponents.registerComponent<TransformComponent>();
    mComponents.registerComponent<RenderComponent>();
    mComponents.registerComponent<PhysicsComponent>();
    mComponents.registerComponent<ColliderComponent>();
    mComponents.registerComponent<BoxColliderComponent>();
    mComponents.registerComponent<SphereColliderComponent>();
    mComponents.registerComponent<CameraComponent>();
    mComponents.registerComponent<PointLightComponent>();
    mComponents.registerComponent<DirectionalLightComponent>();
    // Register Custom components here
    mComponents.registerComponent<PlayerMovementComponent>();
    mComponents.registerComponent<ProjectileComponent>();    
}

bool MyWorld::customWorldGen(int entityID, 
                             std::string command, 
                             std::istringstream& data){
    if (command == "PlayerMovement"){
        PlayerMovementComponent pc;
        pc.owner = entityID;
        data >> pc.walkSpeed >> pc.runSpeed;
        addComponentToEntity<PlayerMovementComponent>(entityID, pc);
    }
    else if (command == "Projectile"){
        ProjectileComponent pc;
        pc.owner = entityID;
        data >> pc.damage;
        addComponentToEntity<ProjectileComponent>(entityID, pc);
    }
    return true;
}