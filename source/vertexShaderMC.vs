#version 330 core
// Vertex Shader for mean curvature
// IMPLEMENT
    layout (location = 0) in vec3 aPos;
    layout (location = 2) in vec3 mean_curvature;

    out vec4 color;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    vec3 interpolation(vec3 v0, vec3 v1, float t) {
        return (1 - t) * v0 + t * v1;
    }

    vec4 get_color(vec3 pos){
       // i have 3 different values, not 1 like in gaussian curvature !!!!!!!
       vec3 red = vec3(1.0, 0.0, 0.0);
       vec3 green = vec3(0.0, 1.0, 0.0);
       vec3 blue = vec3(0.0, 0.0, 1.0);

        if (pos.x < pos.y && pos.x < pos.z)
            // interpolation from red to green
            return vec4(interpolation(red, green, mean_curvature[0]), 1.0);
        else if(pos.y < pos.x && pos.y < pos.z)
            // interpolation from green to blue
            return vec4(interpolation(green, blue, mean_curvature[1]), 1.0);
        else
            // interpolation from blue to red
            return vec4(interpolation(blue, red, mean_curvature[2]), 1.0);
    }

    void main() {
        vec3 pos = vec3(model * vec4(aPos, 1.0));

        color = get_color(pos); //vertex color obtained using gaussian curvature

        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }