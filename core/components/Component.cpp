#include "Component.h"

namespace glm
{
	void from_json(const nlohmann::json& j, vec3& vec)
	{
		vec.x = j[0];
		vec.y = j[1];
		vec.z = j[2];
	}

	void to_json(nlohmann::json& j, const vec3& vec)
	{
  		j = nlohmann::json::array({ vec.x,
		  						    vec.y,
								 	vec.z});
	}

	void from_json(const nlohmann::json& j, quat& qua)
	{
		qua.x = j[0];
		qua.y = j[1];
		qua.z = j[2];
		qua.w = j[3];
	}

	void to_json(nlohmann::json& j, const quat& qua)
	{
  		j = nlohmann::json::array({ qua.x,
		  						    qua.y,
								 	qua.z,
									qua.w });
	}
}