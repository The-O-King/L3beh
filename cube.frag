#version 330 core

in vec3 fragPos;
in vec2 fragTexCoord;
in vec3 fragNormal;
out vec4 FragColor;

uniform sampler2D texture;

void main(){
    FragColor = texture(texture, fragTexCoord);
}