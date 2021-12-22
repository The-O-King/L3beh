#version 330 core

#define MAX_LIGHT 10

struct dirLight{
    vec3 direction;
    vec3 color;
};

struct pointLight{
    float intensity;
    vec3 position;
    vec3 color;
};

uniform dirLight directionLight;
uniform pointLight pointLights[MAX_LIGHT];
uniform int numPointLight;

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec4 tangent;

uniform mat4 MVP;
uniform mat4 model;
uniform mat3 invTranspose;
uniform vec3 cameraPos;

out vec3 fragPosWorld;
out vec3 fragPosTangent;
out vec2 fragTexCoord;
out vec3 cameraPosTangent;
out vec3 fragDirLight;
out vec3 fragPointLights[MAX_LIGHT];

void main(){
    vec3 worldNormal = normalize(invTranspose * normal);
    vec3 worldTangent = normalize(invTranspose * tangent.xyz);
    vec3 worldBitangent = normalize(cross(worldNormal, worldTangent)) * tangent.w;
    mat3 tangentToWorld = {worldTangent, worldBitangent, worldNormal};
    // Transpose b/c inverse of an orthogonal matrix is it's transpose
    mat3 worldToTangent = transpose(tangentToWorld);

    gl_Position = MVP * vec4(pos, 1.0);
    fragPosWorld = vec3(model * vec4(pos, 1.0));
    fragPosTangent = worldToTangent * fragPosWorld;
    fragTexCoord = texCoord;
    cameraPosTangent = worldToTangent * cameraPos;

    for (int x = 0; x < numPointLight; x++) {
        fragPointLights[x] = worldToTangent * pointLights[x].position; 
    }
    fragDirLight = worldToTangent * directionLight.direction;
}