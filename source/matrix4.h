#ifndef MATRIX4_H
#define MATRIX4_H

#include "Point3.h"

/***************************************************************************
MATRIX.h
Comment:  This file contains all matrix definitions.
***************************************************************************/

//-----------------------------------------------------------------------------
/// Template class for 4x4-Matrices.
/**
* This is the main class for all 4x4-Matrices.
* The type of the 16 components is variable.
*/
class Matrix4d {

    //-----------------------------------------------------------------------------
public:

    /**
    * Standard constructor. Matrix and inverse Matrix will be set to identity.
    */
    Matrix4d()
    {
        for(int r=0; r<4; r++) {
            for(int c=0; c<4; c++) {
                if (r==c) {
                    _v[r][c] = 1.0;
                    _inv[r][c] = 1.0;
                }
                else {
                    _v[r][c] = 0.0;
                    _inv[r][c] = 0.0;
                }
            }
        }
    }

    /*
    * write,read the matrix element at i,j:
    * element of the matrix when no third parameter is specified
    * the element at [i,j] in the inverse matrix otherwise
    */
    double& operator() (int i, int j, int invFlag = 0)
    {
        if (invFlag == 0)
            return _v[i][j];
        else
            return _inv[i][j];
    }

    /*
    * write,read the matrix element at i,j:
    * element of the matrix when no third parameter is specified
    * the element at [i,j] in the inverse matrix otherwise
    */
    const double& operator() (int i, int j, int invFlag = 0) const
    {
        if (invFlag == 0)
            return _v[i][j];
        else
            return _inv[i][j];
    }

    /*
    * Constructor for a translation matrix:
    */
    static Matrix4d translation( const double x, const double y, const double z)
    {
        Matrix4d M;
        // set the last column of the matrix
        M(0,3) = x;
        M(1,3) = y;
        M(2,3) = z;

        // set the last column of the inverse matrix
        M(0,3,-1) = -x;
        M(1,3,-1) = -y;
        M(2,3,-1) = -z;
        return M;
    }

    /*
    * Constructor for a rotation matrix:
    */
    static Matrix4d rotation( const double angle, const char axis)
    {
        Matrix4d M;

        // set rotation around x axis:
        if ((axis == 'x') || (axis == 'X')) {
            M(1,1) =  cos(angle);   M(1,1,-1) =  cos(angle);
            M(1,2) = -sin(angle);   M(1,2,-1) =  sin(angle);
            M(2,1) =  sin(angle);   M(2,1,-1) = -sin(angle);
            M(2,2) =  cos(angle);   M(2,2,-1) =  cos(angle);
            return M;
        }

        // set rotation around y axis:
        if ((axis == 'y') || (axis == 'Y')) {
            M(0,0) =  cos(angle);   M(0,0,-1) =  cos(angle);
            M(0,2) =  sin(angle);   M(0,2,-1) = -sin(angle);
            M(2,0) = -sin(angle);   M(2,0,-1) =  sin(angle);
            M(2,2) =  cos(angle);   M(2,2,-1) =  cos(angle);
            return M;
        }

        // set rotation around z axis:
        if ((axis == 'z') || (axis == 'Z')) {
            M(0,0) =  cos(angle);   M(0,0,-1) =  cos(angle);
            M(0,1) = -sin(angle);   M(0,1,-1) =  sin(angle);
            M(1,0) =  sin(angle);   M(1,0,-1) = -sin(angle);
            M(1,1) =  cos(angle);   M(1,1,-1) =  cos(angle);
            return M;
        }

        return M;
    }

    /*
    * Constructor for a scaling matrix:
    */
    static Matrix4d scaling( const double x, const double y, const double z)
    {
        Matrix4d M;
        // set the diagonal of the matrix
        M(0,0) = x;
        M(1,1) = y;
        M(2,2) = z;
        // set the diagonal of the inverse matrix
        M(0,0,-1) = 1/x;
        M(1,1,-1) = 1/y;
        M(2,2,-1) = 1/z;
        return M;
    }

    /*
    * Constructor for a projection matrix:
    */
    static Matrix4d projection( const double X, const double Y, const double s)
    {
        Matrix4d M;

        M(0,0) =  1.0;        M(0,0,-1) =  s;
        M(1,1) = -1.0;        M(1,1,-1) = -s;
        M(2,2) =  0.0;        M(2,2,-1) = 0.0;
        M(3,3) =  0.0;        M(3,3,-1) = 0.0;

        M(0,2) = X - 0.5*s;   M(0,3,-1) = -X + 0.5*s;
        M(1,2) = Y - 0.5*s;   M(1,3,-1) =  Y - 0.5*s;
        M(3,2) = s;           M(2,3,-1) = 1.0;
        M(2,3) = s;           M(3,2,-1) = 1.0;

        return M;
    }

    /*
    * inverts the current matrix:
    */
    void invert()
    {
        for (int r=0; r<4; r++)
            for (int c=0; c<4; c++) {
                double tmp = _v[r][c];
                _v[r][c]   = _inv[r][c];
                _inv[r][c] = tmp;
            }
    }

    /*
    * returns the invers of the current matrix:
    */
    Matrix4d inverse()
    {
        Matrix4d M;
        for (int r=0; r<4; r++)
            for (int c=0; c<4; c++) {
                M(r,c)    = _inv[r][c];
                M(r,c,-1) = _v[r][c];
            }
        return M;
    }

    /*
    * transpose the current matrix:
    */
    void transpose()
    {
        for (int r=0; r<2; r++)
            for (int c=r+1; c<3; c++) {
                double tmp = _v[r][c];
                _v[r][c] = _v[c][r];
                _v[c][r] = tmp;
                tmp = _inv[r][c];
                _inv[r][c] = _inv[c][r];
                _inv[c][r] = tmp;
            }
    }

  /**
  * Multiplication of two matrices.
  * @param m - the other Matrix
  * @return the <b> product </b> of the matrix multiplication
  */
    const Matrix4d operator * ( const Matrix4d& m ) const {
        Matrix4d M;
        // set matrix diagonals to 0
        for (int r=0; r<4; r++) { M(r,r) = 0; M(r,r,-1) = 0;}
        // Now multiply the two matrices:
        for (int r=0; r<4; r++)
            for (int c=0; c<4; c++)
                for (int i=0; i<4; i++) {
                    M(r,c)    += _v[r][i]*m(i,c);
                    M(r,c,-1) += m(r,i,-1)*_inv[i][c];
                }
        return M;
    }

  /**
  * Equality operator based on the elements of the matrix.
  * @param m - matrix to compare with
  * @return \b true if the matrices are equal, else \b false
  */
    bool operator == ( Matrix4d& m ) const {
        for (int r=0; r<4; r++)
            for (int c=0; c<4; c++)
                if (_v[r][c] != m(r,c))
                    return false;

        return true;
    }

  /**
  * Multiplication with a direction.
  * @param d - direction
  * @return the <b> transformed direction </b>
  */
    Point3d operator % ( const Point3d& d ) const {
        double x,y,z;
        x = _v[0][0]*d.x() + _v[0][1]*d.y() + _v[0][2]*d.z();
        y = _v[1][0]*d.x() + _v[1][1]*d.y() + _v[1][2]*d.z();
        z = _v[2][0]*d.x() + _v[2][1]*d.y() + _v[2][2]*d.z();

        return Point3d( x, y, z );
    }

    /**
  * Multiplication with a point.
  * @param p - point
  * @return the <b> transformed point </b>
  */
    Point3d operator * ( const Point3d& p ) const {
        double x,y,z,w;
        x = _v[0][0]*p.x() + _v[0][1]*p.y() + _v[0][2]*p.z() + _v[0][3];
        y = _v[1][0]*p.x() + _v[1][1]*p.y() + _v[1][2]*p.z() + _v[1][3];
        z = _v[2][0]*p.x() + _v[2][1]*p.y() + _v[2][2]*p.z() + _v[2][3];
        w = _v[3][0]*p.x() + _v[3][1]*p.y() + _v[3][2]*p.z() + _v[3][3];

        return Point3d( x/w, y/w, z/w );
    }

//-----------------------------------------------------------------------------
private:

    /*
    * The components of the Matrix in _v[][]
    * The components of the inverse Matrix in _inv[][]
    */
    double _v[4][4];
    double _inv[4][4];
};

#endif
