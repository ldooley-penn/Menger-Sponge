#version 410 core

in vec3 FragPos;

void main(){
    gl_FragColor = vec4(abs(FragPos), 1.0);
}