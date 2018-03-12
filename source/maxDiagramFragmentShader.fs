#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 Coords;

uniform vec3 lightPos; 
uniform vec3 lightColor;

void main()
{
        // ambient
        float ambientStrength = 0.1;
        vec3 ambient = ambientStrength * lightColor;
        
        // diffuse 
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - Coords);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;

        if (Coords.x > Coords.y && Coords.x > Coords.z) {
            vec3 result = (ambient + diffuse) * vec3(0.0f, 0.0f, 1.0f); // blue
            FragColor = vec4(result, 1.0f);
        } else if(Coords.y > Coords.x && Coords.y > Coords.z) {
            vec3 result = (ambient + diffuse) * vec3(0.0f, 1.0f, 0.0f); //green
            FragColor = vec4(result, 1.0f);
        } else {
            vec3 result = (ambient + diffuse) * vec3(1.0f, 0.0f, 0.0f); // red
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