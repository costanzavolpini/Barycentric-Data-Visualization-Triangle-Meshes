#ifndef BASE_H
#define BASE_H

/***************************************************************************
Base.h
Comment:  This file contains all basic definitions.
***************************************************************************/

#if defined (__APPLE_CC__)
#include <OpenGL/gl3.h>
#else
#include <GL/gl3.h>       /* assert OpenGL 3.2 core profile available. */
#endif

// STL includes
#include <iostream>
#include <assert.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <float.h>
#include <queue>
#include <set>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

#include "glad.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_inverse.hpp>
 

#include "Point3.h"


#endif
