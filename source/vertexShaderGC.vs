#version 330 core
// Vertex Shader for gaussian curvature
    layout (location = 0) in vec3 aPos;
    layout (location = 2) in vec3 gaussian_curvature;

    out vec4 color;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    uniform float min_gc;
    uniform float max_gc;

    vec3 interpolation(vec3 v0, vec3 v1, float t) {
        return (1 - t) * v0 + t * v1;
    }

    /**
    * Function to interpolate between 2 HSV colors given a value
    */
    vec3 interpolation_HSV (vec3 a, vec3 b, float t)
    {
        // Hue interpolation
        float h;
        float d = b[0] - a[0];
        if (a[0] > b[0])
        {
            // Swap (a.h, b.h)
            float h3 = b[0];
            b[0] = a[0];
            a[0] = h3;

            d = -d;
            t = 1 - t;
        }

        if (d > 0.5) // 180deg
        {
            a[0] = a[0] + 1; // 360deg
            h = mod(( a[0] + t * (b[0] - a[0]) ), 1); // 360deg
        }
        if (d <= 0.5) // 180deg
            h = a[0] + t * d;

        // Interpolates the rest
        return vec3(h, a[1] + t * (b[1]-a[1]), a[2] + t * (b[2]-a[2]));
    }

    // vec3 get_HSV_color(vec3 RGB){
    //     float H = 0.0;
    //     float S = 0.0;
    //     float V = 0.0;

    //     float R = RGB.x;
    //     float G = RGB.y;
    //     float B = RGB.z;

    //     float min_RGB = min(min(R, G), B);
    //     float max_RGB = max(max(R, G), B);

    //     float delta = max_RGB - min_RGB;

    //     if(delta == 0){
    //         H = 0;
    //     }else if(max_RGB == R){ // max = R
    //         H = 60 * (G - B)/(delta);
    //     } else if(max_RGB == G){ // max = G
    //         H = 60 * (2 + (B - R)/(delta));
    //     } else { // max = B
    //         H = 60 * (4 + (R - G)/(delta));
    //     }

    //     if(H < 0)
    //         H = H + 360;

    //     if(max_RGB == 0)
    //         S = 0;
    //     else
    //         S = delta/max_RGB;

    //     V = max_RGB;

    //     return vec3(H, S, V);
    // }

//     vec3 rgb2hsv(vec3 c) {
//     vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
//     vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
//     vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

//     float d = q.x - min(q.w, q.y);
//     float e = 1.0e-10;
//     return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
// }

    vec3 hsv2rgb(vec3 c)
    {
        vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
        vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
        return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
    }


    vec4 get_result_color_gc(){
        // TODO: RGB -> HSV -> RGB
       float val = gaussian_curvature[0]; // gaussian_curvature is a vec3 composed by same value

       // colors in HSV
       vec3 red = vec3(0.0, 100.0, 100.0); //h s v
       vec3 green = vec3(120.0, 100.0, 100.0);
       vec3 blue = vec3(240.0, 100.0, 100.0);

       red = vec3(1, 0, 0);
       green = vec3(0, 1, 0);
       blue = vec3(0, 0, 1);

       if (val < 0) { //negative numbers until 0
            return vec4(interpolation(green, red, min(val/min_gc, 1.0)), 1.0);
        } else { //from 0 to positive
            return vec4(interpolation(green, blue, min(val/max_gc, 1.0)), 1.0);
        }
    }


    void main() {
        vec3 pos = vec3(model * vec4(aPos, 1.0));

        color = get_result_color_gc(); //vertex color obtained using gaussian curvature

        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }

