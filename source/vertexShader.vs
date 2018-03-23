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

    vec4 get_result_color(vec3 pos, vec3 ambient, vec3 diffuse, vec3 specular) {
        // attenuation
        float distance = length(light.position - pos);
       // float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
        float attenuation = 1.0/ distance;

        //ambient *= attenuation;
        //diffuse *= attenuation;
        //specular *= attenuation;

        return vec4((ambient + diffuse + specular), 1.0);
    }

    void main() {
        vec3 pos = vec3(model * vec4(aPos, 1.0));
        vec3 normal = mat3(transpose(inverse(model))) * aNormal;  
    
       // vertex_color = normalize(get_result_color(pos, light.ambient, get_diffuse(pos, normal), get_specular(pos, normal)));

    vertex_color = vec4((pos + vec3(1.0,1.0,1.0))/2, 1.0f); 
    // vertex_color = vec4(pos, 1.0);

        if(vertex_color[0] >= 0.7){
            vertex_color = vec4(1.0, 0.0, 0.0, 1.0); //red
        }

        if(vertex_color[1] >= 0.5){
            vertex_color = vec4(0.0, 1.0, 0.0, 1.0); //green
        }

        if(vertex_color[2] >= 0.2){
            vertex_color = vec4(0.0, 0.0, 1.0, 1.0); //blue
        }
    
        gl_Position = projection * view * model * vec4(aPos, 1.0); 
    }
 
