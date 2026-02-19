#version 410 core

layout (location = 0) in vec3 vPos;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec3 FragPos;

void main(){
    FragPos = vPos;
    gl_Position = ProjectionMatrix * ViewMatrix * vec4(vPos.x, vPos.y, vPos.z, 1.0);
}