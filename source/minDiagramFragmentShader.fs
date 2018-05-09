#version 330 core
in vec3 coords;
in vec4 wedge_color[3]; // an array of 3 vectors of size 4 (since it is a triangle)
out vec4 fragColor;

void main()
{
	if (coords[0] < coords[1]) {
		if (coords[0] < coords[2]) {
			fragColor = wedge_color[0];
		}
		else {
			fragColor = wedge_color[2];
		}
	}
	else {
		if (coords[1] < coords[2]) {
			fragColor = wedge_color[1];
		}
		else {
			fragColor = wedge_color[2];
		}
	}
}


// ref: https://www.redblobgames.com/x/1730-terrain-shader-experiments/
/**
    MIN DIAGRAM - colour on the points closest to an edge
    the region around an edge is
    defined by connecting the endpoints of the edge with the triangle
    centres of the adjacent triangles.
*/