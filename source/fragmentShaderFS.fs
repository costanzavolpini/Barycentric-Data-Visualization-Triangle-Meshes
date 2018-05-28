#version 330 core

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
 };

uniform vec3 view_position;
uniform Light light;
uniform float shininess;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vec3 world_position_triangle[3];
in vec3 triangle_normal;
out vec4 color;

// get specular color at current Pos
    vec3 get_specular(vec3 pos, vec3 normal, vec3 light_direction) {
        // get directional vector to the camera from pos
        vec3 view_direction = normalize(view_position - pos); //view direction

        // specular shading
        vec3 reflect_direction = - normalize(reflect(light_direction, normal)); //reflection
        float specular_intensity = pow(max(dot(reflect_direction, view_direction), 0.0), shininess);

        // get resulting colour
        return light.specular * specular_intensity;
    }


    // return diffuse at current Pos
    vec3 get_diffuse(float lambert_term) {
        // get resulting colour
        return light.diffuse * lambert_term;
    }

    vec4 get_result_color_lighting(vec3 pos, vec3 normal, vec3 light_position) {
        vec3 light_direction = normalize(light_position - pos); //light direction
        float diffuse_intensity = max(dot(light_direction, normal), 0.0);

        vec3 ambient = light.ambient;
        vec3 diffuse = get_diffuse(diffuse_intensity);

        if(diffuse_intensity > 0.0001){
            vec3 specular = get_specular(pos, normal, light_direction);
            return vec4((ambient + diffuse + specular), 1.0);
        }

        return vec4((ambient + diffuse) , 1.0);
    }



void main()
{
    vec3 barycenter = (world_position_triangle[0] + world_position_triangle[1] + world_position_triangle[2])/3;
    vec3 world_position = vec3(model * vec4(barycenter, 1.0));
    vec3 world_normal = mat3(transpose(inverse(model))) * triangle_normal;
    vec3 light_pos = vec3(projection * vec4(light.position, 1.0));

    color = get_result_color_lighting(world_position, world_normal, light_pos); // color obtained with lighting calculations
}
