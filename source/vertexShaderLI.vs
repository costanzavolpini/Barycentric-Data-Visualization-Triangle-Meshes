#version 330 core
// Vertex Shader for linear interpolation
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aColor;

out vec3 resColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    resColor = aColor;
}