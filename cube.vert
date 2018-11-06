#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

uniform mat4 MVP;
out vec3 fragPos;
out vec2 fragTexCoord;
out vec3 fragNormal;

void main(){
    gl_Position = MVP * vec4(pos, 1.0);
    fragPos = pos;
    fragTexCoord = texCoord;
    fragNormal = normal;
}