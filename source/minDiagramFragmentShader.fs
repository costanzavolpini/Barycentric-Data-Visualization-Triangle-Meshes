#version 330 core
// out vec4 FragColor;
// in vec3 Coords;
// void main()
// {
//         if (Coords.x < Coords.y && Coords.x < Coords.z)
//             FragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f); // blue
//         else if(Coords.y < Coords.x && Coords.y < Coords.z)
//             FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f); // green
//         else
//             FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f); // red
// }
in vec3 coords;
in vec4 wedge_color[3]; // an array of 3 vectors of size 4 (since it is a triangle)
out vec4 fragColor;

void main()
{
	if (coords[0] < coords[1]) {
		if (coords[0] < coords[2]) {
            // fragColor = vec4(1.0, 0.0, 0.0, 1.0);
			fragColor = wedge_color[0];
		}
		else {
            // fragColor = vec4(0.0, 0.0, 1.0, 1.0);
			fragColor = wedge_color[2];
		}
	}
	else {
		if (coords[1] < coords[2]) {
            // fragColor = vec4(0.0, 1.0, 0.0, 1.0);
			fragColor = wedge_color[1];
		}
		else {
            // fragColor = vec4(0.0, 0.0, 1.0, 1.0);
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