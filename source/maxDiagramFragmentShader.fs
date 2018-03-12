#version 330 core
out vec4 FragColor;

struct Light {
    vec3 position;  
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
};

in vec3 Normal;  
in vec3 Coords;

uniform vec3 viewPos; 
uniform Light light;


void main()
{
        // ambient
        vec3 ambient = light.ambient;

        // diffuse 
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(light.position - Coords);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff;

        // specular
        vec3 viewDir = normalize(viewPos - Coords);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = light.specular * spec;

        // attenuation
        float distance    = length(light.position - Coords);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

        ambient *= attenuation;  
        diffuse *= attenuation;
        specular *= attenuation;   

        if (Coords.x > Coords.y && Coords.x > Coords.z) {
            vec3 result = (ambient + diffuse + specular) * vec3(0.0f, 0.0f, 1.0f); // blue
            FragColor = vec4(result, 1.0f);
        } else if(Coords.y > Coords.x && Coords.y > Coords.z) {
            vec3 result = (ambient + diffuse + specular) * vec3(0.0f, 1.0f, 0.0f); //green
            FragColor = vec4(result, 1.0f);
        } else {
            vec3 result = (ambient + diffuse + specular) * vec3(1.0f, 0.0f, 0.0f); // red
            FragColor = vec4(result, 1.0f);
        }
}

//ref: https://www.redblobgames.com/x/1730-terrain-shader-experiments/
/**
    MAX DIAGRAM (Nearest-Neighbor method) - put colour closest to a vertex
    the region around a vertex is defined
    by connecting the triangle centres and edge midpoints of the triangles
    and edges adjacent to a vertex.
*/