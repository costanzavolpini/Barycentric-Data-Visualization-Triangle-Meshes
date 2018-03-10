#ifndef LIGHT_H
#define LIGHT_H
#include "Color.h"
#include "Structs.h"

/***************************************************************************
Light.h
Comment:  This file contains all Light definitions.
***************************************************************************/

class Light {

//-----------------------------------------------------------------------------
public:

    // default constructor for the light source: sets position and color of the light to (0,0,0)
    Light()
        : isDirectional(false), isSpot(false), cosThetaL(0.0), fallOffCoeff(0.0) {
        pos = Point3d(0.0, 0.0, 0.0);
        dir = Point3d(0.0, 0.0, 0.0);
        col.ambient  = Color3d(0.0, 0.0, 0.0);
        col.diffuse  = Color3d(0.0, 0.0, 0.0);
        col.specular = Color3d(0.0, 0.0, 0.0);
        col.shininess = 0.0;
    }

    // constructor for the light source: sets position and color of the light
    Light(Point3d p, Material c)
        : isDirectional(false), isSpot(false), cosThetaL(0.0), fallOffCoeff(0.0) {
        pos = p;
        col = c;
    }

    // set the light position
    void setPosition(Point3d p) {
        pos = p;
    }

    // get the light position
    Point3d getPosition() {
        return pos;
    }

    // set the lightsource to a directional light
    void setDirectional() {
        isDirectional = true;
        isSpot = false;
        pos.normalize();
    }

    // set the lightsource to a spot light
    void setSpotLight(Point3d d, double theta, double K) {
        isSpot = true;
        isDirectional = false;
        dir = d;
        dir.normalize();
        cosThetaL = cos(theta);
        fallOffCoeff = K;
    }

    // Returns the color of the light source for a certain point.
    // The returned color equals the light color, except for a spot light
    // where the point gets only the ambient term of the light if it
    // is not within the opening angle of the spot light.
    const Material getColor(Point3d) {
        if (!isSpot)
            // light is not a spot light, so return full light color
            return col;

        //TODO add spotlight
        return col;
    }

    // returns the (normalized) vector from point p to the light source
    Point3d getLightVector(Point3d p) {
        if (isDirectional)
            return pos;
        else
            return (pos-p).normalized();
    }

    //-----------------------------------------------------------------------------

private:

    // light colour
    Material col;

    // light position
    Point3d pos;

// flags and additional data for special kinds of light sources
public:
    bool isDirectional;
    bool isSpot;

private:
    Point3d dir;         // direction of a spot light
    double cosThetaL;    // cos of a spot light's opening angle
    double fallOffCoeff; // the 'K' exponent for a spot light

};

#endif
