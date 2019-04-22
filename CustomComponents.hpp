#ifndef CUSTOMCOMPONENT_HPP
#define CUSTOMCOMPONENT_HPP

using namespace std;

struct PlayerMovementComponent{
	bool active;
	int currProjectile = 0;
	float walkSpeed = 3.0;
	float runSpeed = 6.0;
};

struct ProjectileComponent{
	float damage = 0;
};

#endif