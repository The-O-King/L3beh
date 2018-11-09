#include "PhysicsSystem.h"
#include "CustomComponents.hpp"
#include "core/components.h"
#include "core/world.h"

PhysicsSystem::PhysicsSystem(World* w){
    mWorld = w;
    componentSignature main;
    main[type_id<TransformComponent>()] = 1;
    main[type_id<PhysicsComponent>()] = 1;
    neededComponentSignatures.push_back(main);
}

void PhysicsSystem::init(){

}

void PhysicsSystem::update(float deltaTime){
    for (int e : entities){
        TransformComponent& tc = mWorld->getComponent<TransformComponent>(e);
        PhysicsComponent& phys = mWorld->getComponent<PhysicsComponent>(e);
        if (phys.type == PhysicsType::DYNAMIC){
            glm::vec3 last_accel = phys.acceleration;
            tc.position += phys.velocity * deltaTime + (.5f * last_accel * deltaTime * deltaTime);

            phys.sumForces += phys.mass * phys.gravityScale * glm::vec3(0, -9.8, 0);
            glm::vec3 new_accel = phys.sumForces * phys.invMass;
            glm::vec3 avg_accel = (last_accel + new_accel) / 2.0f;
            phys.velocity += avg_accel * deltaTime;

            phys.sumForces = glm::vec3(0);
        }
        else{
            phys.velocity = glm::vec3(0);
            phys.acceleration = glm::vec3(0);
        }
    }
}