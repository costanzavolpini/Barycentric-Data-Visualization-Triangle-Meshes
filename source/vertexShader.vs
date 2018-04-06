#version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal; 
    layout (location = 2) in vec3 gaussian_curvature; 

    struct Light {
        vec3 position;
    
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
    };

    out vec4 vertex_color;
    
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    uniform vec3 viewPos; 
    uniform Light light;
    uniform float shininess;
    uniform float min_gc;
    uniform float max_gc;

    // get specular color at current Pos
    vec3 get_specular(vec3 pos, vec3 normal) {
        // get directional vector to the light and to the camera from pos
        vec3 lightDir = normalize(light.position - pos);
        vec3 viewDir = normalize(viewPos - pos);

        // specular shading
        vec3 reflectDir = reflect(-lightDir, normal);
        float specular_intensity = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

        // get resulting colour
        vec3 specular = light.specular * specular_intensity;
    
        return specular;
    }


    // return diffuse at current Pos
    vec3 get_diffuse(vec3 pos, vec3 normal) {
        vec3 lightDir = normalize(light.position - pos);
        float diffuse_intensity = max(dot(normal, lightDir), 0.0); 

        // get resulting colour
        vec3 diffuse = light.diffuse * diffuse_intensity;

        return diffuse;
    }

    vec4 get_result_color_lighting(vec3 ambient, vec3 diffuse, vec3 specular) {
        return vec4(ambient + diffuse + specular, 1.0);
    }

    vec3 interpolation(vec3 v0, vec3 v1, float t) {
        return (1 - t) * v0 + t * v1;
    }

    vec4 get_result_color_gc(){
       float val = gaussian_curvature[0]; // gaussian_curvature is a vec3 composed by same value
       vec3 red = vec3(1.0, 0.0, 0.0);
       vec3 green = vec3(0.0, 1.0, 0.0);
       vec3 blue = vec3(0.0, 0.0, 1.0);

       if (val < 0) { //negative numbers until 0 -> map from red to green
            return vec4(interpolation(red, green, val/min_gc), 1.0);
        } else { //map from green to blue, from 0 to positive
            return vec4(interpolation(green, blue, val/max_gc), 1.0);
        }
    }

    void main() {
        vec3 pos = vec3(model * vec4(aPos, 1.0));
        vec3 normal = mat3(transpose(inverse(model))) * aNormal;  
    
        vertex_color = get_result_color_lighting(light.ambient, get_diffuse(pos, normal), get_specular(pos, normal)); // color obtained with lighting calculations
        // vertex_color = get_result_color_gc(); //color obtained using gaussian curvature

        gl_Position = projection * view * model * vec4(aPos, 1.0); 
    }
 
