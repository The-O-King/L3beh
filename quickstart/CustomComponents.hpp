#ifndef CUSTOMCOMPONENT_HPP
#define CUSTOMCOMPONENT_HPP

#include <core/components/Component.h>

struct PlayerMovementComponent : public Component{
    float walkSpeed = 3.0f;
    float runSpeed = 5.0f;
    float jumpForce = 500.0f;
    bool isJumping = false;
    bool isRunning = false;
    int currProjectile = 0;
};

struct ProjectileComponent : public Component{
	float damage = 0;
};

#endif