#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <third_party/nlohmann/json.hpp>

namespace glm
{
	void from_json(const nlohmann::json& j, vec3& vec);
	void to_json(nlohmann::json& j, const vec3& vec);
	void from_json(const nlohmann::json& j, quat& qua);
	void to_json(nlohmann::json& j, const quat& qua);
}

struct Component {
	int owner = -1;
	virtual void serialize(nlohmann::json& input) const = 0;
	virtual void deserialize(const nlohmann::json& output) = 0;
};

template <class T>
struct Serialization : public Component {

	static std::string getName() {
		return T::getNameImpl();
	}

	void serialize(nlohmann::json& input) const {
	    to_json(input, *static_cast<const T*>(this));
	}

	void deserialize(const nlohmann::json& output) {
		from_json(output, *static_cast<T*>(this));
	}
};

#endif