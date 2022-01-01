#ifndef CAMERACOMPONENT_H
#define CAMERACOMPONENT_H

#include "Component.h"

struct CameraComponent : Serialization<CameraComponent> {
	bool isActive;
	float fov;

	static std::string getNameImpl() { return "Camera"; }

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(CameraComponent, isActive, fov)
};

#endif