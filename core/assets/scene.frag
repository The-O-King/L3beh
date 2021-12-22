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

struct material{
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

in vec3 fragPosWorld;
in vec3 fragPosTangent;
in vec2 fragTexCoord;
in vec3 cameraPosTangent;
in vec3 fragDirLight;
in vec3 fragPointLights[MAX_LIGHT];
out vec4 FragColor;

uniform sampler2D texture;
uniform vec3 cameraPos;

uniform dirLight directionLight;
uniform bool hasDirLight;

uniform pointLight pointLights[MAX_LIGHT];
uniform int numPointLight;

uniform material mat;

const vec3 fragNormalTangent = vec3(0, 0, 1);

vec3 calcPointLight(pointLight pl, vec3 plPosition){
    vec3 lightDir = normalize(plPosition - fragPosTangent);
    vec3 viewDir = normalize(cameraPosTangent - fragPosTangent);
    vec3 reflectDir = reflect(-lightDir, fragNormalTangent);
    float dist = length(plPosition - fragPosTangent);
    float attenuation = pl.intensity / dist;

    vec3 diff = max(dot(lightDir, fragNormalTangent), 0.0) * mat.diffuse;
    vec3 diffuse = diff * pl.color * attenuation;
    vec3 spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess) * mat.specular;
    vec3 specular = spec * pl.color * attenuation;
    return diffuse + specular;
}

vec3 calcDirLight(dirLight dl, vec3 dlDirection){
    vec3 lightDir = normalize(-dlDirection);
    vec3 viewDir = normalize(cameraPosTangent - fragPosTangent);
    vec3 reflectDir = reflect(-lightDir, fragNormalTangent);

    vec3 diff = max(dot(lightDir, fragNormalTangent), 0.0) * mat.diffuse;
    vec3 diffuse = diff * dl.color;
    vec3 spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess) * mat.specular;
    vec3 specular = spec * dl.color;
    return diffuse + specular;
}

void main(){
    vec3 res = vec3(0, 0, 0);
    for (int x = 0; x < numPointLight; x++){
        res += calcPointLight(pointLights[x], fragPointLights[x]);
    }

    if (hasDirLight){
        res += calcDirLight(directionLight, fragDirLight);
    }

    FragColor = vec4(res, 1.0) * texture2D(texture, fragTexCoord);
}