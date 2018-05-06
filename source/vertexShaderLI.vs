#version 330 core
// Vertex Shader for linear interpolation
layout (location = 0) in vec3 aPos;
layout (location = 3) in vec3 aColor;

out vec4 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    color = vec4(aColor, 1.0);
}