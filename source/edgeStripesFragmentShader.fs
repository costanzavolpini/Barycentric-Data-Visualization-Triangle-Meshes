#version 330 core
out vec4 FragColor;
in vec3 Coords;
void main() 
{ 
    if (Coords.x < 0.333)
        FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    else if (Coords.x > 0.666)
        FragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
    else
        FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
} 

/**
    First version.
*/