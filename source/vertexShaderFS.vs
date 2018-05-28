#version 330 core
// Vertex Shader for extend flat shading
    layout (location = 0) in vec3 aPos;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    out vec3 world_position;

    void main() {
        world_position = vec3(model * vec4(aPos, 1.0));
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }

