#ifndef STRUCTS_h
#define STRUCTS_h

//-----------------------------------------------------------------------------
/** Material structure that contains the four types of material coefficiants  *
  * Initilazes the material with a gray color																	*/
#include "Color.h"
#include "point3.h"

struct Material{
    Color3d ambient;
    Color3d diffuse;
    Color3d specular;
    double shininess;
    double reflectivity;
};

//-----------------------------------------------------------------------------
/** The ray structure contains the origin and direction of the ray						*/  
struct Ray{
    Point3d direction;
    Point3d origin;
};

//-----------------------------------------------------------------------------
/** Simple intersection structure. Contains not only the intersection point,  
 *  but although if there is an intersection after all and if the camera
 *  is within the object																											*/
struct IntersectionInfo{
    Point3d point;
    Point3d normal;
    double distance;
    bool intersect;
    bool inside;
};

#endif
