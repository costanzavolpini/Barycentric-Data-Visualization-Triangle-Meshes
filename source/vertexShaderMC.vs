#version 330 core
// Vertex Shader for mean curvature
// IMPLEMENT
    layout (location = 0) in vec3 aPos;
    layout (location = 3) in vec3 mean_curvature;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    out COLOR_OUT {
        vec4 color_1;
        vec4 color_2;
        vec4 color_3;
    } color_out;

    uniform float min_mc;
    uniform float max_mc;

    vec3 interpolation(vec3 v0, vec3 v1, float t) {
        return (1 - t) * v0 + t * v1;
    }

    vec3 hsv2rgb(vec3 c)
    {
        vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
        vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
        return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
    }


    vec4 get_result_color_mc(float val){
       // colors in HSV
       vec3 red = vec3(0.0, 1.0, 1.0); //h s v
       vec3 green = vec3(0.333, 1.0, 1.0);
       vec3 blue = vec3(0.6667, 1.0, 1.0);

       if (val < 0) { //negative numbers until 0
            return vec4(hsv2rgb(interpolation(green, red, min(val/min_mc, 1.0))), 1.0);
        } else { //from 0 to positive
            return vec4(hsv2rgb(interpolation(green, blue, min(val/max_mc, 1.0))), 1.0);
        }
    }

    void main() {
        vec3 pos = vec3(model * vec4(aPos, 1.0));

        color_out.color_1 = get_result_color_mc(mean_curvature[0]); //vertex color obtained using gaussian curvature
        color_out.color_2 = get_result_color_mc(mean_curvature[1]); //vertex color obtained using gaussian curvature
        color_out.color_3 = get_result_color_mc(mean_curvature[2]); //vertex color obtained using gaussian curvature

        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }