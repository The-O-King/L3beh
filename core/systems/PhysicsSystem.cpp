#include "PhysicsSystem.h"
#include "../components.h"
#include "../world.h"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/quaternion.hpp"

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
        glm::vec3 last_accel = phys.acceleration;
        glm::vec3 last_accel_angular = phys.angularAcceleration;
        tc.position += (phys.velocity * deltaTime + (.5f * last_accel * deltaTime * deltaTime)) * phys.lockPosition;
        glm::vec3 integrateAngVel = (phys.angularVelocity + (last_accel_angular * deltaTime)) * phys.lockRotation;
        tc.rotation = glm::normalize(tc.rotation + (.5f * glm::quat(0.0f, integrateAngVel) * tc.rotation * deltaTime));

        phys.sumForces += phys.mass * phys.gravityScale * glm::vec3(0, -9.8, 0);
        glm::vec3 new_accel = phys.sumForces * phys.invMass;
        glm::vec3 avg_accel = (last_accel + new_accel) * 0.5f;
        phys.velocity += avg_accel * deltaTime;
        phys.acceleration = new_accel;
        
        glm::mat3 invInertia = phys.invInertia;
        invInertia = getRotation(tc) * invInertia * glm::transpose(getRotation(tc));
        glm::vec3 new_accel_angular = invInertia * phys.sumTorques;
        glm::vec3 avg_accel_angular = (last_accel_angular + new_accel_angular) * 0.5f;
        phys.angularVelocity += avg_accel_angular * deltaTime;
        phys.angularAcceleration = new_accel_angular;

        phys.sumForces = glm::vec3(0);
        phys.sumTorques = glm::vec3(0);
    }
}