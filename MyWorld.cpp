#include "core/runner.h"
#include "core/components.h"
#include "PlayerMovementSystem.h"
#include "TransformSystem.h"
#include "PhysicsSystem.h"
#include "CollisionSystem.h"
#include "RenderSystem.h"
#include "ProjectileSystem.h"
#include "MyWorld.h"

MyWorld::MyWorld(){
    mSystems.push_back(new TransformSystem(this));
    mSystems.push_back(new RenderSystem(this));
    mSystems.push_back(new PlayerMovementSystem(this));
    mSystems.push_back(new ProjectileSystem(this));
    mSystems.push_back(new CollisionSystem(this));
    mSystems.push_back(new PhysicsSystem(this));
    mComponents.registerComponent<TransformComponent>();
    mComponents.registerComponent<RenderComponent>();
    mComponents.registerComponent<PlayerMovementComponent>();
    mComponents.registerComponent<PhysicsComponent>();
    mComponents.registerComponent<ColliderComponent>();
    mComponents.registerComponent<BoxColliderComponent>();
    mComponents.registerComponent<SphereColliderComponent>();
    mComponents.registerComponent<CameraComponent>();
    mComponents.registerComponent<PointLightComponent>();
    mComponents.registerComponent<DirectionalLightComponent>();
    mComponents.registerComponent<ProjectileComponent>();    
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
        data >> pc.mass >> pc.gravityScale >> pc.restitutionCoefficient >> pc.friction >> pc.lockRotation.x >> pc.lockRotation.y >> pc.lockRotation.z >> pc.lockPosition.x >> pc.lockPosition.y >> pc.lockPosition.z;
        pc.lockRotation.x = pc.lockRotation.x ? 0 : 1; pc.lockRotation.y = pc.lockRotation.y ? 0 : 1; pc.lockRotation.z = pc.lockRotation.z ? 0 : 1;
        pc.lockPosition.x = pc.lockPosition.x ? 0 : 1; pc.lockPosition.y = pc.lockPosition.y ? 0 : 1; pc.lockPosition.z = pc.lockPosition.z ? 0 : 1;
        if (pc.mass < 0)  pc.mass = 0;
        pc.mass == 0 ? pc.invMass = 0 : pc.invMass = 1/pc.mass;
        addComponentToEntity<PhysicsComponent>(entityID, pc);
    }
    else if (command == "BoxCollider"){
        BoxColliderComponent cc;
        data >> cc.isTrigger >> cc.offset.x >> cc.offset.y >> cc.offset.z >> cc.halfSize.x >> cc.halfSize.y >> cc.halfSize.z;
        addComponentToEntity<BoxColliderComponent>(entityID, cc);
        addComponentToEntity<ColliderComponent>(entityID, cc);
    }
    else if (command == "SphereCollider"){
        SphereColliderComponent sc;
        data >> sc.isTrigger >> sc.offset.x >> sc.offset.y >> sc.offset.z >> sc.radius;
        addComponentToEntity<SphereColliderComponent>(entityID, sc);
        addComponentToEntity<ColliderComponent>(entityID, sc);
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
    else if (command == "DirectionalLight"){
        DirectionalLightComponent dc;
        data >> dc.color.r >> dc.color.g >> dc.color.b >> dc.direction.x >> dc.direction.y >> dc.direction.z;
        addComponentToEntity<DirectionalLightComponent>(entityID, dc);
    }
    else if (command == "Projectile"){
        ProjectileComponent pc;
        data >> pc.damage;
        addComponentToEntity<ProjectileComponent>(entityID, pc);
    }
    return true;
}