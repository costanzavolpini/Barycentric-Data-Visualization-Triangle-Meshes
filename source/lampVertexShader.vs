#version 330 core
layout (location = 0) in vec3 Coords;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 transform;


void main()
{
	gl_Position = projection * view * transform * vec4(Coords, 1.0);
}