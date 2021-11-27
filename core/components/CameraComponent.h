#ifndef CAMERACOMPONENT_H
#define CAMERACOMPONENT_H

#include "Component.h"

struct CameraComponent : public Component {
	bool isActive;
	float fov;
};

#endif