#version 330 core
// Vertex Shader for gaussian curvature
    layout (location = 0) in vec3 aPos;
    layout (location = 2) in vec3 gaussian_curvature;
    layout (location = 4) in vec3 mean_curvature_vertex;
    layout (location = 3) in vec3 mean_curvature_edge;


    out vec4 color;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    uniform float min_curvature;
    uniform float max_curvature;

    uniform bool isGaussian;
    uniform bool isMeanCurvatureEdge;

    vec3 interpolation(vec3 v0, vec3 v1, float t) {
        return (1 - t) * v0 + t * v1;
    }

    vec3 hsv2rgb(vec3 c)
    {
        vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
        vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
        return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
    }


    vec4 get_result_color_gc(){
        float val = gaussian_curvature[0]; // gaussian_curvature is a vec3 composed by same value
        if(!isGaussian && !isMeanCurvatureEdge)
            val = mean_curvature_vertex[0]; // mean curvature is a vec3 composed by same value
        else if(!isGaussian && isMeanCurvatureEdge)
            val = mean_curvature_edge[0]; // mean curvature is a vec3 composed by same value

       // colors in HSV
       vec3 red = vec3(0.0, 1.0, 1.0); //h s v
       vec3 green = vec3(0.333, 1.0, 1.0);
       vec3 blue = vec3(0.6667, 1.0, 1.0);

       if (val < 0) { //negative numbers until 0
            return vec4(hsv2rgb(interpolation(green, red, min(val/min_curvature, 1.0))), 1.0);
        } else { //from 0 to positive
            return vec4(hsv2rgb(interpolation(green, blue, min(val/max_curvature, 1.0))), 1.0);
        }
    }


    void main() {
        vec3 pos = vec3(model * vec4(aPos, 1.0));

        color = get_result_color_gc(); //vertex color obtained using gaussian curvature

        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }

