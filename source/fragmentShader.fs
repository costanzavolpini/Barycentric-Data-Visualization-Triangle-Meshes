#version 330 core
out vec4 fragColor;
in vec3 resColor;

void main()
{
   fragColor = vec4(resColor, 1.0);
}