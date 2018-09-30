#version 330 core

layout (location = 0) in vec3 pos;
uniform mat4 MVP;
out vec3 fragPos;

void main(){
    gl_Position = MVP * vec4(pos, 1.0);
    fragPos = pos;
}