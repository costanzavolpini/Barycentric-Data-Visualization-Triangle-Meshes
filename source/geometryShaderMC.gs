#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in COLOR_OUT {
    vec4 color_1;
    vec4 color_2;
    vec4 color_3;
} gs_in[];

out vec3 coords;
out vec4 wedge_color[3]; // an array of 3 vectors of size 4 (since it is a triangle)

void main()
{
	wedge_color[0] = gs_in[0].color_1;
	wedge_color[1] = gs_in[0].color_2;
	wedge_color[2] = gs_in[0].color_3;

	coords = vec3(1.0, 0.0, 0.0);
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

	coords = vec3(0.0, 1.0, 0.0);
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

	coords = vec3(0.0, 0.0, 1.0);
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}