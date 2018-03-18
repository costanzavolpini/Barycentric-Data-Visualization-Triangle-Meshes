#version 330 core
in vec4 vertex_color[3];
layout (points, max_vertices = 3) out;

// Geometry Shader
out VertexAttrib
{
  vec3 coords;
  vec4 wedge_color[3];
} outData;


void main()
{
	outData.wedge_color[0] = vertex_color[0];
	outData.wedge_color[1] = vertex_color[1];
	outData.wedge_color[2] = vertex_color[2];

	outData.coords = vec3(1.0, 0.0, 0.0);
    gl_Position = vertex_color[0];
    EmitVertex();

	outData.coords = vec3(0.0, 1.0, 0.0);
    gl_Position = vertex_color[1];
    EmitVertex();

	outData.coords = vec3(0.0, 0.0, 1.0);
    gl_Position = vertex_color[2];
    EmitVertex();

    EndPrimitive();
}