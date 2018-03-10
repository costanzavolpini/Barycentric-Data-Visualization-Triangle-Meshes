/*
 *  Color.h
 *  QImageViewer
 *
 */

#ifndef COLOR_H
#define COLOR_H

/***************************************************************************
Color.h
Comment:  This file contains all Color definitions.
***************************************************************************/

//-----------------------------------------------------------------------------
/// Template class for colors with RGB-components.
/**
 * This is the main class for colors.
 * The type of the three components is variable.
 */
class Color3d {

    //-----------------------------------------------------------------------------
public:

    /**
     * Standard constructor. Color will be set to black.
     */
    Color3d()
        : _r( 0.0 ), _g( 0.0 ), _b( 0.0 ) {}

    /**
     * Constructor with given value that will be set to all components.
     * @param v - the value
     */
    Color3d( const double v )
        : _r( v ), _g( v ), _b( v ) {}

    /**
     * Constructor with given values for all 3 components.
     * @param r - red value
     * @param g - green value
     * @param b - blue value
     */
    Color3d( const double r, const double g, const double b )
        : _r( r ), _g( g ), _b( b ) {}

    /**
     * Returns the red value of this color.
     * @return the \b first coordinate
     */
    double& r() { return _r; }

    /**
     * Returns the red value of this color (constant).
     * @return the \b first coordinate
     */
    double r() const { return _r; }

    /**
     * Returns the green value of this color.
     * @return the \b second coordinate
     */
    double& g() { return _g; }

    /**
     * Returns the green value of this color (constant).
     * @return the \b second coordinate
     */
    double g() const { return _g; }

    /**
     * Returns the blue value of this color.
     * @return the \b third coordinate
     */
    double& b() { return _b; }

    /**
     * Returns the blue value of this color (constant).
     * @return the \b third coordinate
     */
    double b() const { return _b; }

    /**
     * Operator that returns the component at the given index.
     * @param i - index of the component
     * @return the \b component at index \em i
     */
    double& operator [] ( const int i ) {
        assert( i < 3 );
        if ( i == 0 )
            return _r;
        if ( i == 1 )
            return _g;

        return _b;
    }

    /**
     * Operator that returns the component at the given index (constant).
     * @param i - index of the component
     * @return the \b component at index \em i
     */
    double operator [] ( const int i ) const {
        assert( i < 3 );
        if ( i == 0 )
            return _r;
        if ( i == 1 )
            return _g;

        return _b;
    }

    /**
     * Equality operator based on the components of the color.
     * @param p - color to compare with
     * @return \b true if this color is equal to p, else \b false
     */
    bool operator == ( const Color3d& p ) const {
        if ( _r == p.r() && _g == p.g() && _b == p.b() )
            return true;
        return false;
    }

    /**
     * Adding Operator.
     * @param p - the addend
     * @return the \b sum of the colors
     */
    const Color3d operator + ( const Color3d& p ) const {
        return Color3d( _r + p.r(), _g + p.g(), _b + p.b() );
    }

    /**
     * Add a color to this one.
     * @param p - the addend
     */
    void operator += ( const Color3d& p ) {
        _r += p.r(); _g += p.g(); _b += p.b();
    }

    /**
     * Multiply operator for a single value.
     * All components will be multiplied with the given value.
     * @param w - the multiplier
     * @return the <b> new point </b>
     */
    const Color3d operator * ( const double w ) const {
        return Color3d( _r * w, _g * w, _b * w );
    }

    /**
     * Multiply operator for a two colours.
     * All components will be multiplied with the given color.
     * @param p - the multiplier
     * @return the <b> new point </b>
     */
    const Color3d operator * ( const Color3d& p ) const {
        return Color3d( _r * p.r(), _g * p.g(), _b * p.b() );
    }

    /**
     * Multiply operator for a single value.
     * All components will be multiplied with the given value.
     * @param w - the multiplier
     * @return the <b> new point </b>
     */
    friend const Color3d operator * ( const double w, const Color3d& p ) {
        return p * w;
    }

    /**
     * Multiply all components of this color with the given value.
     * @param w - the multiplier
     */
    void operator *= ( const double w ) {
        _r *= w; _g *= w; _b *= w;
    }

    /**
     * Multiply all components of this color with the given value.
     * @param w - the multiplier
     */
    void operator *= ( const Color3d& p ) {
        _r *= p.r(); _g *= p.g(); _b *= p.b();
    }

    /**
     * Returns the clamped color.
     * @return the \b clamped color
     */
    void clamped() {
        if (_r < 0.0) _r = 0.0;
        if (_r > 1.0) _r = 1.0;
        if (_g < 0.0) _g = 0.0;
        if (_g > 1.0) _g = 1.0;
        if (_b < 0.0) _b = 0.0;
        if (_b > 1.0) _b = 1.0;
    }

    inline bool isWhite() { return (_r+_g+_b) >=3; }

    /**
     * Set a random colour.
     * Warning: no srand() init is done!
     */
    static const Color3d random()
    {
        const int rmax = RAND_MAX;

        return Color3d(
                    (double) ( rand() ) / rmax,
                    (double) ( rand() ) / rmax,
                    (double) ( rand() ) / rmax
                    );
    }

    //-----------------------------------------------------------------------------
private:

    // red, green, and blue
    double _r, _g, _b;

};

#endif
