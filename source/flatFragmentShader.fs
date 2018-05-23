#version 330 core
out vec4 fragColor;
flat in vec4 color_flat;

void main()
{
    fragColor = color_flat;
}