#version 330 core

#define MAX_LIGHT 100

struct dirLight{
    vec3 direction;
    vec3 color;
};

struct pointLight{
    float intensity;
    vec3 position;
    vec3 color;
};

in vec3 fragPos;
in vec2 fragTexCoord;
in vec3 fragNormal;
out vec4 FragColor;

uniform sampler2D texture;
uniform vec3 cameraPos;

uniform dirLight directionLight;
uniform bool hasDirLight;

uniform pointLight pointLights[MAX_LIGHT];
uniform int numPointLight;

vec3 calcPointLight(pointLight pl){
    vec3 lightDir = normalize(pl.position - fragPos);
    float diff = max(dot(lightDir, fragNormal), 0.0);
    float dist = length(pl.position - fragPos);
    float attenuation = pl.intensity / dist;
    vec3 diffuse = diff * pl.color * attenuation;
    vec3 viewDir = normalize(cameraPos - pl.position);
    vec3 reflectDir = reflect(-lightDir, fragNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * pl.color * attenuation;
    return diffuse + specular;
}

void main(){
    vec3 res = vec3(0, 0, 0);
    for (int x = 0; x < numPointLight; x++){
        res += calcPointLight(pointLights[x]);
    }

    FragColor = vec4(res * texture(texture, fragTexCoord), 1.0);
}