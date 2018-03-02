#version 330 core
out vec4 FragColor;
in vec3 Coords;
void main()
{
        if (Coords.x < Coords.y && Coords.x < Coords.z) 
            FragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f); // blue
        else if(Coords.y < Coords.x && Coords.y < Coords.z)
            FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f); // green
        else
            FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f); // red
}

// ref: https://www.redblobgames.com/x/1730-terrain-shader-experiments/
/**
    MIN DIAGRAM - colour on the points closest to an edge
    the region around an edge is
    defined by connecting the endpoints of the edge with the triangle
    centres of the adjacent triangles. 
*/