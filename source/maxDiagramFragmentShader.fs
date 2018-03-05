#version 330 core
out vec4 FragColor;
in vec3 Coords;
void main()
{
        if (Coords.x > Coords.y && Coords.x > Coords.z)
            FragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f); // blue
        else if(Coords.y > Coords.x && Coords.y > Coords.z)
            FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f); // green
        else
            FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f); // red
}

//ref: https://www.redblobgames.com/x/1730-terrain-shader-experiments/
/**
    MAX DIAGRAM (Nearest-Neighbor method) - put colour closest to a vertex
    the region around a vertex is defined
    by connecting the triangle centres and edge midpoints of the triangles
    and edges adjacent to a vertex.
*/