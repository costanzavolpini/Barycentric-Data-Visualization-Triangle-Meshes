#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 world_position[3];
out vec3 triangle_normal;
out vec3 world_position_triangle[3];

uniform mat4 model;

void main()
{
    world_position_triangle[0] = world_position[0];
	world_position_triangle[1] = world_position[1];
	world_position_triangle[2] = world_position[2];

    triangle_normal = normalize (cross ((world_position[1] - world_position[0]), (world_position[2] - world_position[0])));

    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}