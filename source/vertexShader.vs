#version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;

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


    // get specular color at current Pos
    vec3 get_specular(vec3 pos, vec3 normal, vec3 lightDir) {
        // get directional vector to the camera from pos
        vec3 viewDir = normalize(viewPos - pos); //view direction

        // specular shading
        vec3 reflectDir = - normalize(reflect(lightDir, normal)); //reflection
        float specular_intensity = pow(max(dot(reflectDir, viewDir), 0.0), shininess);

        // get resulting colour
        return light.specular * specular_intensity;
    }


    // return diffuse at current Pos
    vec3 get_diffuse(float lambert_term) {
        // get resulting colour
        return light.diffuse * lambert_term;
    }

    vec4 get_result_color_lighting(vec3 pos, vec3 normal, vec3 light_position) {
        vec3 lightDir = normalize(light_position - pos); //light direction
        float diffuse_intensity = max(dot(lightDir, normal), 0.0);

        vec3 ambient = light.ambient;
        vec3 diffuse = get_diffuse(diffuse_intensity);

        if(diffuse_intensity > 0.0){
            vec3 specular = get_specular(pos, normal, lightDir);
            return vec4(ambient + diffuse + specular, 1.0);
        }

        return vec4(ambient + diffuse, 1.0);
    }


    void main() {
        vec3 world_pos = vec3(model * vec4(aPos, 1.0));
        vec3 world_normal = mat3(transpose(inverse(model))) * aNormal;

        // vec3 light_pos = vec3(world_normal * light.position);
        vec3 light_pos = light.position;

        vertex_color = get_result_color_lighting(world_pos, world_normal, light_pos); // color obtained with lighting calculations

        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }

