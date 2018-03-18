#version 330 core

in VertexAttrib
{
  vec3 coords;
  vec4 wedge_color[3];
} outData;
out vec4 fragColor;

void main()
{
	if (outData.coords[0] > outData.coords[1]) {
		if (outData.coords[0] > outData.coords[2]) {
			fragColor = vec4(1.0, 0.0, 0.0, 1.0);
			fragColor = outData.wedge_color[0];
		}
		else {
			fragColor = vec4(0.0, 0.0, 1.0, 1.0);
			fragColor = outData.wedge_color[2];
		}
	}
	else {
		if (outData.coords[1] > outData.coords[2]) {
			fragColor = vec4(0.0, 1.0, 0.0, 1.0);
			fragColor = outData.wedge_color[1];
		}
		else {
			fragColor = vec4(0.0, 0.0, 1.0, 1.0);
			fragColor = outData.wedge_color[2];
		}
	}
}

//ref: https://www.redblobgames.com/x/1730-terrain-shader-experiments/
/**
    MAX DIAGRAM (Nearest-Neighbor method) - put colour closest to a vertex
    the region around a vertex is defined
    by connecting the triangle centres and edge midpoints of the triangles
    and edges adjacent to a vertex.
*/