#version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;
    layout (location = 2) in vec3 aCoords;

    out vec3 Color;
    out vec3 Coords;
    out vec3 Normal;
    uniform mat4 transform;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
       gl_Position =  projection * view * transform * vec4(aPos, 1.0);
       Coords = aCoords;
       Normal = aNormal;  
    }

//create a vec3 called aPos
//cast this to a vector of size 4
