#version 330 core
out vec4 FragColor;
in vec3 Coords;
void main() 
{ 
    FragColor = vec4(Coords, 1.0f);
} 

/**
    Classic color interpolation - Barycentric coordinates
*/