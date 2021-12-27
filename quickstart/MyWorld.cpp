#include <core/runner.h>
#include <core/systems/HierarchySystem.h>
#include <core/systems/TransformSystem.h>
#include <core/systems/PhysicsSystem.h>
#include <core/systems/CollisionSystem.h>
#include <core/systems/RenderSystem.h>
#include <core/components/HierarchyParentComponent.h>
#include <core/components/HierarchyChildrenComponent.h>
#include <core/components/TransformComponent.h>
#include <core/components/TransformGlobalComponent.h>
#include <core/components/RenderComponent.h>
#include <core/components/PhysicsComponent.h>
#include <core/components/LightComponents.h>
#include <core/components/CameraComponent.h>
#include "PlayerMovementSystem.h"
#include "ProjectileSystem.h"
#include "MyWorld.h"

MyWorld::MyWorld(){
    // Add custom Systems here
    mSystems.push_back(new PlayerMovementSystem(this));
    mSystems.push_back(new ProjectileSystem(this));
    // Stop adding custom Systems here
    mSystems.push_back(new HierarchySystem(this));
    mSystems.push_back(new TransformSystem(this));
    mSystems.push_back(new CollisionSystem(this));
    mSystems.push_back(new PhysicsSystem(this));
    mSystems.push_back(new RenderSystem(this));
    registerComponent<HierarchyParentComponent>();
    registerComponent<HierarchyChildrenComponent>();
    registerComponent<TransformComponent>();
    registerComponent<TransformGlobalComponent>();
    registerComponent<RenderComponent>();
    registerComponent<PhysicsComponent>();
    registerComponent<ColliderComponent>();
    registerComponent<BoxColliderComponent>();
    registerComponent<SphereColliderComponent>();
    registerComponent<CameraComponent>();
    registerComponent<PointLightComponent>();
    registerComponent<DirectionalLightComponent>();
    // Register Custom components here
    registerComponent<PlayerMovementComponent>();
    registerComponent<ProjectileComponent>();    
}

bool MyWorld::customWorldGen(int entityID, 
                             std::string command, 
                             std::istringstream& data){
    if (command == "PlayerMovement"){
        PlayerMovementComponent& pc = addComponent<PlayerMovementComponent>(entityID);
        data >> pc.walkSpeed >> pc.runSpeed;
    }
    else if (command == "Projectile"){
        ProjectileComponent& pc = addComponent<ProjectileComponent>(entityID);
        data >> pc.damage;
    }
    return true;
}