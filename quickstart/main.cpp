#include "core/runner.h"
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
#include <iostream>

int main(int argc, char *argv[]){
    if (argc < 2) {
        std::cout << "Pass in first world config txt file" << std::endl;
        return -1;
    }

    World newWorld;
    // Add custom Systems here
    newWorld.registerSystem(new PlayerMovementSystem(&newWorld));
    newWorld.registerSystem(new ProjectileSystem(&newWorld));
    // Stop adding custom Systems here
    newWorld.registerSystem(new HierarchySystem(&newWorld));
    newWorld.registerSystem(new TransformSystem(&newWorld));
    newWorld.registerSystem(new CollisionSystem(&newWorld));
    newWorld.registerSystem(new PhysicsSystem(&newWorld));
    newWorld.registerSystem(new RenderSystem(&newWorld));
    newWorld.registerComponent<HierarchyParentComponent>();
    newWorld.registerComponent<HierarchyChildrenComponent>();
    newWorld.registerComponent<TransformComponent>();
    newWorld.registerComponent<TransformGlobalComponent>();
    newWorld.registerComponent<RenderComponent>();
    newWorld.registerComponent<PhysicsComponent>();
    newWorld.registerComponent<ColliderComponent>();
    newWorld.registerComponent<BoxColliderComponent>();
    newWorld.registerComponent<SphereColliderComponent>();
    newWorld.registerComponent<CameraComponent>();
    newWorld.registerComponent<PointLightComponent>();
    newWorld.registerComponent<DirectionalLightComponent>();
    // Register Custom components here
    newWorld.registerComponent<PlayerMovementComponent>();
    newWorld.registerComponent<ProjectileComponent>();   
    return run(newWorld, argv[1]);
}