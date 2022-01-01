#ifndef CUSTOMCOMPONENT_HPP
#define CUSTOMCOMPONENT_HPP

#include <core/components/Component.h>

struct PlayerMovementComponent : Serialization<PlayerMovementComponent>{
    float walkSpeed = 3.0f;
    float runSpeed = 5.0f;
    float jumpForce = 500.0f;
    bool isJumping = false;
    bool isRunning = false;
    int currProjectile = 0;

    static std::string getName() { return "PlayerMovement"; }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(PlayerMovementComponent, walkSpeed, runSpeed, jumpForce, isJumping, isRunning, currProjectile)
};

struct ProjectileComponent : Serialization<ProjectileComponent> {

    float damage = 0;

    static std::string getNameImpl() { return "Projectile"; }

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(ProjectileComponent, damage)
};

#endif