#ifndef ARCBALL_H
#define ARCBALL_H

// #ifdef __APPLE__
// #include <OpenGL/OpenGL.h>
// #endif
// #ifdef __linux__
// #include <GL/gl.h>
// #include <GL/glu.h>
// #endif

// #ifdef _WIN32
// #ifndef NOMINMAX
// #define NOMINMAX
// #endif
// #include <windows.h>
// #include <gl\gl.h>            // Header File For The OpenGL32 Library
// #include <gl\glu.h>            // Header File For The GLu32 Library
// #endif

#include <Eigen/Core>
#include <Eigen/Geometry>
#include "Base.h"

using namespace Eigen;

/***************************************************************************
Arcball.h
Comment:  Arcball is a method to manipulate and rotate objects in 3D intuitively. 
Reference: https://braintrekking.wordpress.com/2012/08/21/tutorial-of-arcball-without-quaternions/

ArcBall Algorithm made easy
*  - Detect the left-button of the mouse being depressed.
*  - Keep track of the last known mouse position.
*  - Treat the mouse position as the projection of a point on the hemi-sphere down to the image plane (along the z-axis), and determine that point on the hemi-sphere.
*  - Detect the mouse movement
*  - Determine the great circle connecting the old mouse-hemi-sphere point to the current mouse-hemi-sphere point.
*  - Calculate the normal to this plane. This will be the axis about which to rotate.
*  - Set the OpenGL state to modify the MODELVIEW matrix.
*  - Read off the current matrix, since we want this operation to be the last transformation, not the first, and OpenGL does things LIFO.
*  - Reset the model-view matrix to the identity
*  - Rotate about the axis
*  - Multiply the resulting matrix by the saved matrix.
*  - Force a redraw of the scene.
***************************************************************************/


