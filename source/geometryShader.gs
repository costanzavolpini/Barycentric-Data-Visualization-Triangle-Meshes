#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3 ) out; 

in vec4 vertex_color[3];
out vec3 coords;
out vec4 wedge_color[3];



void main()
{
	wedge_color[0] = vertex_color[0];
	wedge_color[1] = vertex_color[1];
	wedge_color[2] = vertex_color[2];

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