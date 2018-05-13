#version 330 core
// Vertex Shader for mean curvature
// IMPLEMENT
    layout (location = 0) in vec3 aPos;
    layout (location = 2) in vec3 mean_curvature;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    out COLOR_OUT {
        vec4 color_1;
        vec4 color_2;
        vec4 color_3;
    } color_out;

    vec3 interpolation(vec3 v0, vec3 v1, float t) {
        return (1 - t) * v0 + t * v1;
    }

    vec4 get_color(float val){
       vec3 red = vec3(1.0, 0.0, 0.0);
       vec3 green = vec3(0.0, 1.0, 0.0);
       vec3 blue = vec3(0.0, 0.0, 1.0);

        if (val < 0) { //negative numbers until 0 -> map from red to green
            return vec4(interpolation(red, green, val), 1.0);
        } else { //map from green to blue, from 0 to positive
            return vec4(interpolation(green, blue, val), 1.0);
        }
    }

    void main() {
        vec3 pos = vec3(model * vec4(aPos, 1.0));

        color_out.color_1 = get_color(mean_curvature[0]); //vertex color obtained using gaussian curvature
        color_out.color_2 = get_color(mean_curvature[1]); //vertex color obtained using gaussian curvature
        color_out.color_3 = get_color(mean_curvature[2]); //vertex color obtained using gaussian curvature

        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }