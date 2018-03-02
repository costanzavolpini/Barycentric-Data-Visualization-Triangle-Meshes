#version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aCoords;
    out vec3 Coords;
    void main()
    {
       gl_Position = vec4(aPos, 1.0);
       Coords = aCoords;
    }

//create a vec3 called aPos
//cast this to a vector of size 4