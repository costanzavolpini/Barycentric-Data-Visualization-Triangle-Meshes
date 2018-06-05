#version 330 core

in vec3 coords;
in vec4 wedge_color[3]; // an array of 3 vectors of size 4 (since it is a triangle)
out vec4 fragColor;

void main()
{
    fragColor = (wedge_color[0] + wedge_color[1] + wedge_color[2])/3.0; // barycenter
}

/**
*    Flat shading triangle
*/