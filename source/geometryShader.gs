#version 330 core
in vec4 vertex_color[3];
layout (points, max_vertices=3) out;

out vec3 coords;
out vec4 wedge_color[3];

void main()
{
	wedge_color[0] = vertex_color[0];
	wedge_color[1] = vertex_color[1];
	wedge_color[2] = vertex_color[2];

	coords = vec3(1.0, 0.0, 0.0);
    gl_Position = vertex_color[0];
    EmitVertex();

	coords = vec3(0.0, 1.0, 0.0);
    gl_Position = vertex_color[1];
    EmitVertex();

	coords = vec3(0.0, 0.0, 1.0);
    gl_Position = vertex_color[2];
    EmitVertex();

    EndPrimitive();
}