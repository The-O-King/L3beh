#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

uniform mat4 MVP;
uniform mat4 model;
uniform mat3 invTranspose;

out vec3 fragPos;
out vec2 fragTexCoord;
out vec3 fragNormal;
out vec3 cameraPos;

void main(){
    gl_Position = MVP * vec4(pos, 1.0);
    fragPos = vec3(model * vec4(pos, 1.0));
    fragTexCoord = texCoord;
    fragNormal = normalize(invTranspose * normal);
}