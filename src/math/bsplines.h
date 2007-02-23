/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 Header:                    bsplines.h
 Author(fortran edition):   JONES, R. E., (SNLA)
                            BOISVERT, RONALD, NBS
 Transformed and Wraped By: Matthew Gong
 Date started:              09/27/2006

 ------ Copyright (C) 2006 Matthew Gong (matthewzhenggong@gmail.com) ------

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA  02111-1307, USA.

 Further information about the GNU General Public License can also be found on
 the world wide web at http://www.gnu.org.

HISTORY
--------------------------------------------------------------------------------
06/27/2006   Matt   Created

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
SENTRY
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#ifndef BSPLINES_H
#define BSPLINES_H

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include <vector>

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
DEFINITIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

  /**
   * @typedef Value List.
   *
   * Use std::vector to maintain the value list.
   */
  typedef std::vector < double > TLIST;


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
FORWARD DECLARATIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DOCUMENTATION

 Purpose: Provide the convenient B-Spline interpolation functions and classes.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DECLARATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/** 1 dimention B-Spline interpolation function and class
  * 
  * example :
  *    double x[]={1,2,3,5,7};
  *    double y[]={1,3,2,4,7};
  *    BSpline1D bs(x, 5, y, 3);
  *    if (bs.ok())
  *    {
  *      double x0, y0;
  *      for (x0=0; x0 <= 8; x0 += 0.1)
  *      {
  *        y0 = bs.evaluate(x0);
  *        printf("%g\t%g\n", x0, y0);
  *      }
  *    }
  *    else
  *    {
  *      printf("fail to init this B-Spline.\n");
  *    }
  * 
  */
class BSpline1D
{
public:

  /**
   * Construct a spline domain for the given set of x & y values,
   * The parameters are the same as for setDomain().
   * Call ok() to check whether domain setup succeeded after construction.
   *
   * Create a single spline with the parameters required to set up
   * the domain and subsequently smooth the given set of y values.
   * The y values must correspond to each of the values in the x array.
   * If either the domain setup fails or the spline cannot be solved,
   * the state will be set to not ok.
   *
   * @see ok().
   *
   * @param x		The array of x values in the domain.
   * @param nx	The number of values in the @p x array.
   * @param y		The array of y values corresponding to each of the
   *			nX() x values in the domain.
   */
  BSpline1D( const double * x, size_t nx, const double * y, const int & order = 3 );


  /**
   * Construct a spline domain for the given set of x & y values,
   * Call ok() to check whether domain setup succeeded after construction.
   *
   * Create a single spline with the parameters required to set up
   * the domain and subsequently smooth the given set of y values.
   * The y values must correspond to each of the values in the x array.
   * If either the domain setup fails or the spline cannot be solved,
   * the state will be set to not ok.
   *
   * @see ok().
   *
   * @param x		The vector of x values in the domain.
   * @param y		The vector of y values corresponding to each of 
   *			x values in the domain.
   */
  BSpline1D( const TLIST &x, const TLIST &y, const int & order = 3 );

  /**
   * The default construct a spline domain.
   * Call setDomain() later to setup B-spline.
   *
   * the state will be set to not ok.
   *
   * @see setDomain(const double * x, size_t nx, const double * y).
   */
  BSpline1D() : OK( false ), ErrNo(-1)
  {}

  /**
   * Change the domain of this base.  
   * The x values must be in ascending order.  The setup of
   * the domain may fail because  the resulting matrix
   * could not be factored.  If setup fails, the method returns false.
   *
   * @param x		The array of x values in the domain.
   * @param nx	The number of values in the @p x array.
   * @param y		The array of y values in the domain.
   *
   * see ok().
   */
  bool setDomain( const double * x, size_t nx, const double * y, const int & order = 3 );

  /**
   * Change the domain of this base.  
   * The x values must be in ascending order.  The setup of
   * the domain may fail because  the resulting matrix
   * could not be factored.  If setup fails, the method returns false.
   *
   * @param x		The array of x values in the domain.
   * @param y		The array of y values in the domain.
   *
   * see ok().
   */
  bool setDomain( const TLIST &x, const TLIST &y, const int & order = 3 );

  /**
   * Change the domain of this base after changing the nodes' pos.  
   * The setup of the domain may fail because  the resulting matrix
   * could not be factored.  If setup fails, the method returns false.
   */
  bool setDomain( const int & order = 3 );

  /**
   * Return the evaluation of the sample curve 
   * at a particular @p x value.  If current state is not ok(), returns 0.
   * for big number of evaluation. for speed.
   *
   * @param x	 evaluate at the @p x value.
   */
  double evaluate( double x, int ideriv = 0 );

  /**
   * Return the current state of the object, either ok or not ok.
   * Use this method to test for valid state after construction or after
   * a call to setDomain().  ok() will return false if either fail, such
   * as when an appropriate number of nodes and node interval cannot be
   * found for a given wavelength, or when the linear equation for the
   * coefficients cannot be solved.
   */
  bool ok() const
  {
    return OK;
  }

  /**
   * Return the number of node list.
   */
  size_t nNode() const
  {
    return node_x_list.size();
  }

  /**
   * Return the x value at i-th point in the node list.
   * The i-th point should be in the node list.
   *
   * @param i	 x value at the @p i-th node.
   */
  double xNode( size_t i ) const
  {
    return node_x_list[i];
  }

  /**
   * Set the x value at i-th point in the node list.
   * The i-th point should be in the node list.
   * Need to setDomain again.
   *
   * @param i	 x value at the @p i-th node.
   */
  double xNode( size_t i, double x )
  {
    double tmp = node_x_list[i];

    node_x_list[i] = x;
    OK = false;
    return tmp;
  }

  /**
   * Return the y value at i-th point in the node list.
   * The i-th point should be in the node list.
   *
   * @param i	 y value at the @p i-th node.
   */
  double yNode( size_t i ) const
  {
    return node_y_list[i];
  }

  /**
   * Set the y value at i-th point in the node list.
   * The i-th point should be in the node list.
   * Need to setDomain again.
   *
   * @param i	 y value at the @p i-th node.
   */
  double yNode( size_t i, double y )
  {
    double tmp = node_y_list[i];

    node_y_list[i] = y;
    OK = false;
    return tmp;
  }

  int ReturnCode(void) const;

protected:

  /**
    * Node X list.
    *
    * Maintain the input node x list.
    */
  TLIST node_x_list;

  /**
    * Node Y list.
    *
    * Maintain the input node y list.
    */
  TLIST node_y_list;

  /**
    * knots list.
    *
    * Maintain the knots list.
    */
  TLIST knots_list;

  /**
    * B-Spline coeffient list.
    *
    * Maintain the B-Spline coeffient list.
    */
  TLIST bcoef_list;

  /**
    * State flag.
    *
    * Maintain the state for the B-spline.
    * 
    * @see ok().
    */
  bool OK;

  /// number
  int n;

  /// order
  int k;

  /// iterator for search the b-spline value
  int inbv;

  /// work space
  TLIST work_list;

  TLIST q_list;
  int ErrNo;
};

/** 2 dimention B-Spline interpolation function and class
  * 
  * example :
  *     double x1[]={1, 2, 3, 5, 7};
  *     double y1[]={1, 3, 5, 6, 7};
  *     double z1[]={1, 2, 3, 4, 5,
  *                  6, 7, 8, 9, 10,
  *                  11,12,13,14,15,
  *                  16,17,18,19,20,
  *                  21,22,23,24,25};
  *     BSpline2D bs2(x1,5,y1,5,z1);
  *     if (bs2.ok())
  *     {
  *       printf("%g\t", x0);
  *       for (y0=0; y0 <= 8; y0 += 1)
  *       {
  *        printf("%g\t", y0);
  *       }
  *       printf("\n");
  *     for (x0=0; x0 <= 8; x0 += 1)
  *     {
  *       printf("%g\t", x0);
  *       for (y0=0; y0 <= 8; y0 += 1)
  *       {
  *        z0 = bs2.evaluate(x0,y0);
  *        printf("%g\t", z0);
  *       }
  *       printf("\n");
  *     }
  * 
  */
class BSpline2D
{
public:
  /**
   * Construct a spline domain for the given set of x & y values,
   * The parameters are the same as for setDomain().
   * Call ok() to check whether domain setup succeeded after construction.
   *
   * Create a single spline with the parameters required to set up
   * the domain and subsequently smooth the given set of y values.
   * The y values must correspond to each of the values in the x array.
   * If either the domain setup fails or the spline cannot be solved,
   * the state will be set to not ok.
   *
   * @see ok().
   *
   * @param x		The array of x values in the domain.
   * @param nx	The number of values in the @p x array.
   * @param y		The array of y values corresponding to each of the
   *			nX() x values in the domain.
   */
  BSpline2D( const double * x, size_t nx, const double * y, size_t ny, const double *z, const int & order = 3 );


  /**
   * Construct a spline domain for the given set of x & y values,
   * Call ok() to check whether domain setup succeeded after construction.
   *
   * Create a single spline with the parameters required to set up
   * the domain and subsequently smooth the given set of y values.
   * The y values must correspond to each of the values in the x array.
   * If either the domain setup fails or the spline cannot be solved,
   * the state will be set to not ok.
   *
   * @see ok().
   *
   * @param x		The vector of x values in the domain.
   * @param y		The vector of y values corresponding to each of 
   *			x values in the domain.
   */
  BSpline2D( const TLIST &x, const TLIST &y, const TLIST &z, const int & order = 3 );

  /**
   * The default construct a spline domain.
   * Call setDomain() later to setup B-spline.
   *
   * the state will be set to not ok.
   *
   * @see setDomain(const double * x, size_t nx, const double * y).
   */
  BSpline2D() : OK( false ), ErrNo(-1)
  {}

  /**
   * Change the domain of this base.  
   * The x values must be in ascending order.  The setup of
   * the domain may fail because  the resulting matrix
   * could not be factored.  If setup fails, the method returns false.
   *
   * @param x		The array of x values in the domain.
   * @param nx	The number of values in the @p x array.
   * @param y		The array of y values in the domain.
   *
   * see ok().
   */
  bool setDomain( const double * x, size_t nx, const double * y, size_t ny, const double * z, const int & order = 3 );

  /**
   * Change the domain of this base.  
   * The x values must be in ascending order.  The setup of
   * the domain may fail because  the resulting matrix
   * could not be factored.  If setup fails, the method returns false.
   *
   * @param x		The array of x values in the domain.
   * @param y		The array of y values in the domain.
   *
   * see ok().
   */
  bool setDomain( const TLIST &x, const TLIST &y, const TLIST &z, const int & order = 3 );

  /**
   * Change the domain of this base after changing the nodes' pos.  
   * The setup of the domain may fail because  the resulting matrix
   * could not be factored.  If setup fails, the method returns false.
   */
  bool setDomain( const int & order = 3 );

  /**
   * Return the evaluation of the sample curve 
   * at a particular @p x value.  If current state is not ok(), returns 0.
   * for big number of evaluation. for speed.
   *
   * @param x	 evaluate at the @p x value.
   */
  double evaluate( double x,  double y, int ideriv = 0 );

  /**
   * Return the current state of the object, either ok or not ok.
   * Use this method to test for valid state after construction or after
   * a call to setDomain().  ok() will return false if either fail, such
   * as when an appropriate number of nodes and node interval cannot be
   * found for a given wavelength, or when the linear equation for the
   * coefficients cannot be solved.
   */
  bool ok() const
  {
    return OK;
  }

  /**
   * Return the number of node list.
   */
  size_t nNode() const
  {
    return node_x_list.size();
  }

  /**
   * Return the x value at i-th point in the node list.
   * The i-th point should be in the node list.
   *
   * @param i	 x value at the @p i-th node.
   */
  double xNode( size_t i ) const
  {
    return node_x_list[i];
  }

  /**
   * Set the x value at i-th point in the node list.
   * The i-th point should be in the node list.
   * Need to setDomain again.
   *
   * @param i	 x value at the @p i-th node.
   */
  double xNode( size_t i, double x )
  {
    double tmp = node_x_list[i];

    node_x_list[i] = x;
    OK = false;
    return tmp;
  }

  /**
   * Return the y value at i-th point in the node list.
   * The i-th point should be in the node list.
   *
   * @param i	 y value at the @p i-th node.
   */
  double yNode( size_t i ) const
  {
    return node_y_list[i];
  }

  /**
   * Set the y value at i-th point in the node list.
   * The i-th point should be in the node list.
   * Need to setDomain again.
   *
   * @param i	 y value at the @p i-th node.
   */
  double yNode( size_t i, double y )
  {
    double tmp = node_y_list[i];

    node_y_list[i] = y;
    OK = false;
    return tmp;
  }

  /**
   * Return the z value at i-th point in the node list.
   * The i-th point should be in the node list.
   *
   * @param i	 z value at the @p i-th node.
   */
  double zNode( size_t i ) const
  {
    return node_z_list[i];
  }

  /**
   * Set the z value at i-th point in the node list.
   * The i-th point should be in the node list.
   * Need to setDomain again.
   *
   * @param i	 z value at the @p i-th node.
   */
  double zNode( size_t i, double z )
  {
    double tmp = node_z_list[i];

    node_z_list[i] = z;
    OK = false;
    return tmp;
  }

  int ReturnCode() const;

protected:

  /**
    * Node X list.
    *
    * Maintain the input node x list.
    */
  TLIST node_x_list;

  /**
    * Node Y list.
    *
    * Maintain the input node y list.
    */
  TLIST node_y_list;

  /**
    * Node Z list.
    *
    * Maintain the input node z list.
    */
  TLIST node_z_list;

  /**
    * B-Spline coeffient list.
    *
    * Maintain the B-Spline coeffient list.
    */
  TLIST bcoef_list;

  /**
    * State flag.
    *
    * Maintain the state for the B-spline.
    * 
    * @see ok().
    */
  bool OK;

  /// number
  int nx, ny, ldf;

  /// order
  int kx, ky;

  /// work space
  TLIST work_list;

  TLIST tx, ty;

  int ErrNo;
};

/** 3 dimention B-Spline interpolation function and class
  * 
  */
class BSpline3D
{
public:
  /**
   * Construct a spline domain for the given set of x & y values,
   * The parameters are the same as for setDomain().
   * Call ok() to check whether domain setup succeeded after construction.
   *
   * Create a single spline with the parameters required to set up
   * the domain and subsequently smooth the given set of y values.
   * The y values must correspond to each of the values in the x array.
   * If either the domain setup fails or the spline cannot be solved,
   * the state will be set to not ok.
   *
   * @see ok().
   *
   * @param x		The array of x values in the domain.
   * @param nx	The number of values in the @p x array.
   * @param y		The array of y values corresponding to each of the
   *			nX() x values in the domain.
   */
  BSpline3D( const double * x, size_t nx, const double * y, size_t ny, const double *z, size_t nz, const double *f, const int & order = 3 );


  /**
   * Construct a spline domain for the given set of x & y values,
   * Call ok() to check whether domain setup succeeded after construction.
   *
   * Create a single spline with the parameters required to set up
   * the domain and subsequently smooth the given set of y values.
   * The y values must correspond to each of the values in the x array.
   * If either the domain setup fails or the spline cannot be solved,
   * the state will be set to not ok.
   *
   * @see ok().
   *
   * @param x		The vector of x values in the domain.
   * @param y		The vector of y values corresponding to each of 
   *			x values in the domain.
   */
  BSpline3D( const TLIST &x, const TLIST &y, const TLIST &z, const TLIST &f, const int & order = 3 );

  /**
   * The default construct a spline domain.
   * Call setDomain() later to setup B-spline.
   *
   * the state will be set to not ok.
   *
   * @see setDomain(const double * x, size_t nx, const double * y).
   */
  BSpline3D() : OK( false ), ErrNo(-1)
  {}

  /**
   * Change the domain of this base.  
   * The x values must be in ascending order.  The setup of
   * the domain may fail because  the resulting matrix
   * could not be factored.  If setup fails, the method returns false.
   *
   * @param x		The array of x values in the domain.
   * @param nx	The number of values in the @p x array.
   * @param y		The array of y values in the domain.
   *
   * see ok().
   */
  bool setDomain( const double * x, size_t nx, const double * y, size_t ny, const double * z, size_t nz, const double * f, const int & order = 3 );

  /**
   * Change the domain of this base.  
   * The x values must be in ascending order.  The setup of
   * the domain may fail because  the resulting matrix
   * could not be factored.  If setup fails, the method returns false.
   *
   * @param x		The array of x values in the domain.
   * @param y		The array of y values in the domain.
   *
   * see ok().
   */
  bool setDomain( const TLIST &x, const TLIST &y, const TLIST &z, const TLIST &f, const int & order = 3 );

  /**
   * Change the domain of this base after changing the nodes' pos.  
   * The setup of the domain may fail because  the resulting matrix
   * could not be factored.  If setup fails, the method returns false.
   */
  bool setDomain( const int & order = 3 );

  /**
   * Return the evaluation of the sample curve 
   * at a particular @p x value.  If current state is not ok(), returns 0.
   * for big number of evaluation. for speed.
   *
   * @param x	 evaluate at the @p x value.
   */
  double evaluate( double x,  double y, double z, int ideriv = 0 );

  /**
   * Return the current state of the object, either ok or not ok.
   * Use this method to test for valid state after construction or after
   * a call to setDomain().  ok() will return false if either fail, such
   * as when an appropriate number of nodes and node interval cannot be
   * found for a given wavelength, or when the linear equation for the
   * coefficients cannot be solved.
   */
  bool ok() const
  {
    return OK;
  }

  /**
   * Return the number of node list.
   */
  size_t nNode() const
  {
    return node_x_list.size();
  }

  /**
   * Return the x value at i-th point in the node list.
   * The i-th point should be in the node list.
   *
   * @param i	 x value at the @p i-th node.
   */
  double xNode( size_t i ) const
  {
    return node_x_list[i];
  }

  /**
   * Set the x value at i-th point in the node list.
   * The i-th point should be in the node list.
   * Need to setDomain again.
   *
   * @param i	 x value at the @p i-th node.
   */
  double xNode( size_t i, double x )
  {
    double tmp = node_x_list[i];

    node_x_list[i] = x;
    OK = false;
    return tmp;
  }

  /**
   * Return the y value at i-th point in the node list.
   * The i-th point should be in the node list.
   *
   * @param i	 y value at the @p i-th node.
   */
  double yNode( size_t i ) const
  {
    return node_y_list[i];
  }

  /**
   * Set the y value at i-th point in the node list.
   * The i-th point should be in the node list.
   * Need to setDomain again.
   *
   * @param i	 y value at the @p i-th node.
   */
  double yNode( size_t i, double y )
  {
    double tmp = node_y_list[i];

    node_y_list[i] = y;
    OK = false;
    return tmp;
  }

  /**
   * Return the z value at i-th point in the node list.
   * The i-th point should be in the node list.
   *
   * @param i	 z value at the @p i-th node.
   */
  double zNode( size_t i ) const
  {
    return node_z_list[i];
  }

  /**
   * Set the z value at i-th point in the node list.
   * The i-th point should be in the node list.
   * Need to setDomain again.
   *
   * @param i	 z value at the @p i-th node.
   */
  double zNode( size_t i, double z )
  {
    double tmp = node_z_list[i];

    node_z_list[i] = z;
    OK = false;
    return tmp;
  }

  /**
   * Return the f value at i-th point in the node list.
   * The i-th point should be in the node list.
   *
   * @param i	 f value at the @p i-th node.
   */
  double fNode( size_t i ) const
  {
    return node_f_list[i];
  }

  /**
   * Set the f value at i-th point in the node list.
   * The i-th point should be in the node list.
   * Need to setDomain again.
   *
   * @param i	 f value at the @p i-th node.
   */
  double fNode( size_t i, double f )
  {
    double tmp = node_f_list[i];

    node_f_list[i] = f;
    OK = false;
    return tmp;
  }

  int ReturnCode() const;

protected:

  /**
    * Node X list.
    *
    * Maintain the input node x list.
    */
  TLIST node_x_list;

  /**
    * Node Y list.
    *
    * Maintain the input node y list.
    */
  TLIST node_y_list;

  /**
    * Node Z list.
    *
    * Maintain the input node z list.
    */
  TLIST node_z_list;

  /**
    * Node F list.
    *
    * Maintain the input node f list.
    */
  TLIST node_f_list;

  /**
    * B-Spline coeffient list.
    *
    * Maintain the B-Spline coeffient list.
    */
  TLIST bcoef_list;

  /**
    * State flag.
    *
    * Maintain the state for the B-spline.
    * 
    * @see ok().
    */
  bool OK;

  /// number
  int nx, ny, nz, ldf1, ldf2;

  /// order
  int kx, ky, kz;

  /// work space
  TLIST work_list;

  TLIST tx, ty, tz;

  int ErrNo;
};

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
FUNCTION DECLARATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#ifdef __cplusplus
extern "C"
{
#endif

  /** ***BEGIN PROLOGUE  BINTK
  * ***DATE WRITTEN   800901   (YYMMDD)
  * ***REVISION DATE  820801   (YYMMDD)
  * ***REVISION HISTORY  (YYMMDD)
  *   000330  Modified array declarations.  (JEC)

  * ***CATEGORY NO.  E1A
  * ***KEYWORDS  B-SPLINE,DATA FITTING,INTERPOLATION,SPLINE
  * ***AUTHOR  AMOS, D. E., (SNLA)
  * ***PURPOSE  Produces the B-spline coefficients, BCOEF, of the
  *            B-spline of order K with knots T(I), I=1,...,N+K, which
  *            takes on the value Y(I) at X(I), I=1,...,N.
  * ***DESCRIPTION

  *     Written by Carl de Boor and modified by D. E. Amos

  *     References

  *          A Practical Guide to Splines by C. de Boor, Applied
  *          Mathematics Series 27, Springer, 1979.

  *     Abstract

  *         BINTK is the SPLINT routine of the reference.

  *         BINTK produces the B-spline coefficients, BCOEF, of the
  *         B-spline of order K with knots T(I), I=1,...,N+K, which
  *         takes on the value Y(I) at X(I), I=1,...,N.  The spline or
  *         any of its derivatives can be evaluated by calls to BVALU.
  *         The I-th equation of the linear system A*BCOEF = B for the
  *         coefficients of the interpolant enforces interpolation at
  *         X(I)), I=1,...,N.  Hence, B(I) = Y(I), all I, and A is
  *         a band matrix with 2K-1 bands if A is invertible. The matrix
  *         A is generated row by row and stored, diagonal by diagonal,
  *         in the rows of Q, with the main diagonal going into row K.
  *         The banded system is then solved by a call to BNFAC (which
  *         constructs the triangular factorization for A and stores it
  *         again in Q), followed by a call to BNSLV (which then
  *         obtains the solution BCOEF by substitution). BNFAC does no
  *         pivoting, since the total positivity of the matrix A makes
  *         this unnecessary.  The linear system to be solved is
  *         (theoretically) invertible if and only if
  *                 T(I) .LT. X(I)) .LT. T(I+K),        all I.
  *         Equality is permitted on the left for I=1 and on the right
  *         for I=N when K knots are used at X(1) or X(N).  Otherwise,
  *         violation of this condition is certain to lead to an error.

  *         BINTK calls BSPVN, BNFAC, BNSLV, XERROR

  *     Description of Arguments
  *         Input
  *           X       - vector of length N containing data point abscissa
  *                     in strictly increasing order.
  *           Y       - corresponding vector of length N containing data
  *                     point ordinates.
  *           T       - knot vector of length N+K
  *                     since T(1),..,T(K) .LE. X(1) and T(N+1),..,T(N+K)
  *                     .GE. X(N), this leaves only N-K knots (not nec-
  *                     essarily X(I)) values) interior to (X(1),X(N))
  *           N       - number of data points, N .GE. K
  *           K       - order of the spline, K .GE. 1

  *         Output
  *           BCOEF   - a vector of length N containing the B-spline
  *                     coefficients
  *           Q       - a work vector of length (2*K-1)*N, containing
  *                     the triangular factorization of the coefficient
  *                     matrix of the linear system being solved.  The
  *                     coefficients for the interpolant of an
  *                     additional data set (X(I)),YY(I)), I=1,...,N
  *                     with the same abscissa can be obtained by loading
  *                     YY into BCOEF and then executing
  *                         call BNSLV(Q,2K-1,N,K-1,K-1,BCOEF)
  *           WORK    - work vector of length 2*K

  *     Error Conditions
  *         Improper  input is a fatal error
  *         Singular system of equations is a fatal error
  * ***REFERENCES  D.E. AMOS, *COMPUTATION WITH SPLINES AND B-SPLINES*,
  *                 SAND78-1968,SANDIA LABORATORIES,MARCH,1979.
  *               C. DE BOOR, *PACKAGE FOR CALCULATING WITH B-SPLINES*,
  *                 SIAM JOURNAL ON NUMERICAL ANALYSIS, VOLUME 14, NO. 3,
  *                 JUNE 1977, PP. 441-472.
  *               C. DE BOOR, *A PRACTICAL GUIDE TO SPLINES*, APPLIED
  *                 MATHEMATICS SERIES 27, SPRINGER, 1979.
  * ***ROUTINES CALLED  BNFAC,BNSLV,BSPVN,XERROR
  * ***END PROLOGUE  BINTK
  */
  extern int bintk_( double *x, double *y, double *t, int *n, int *k, double *bcoef, double *q, double *work );

  /** ***BEGIN PROLOGUE  BNFAC
  * ***REFER TO  BINT4,BINTK

  *  BNFAC is the BANFAC routine from
  *        * A Practical Guide to Splines *  by C. de Boor

  *  Returns in  W  the lu-factorization (without pivoting) of the banded
  *  matrix  A  of order  NROW  with  (NBANDL + 1 + NBANDU) bands or diag-
  *  onals in the work array  W .

  * *****  I N P U T  ******
  *  W.....Work array of size  (NROWW,NROW)  containing the interesting
  *        part of a banded matrix  A , with the diagonals or bands of  A
  *        stored in the rows of  W , while columns of  A  correspond to
  *        columns of  W . This is the storage mode used in  LINPACK  and
  *        results in efficient innermost loops.
  *           Explicitly,  A  has  NBANDL  bands below the diagonal
  *                            +     1     (main) diagonal
  *                            +   NBANDU  bands above the diagonal
  *        and thus, with    MIDDLE = NBANDU + 1,
  *          A(I+J,J)  is in  W(I+MIDDLE,J)  for I=-NBANDU,...,NBANDL
  *                                              J=1,...,NROW .
  *        For example, the interesting entries of A (1,2)-banded matrix
  *        of order  9  would appear in the first  1+1+2 = 4  rows of  W
  *        as follows.
  *                          13 24 35 46 57 68 79
  *                       12 23 34 45 56 67 78 89
  *                    11 22 33 44 55 66 77 88 99
  *                    21 32 43 54 65 76 87 98

  *        All other entries of  W  not identified in this way with an en-
  *        try of  A  are never referenced .
  *  NROWW.....Row dimension of the work array  W .
  *        must be  .GE.  NBANDL + 1 + NBANDU  .
  *  NBANDL.....Number of bands of  A  below the main diagonal
  *  NBANDU.....Number of bands of  A  above the main diagonal .

  * *****  O U T P U T  ******
  *  IFLAG.....Integer indicating success( = 1) or failure ( = 2) .
  *     If  IFLAG = 1, then
  *  W.....contains the LU-factorization of  A  into a unit lower triangu-
  *        lar matrix  L  and an upper triangular matrix  U (both banded)
  *        and stored in customary fashion over the corresponding entries
  *        of  A . This makes it possible to solve any particular linear
  *        system  A*X = B  for  X  by A
  *              CALL BNSLV ( W, NROWW, NROW, NBANDL, NBANDU, B )
  *        with the solution X  contained in  B  on return .
  *     If  IFLAG = 2, then
  *        one of  NROW-1, NBANDL,NBANDU failed to be nonnegative, or else
  *        one of the potential pivots was found to be zero indicating
  *        that  A  does not have an LU-factorization. This implies that
  *        A  is singular in case it is totally positive .

  * *****  M E T H O D  ******
  *     Gauss elimination  W I T H O U T  pivoting is used. The routine is
  *  intended for use with matrices  A  which do not require row inter-
  *  changes during factorization, especially for the  T O T A L L Y
  *  P O S I T I V E  matrices which occur in spline calculations.
  *     The routine should not be used for an arbitrary banded matrix.
  * ***ROUTINES CALLED  (NONE)
  * ***END PROLOGUE  BNFAC
  */
  extern int bnfac_( double *w, int *nroww, int *nrow, int *nbandl, int *nbandu, int *iflag );

  /** ***BEGIN PROLOGUE  BNSLV
  * ***REFER TO  BINT4,BINTK

  *  BNSLV is the BANSLV routine from
  *        * A Practical Guide to Splines *  by C. de Boor

  *  Companion routine to  BNFAC . It returns the solution  X  of the
  *  linear system  A*X = B  in place of  B , given the LU-factorization
  *  for  A  in the work array  W from BNFAC.

  * *****  I N P U T  ******
  *  W, NROWW,NROW,NBANDL,NBANDU.....describe the LU-factorization of a
  *        banded matrix  A  of order  NROW  as constructed in  BNFAC .
  *        For details, see  BNFAC .
  *  B.....Right side of the system to be solved .

  * *****  O U T P U T  ******
  *  B.....Contains the solution  X , of order  NROW .

  * *****  M E T H O D  ******
  *     (With  A = L*U, as stored in  W,) the unit lower triangular system
  *  L(U*X) = B  is solved for  Y = U*X, and  Y  stored in  B . Then the
  *  upper triangular system  U*X = Y  is solved for  X  . The calcul-
  *  ations are so arranged that the innermost loops stay within columns.
  * ***ROUTINES CALLED  (NONE)
  * ***END PROLOGUE  BNSLV
  */
  extern int bnslv_( double *w, int *nroww, int *nrow, int *nbandl, int *nbandu, double *b );

  /** ***BEGIN PROLOGUE  BSPVN
  * ***DATE WRITTEN   800901   (YYMMDD)
  * ***REVISION DATE  820801   (YYMMDD)
  * ***CATEGORY NO.  E3,K6
  * ***KEYWORDS  B-SPLINE,DATA FITTING,INTERPOLATION,SPLINE
  * ***AUTHOR  AMOS, D. E., (SNLA)
  * ***PURPOSE  Calculates the value of all (possibly) nonzero basis
  *            functions at X.
  * ***DESCRIPTION

  *     Written by Carl de Boor and modified by D. E. Amos

  *     Reference
  *         SIAM J. Numerical Analysis, 14, No. 3, June, 1977, pp.441-472.

  *     Abstract
  *         BSPVN is the BSPLVN routine of the reference.

  *         BSPVN calculates the value of all (possibly) nonzero basis
  *         functions at X of order MAX(JHIGH,(J+1)*(INDEX-1)), where
  *         T(K) .LE. X .LE. T(N+1) and J=IWORK is set inside the routine
  *         on the first call when INDEX=1.  ILEFT is such that T(ILEFT)
  *         .LE. X .LT. T(ILEFT+1).  A call to INTRV(T,N+1,X,ILO,ILEFT,
  *         MFLAG) produces the proper ILEFT.  BSPVN calculates using the
  *         basic algorithm needed in BSPVD.  If only basis functions are
  *         desired, setting JHIGH=K and INDEX=1 can be faster than
  *         calling BSPVD, but extra coding is required for derivatives
  *         (INDEX=2) and BSPVD is set up for this purpose.

  *         Left limiting values are set up as described in BSPVD.

  *     Description of Arguments
  *         Input
  *          T       - knot vector of length N+K, where
  *                    N = number of B-spline basis functions
  *                    N = sum of knot multiplicities-K
  *          JHIGH   - order of B-spline, 1 .LE. JHIGH .LE. K
  *          K       - highest possible order
  *          INDEX   - INDEX = 1 gives basis functions of order JHIGH
  *                          = 2 denotes previous entry with WORK, IWORK
  *                              values saved for subsequent calls to
  *                              BSPVN.
  *          X       - argument of basis functions,
  *                    T(K) .LE. X .LE. T(N+1)
  *          ILEFT   - largest int such that
  *                    T(ILEFT) .LE. X .LT. T(ILEFT+1)

  *         Output
  *          VNIKX   - vector of length K for spline values.
  *          WORK    - a work vector of length 2*K
  *          IWORK   - a work parameter.  Both WORK and IWORK contain
  *                    information necessary to continue for INDEX = 2.
  *                    When INDEX = 1 exclusively, these are scratch
  *                    variables and can be used for other purposes.

  *     Error Conditions
  *         Improper input is a fatal error.
  * ***REFERENCES  C. DE BOOR, *PACKAGE FOR CALCULATING WITH B-SPLINES*,
  *                 SIAM JOURNAL ON NUMERICAL ANALYSIS, VOLUME 14, NO. 3,
  *                 JUNE 1977, PP. 441-472.
  * ***ROUTINES CALLED  XERROR
  * ***END PROLOGUE  BSPVN
  */
  extern int bspvn_( double *t, int *jhigh, int *k, int *index, double *x, int *ileft, double *vnikx, double *work, int *iwork );

  /** ***BEGIN PROLOGUE  INTRV
  * ***DATE WRITTEN   800901   (YYMMDD)
  * ***REVISION DATE  820801   (YYMMDD)
  * ***CATEGORY NO.  E3,K6
  * ***KEYWORDS  B-SPLINE,DATA FITTING,INTERPOLATION,SPLINE
  * ***AUTHOR  AMOS, D. E., (SNLA)
  * ***PURPOSE  Computes the largest int ILEFT in 1.LE.ILEFT.LE.LXT
  *            such that XT(ILEFT).LE.X where XT(*) is a subdivision
  *            of the X interval.
  * ***DESCRIPTION

  *     Written by Carl de Boor and modified by D. E. Amos

  *     Reference
  *         SIAM J. Numerical Analysis, 14, No. 3, June 1977, pp. 441-472.

  *     Abstract
  *         INTRV is the INTERV routine of the reference.

  *         INTRV computes the largest int ILEFT in 1 .LE. ILEFT .LE.
  *         LXT such that XT(ILEFT) .LE. X where XT(*) is a subdivision of
  *         the X interval.  Precisely,

  *                      X .LT. XT(1)                1         -1
  *         if  XT(I) .LE. X .LT. XT(I+1)  then  ILEFT=I  , MFLAG=0
  *           XT(LXT) .LE. X                         LXT        1,

  *         That is, when multiplicities are present in the break point
  *         to the left of X, the largest index is taken for ILEFT.

  *     Description of Arguments
  *         Input
  *          XT      - XT is a knot or break point vector of length LXT
  *          LXT     - length of the XT vector
  *          X       - argument
  *          ILO     - an initialization parameter which must be set
  *                    to 1 the first time the spline array XT is
  *                    processed by INTRV.

  *         Output
  *          ILO     - ILO contains information for efficient process-
  *                    ing after the initial call, and ILO must not be
  *                    changed by the user.  Distinct splines require
  *                    distinct ILO parameters.
  *          ILEFT   - largest int satisfying XT(ILEFT) .LE. X
  *          MFLAG   - signals when X lies out of bounds

  *     Error Conditions
  *         None
  * ***REFERENCES  C. DE BOOR, *PACKAGE FOR CALCULATING WITH B-SPLINES*,
  *                 SIAM JOURNAL ON NUMERICAL ANALYSIS, VOLUME 14, NO. 3,
  *                 JUNE 1977, PP. 441-472.
  * ***ROUTINES CALLED  (NONE)
  * ***END PROLOGUE  INTRV
  */
  extern int intrv_( double *xt, int *lxt, double *x, int *ilo, int *ileft, int *mflag );

  /** ***BEGIN PROLOGUE  B2INK
  * ***DATE WRITTEN   25 MAY 1982
  * ***REVISION DATE  25 MAY 1982
  * ***REVISION HISTORY  (YYMMDD)
  *   000330  Modified array declarations.  (JEC)
  * ***CATEGORY NO.  E1A
  * ***KEYWORDS  INTERPOLATION, TWO-DIMENSIONS, GRIDDED DATA, SPLINES,
  *             PIECEWISE POLYNOMIALS
  * ***AUTHOR  BOISVERT, RONALD, NBS
  *             SCIENTIFIC COMPUTING DIVISION
  *             NATIONAL BUREAU OF STANDARDS
  *             WASHINGTON, DC 20234
  * ***PURPOSE  B2INK DETERMINES A PIECEWISE POLYNOMIAL FUNCTION THAT
  *            INTERPOLATES TWO-DIMENSIONAL GRIDDED DATA. USERS SPECIFY
  *            THE POLYNOMIAL ORDER (DEGREE+1) OF THE INTERPOLANT AND
  *            (OPTIONALLY) THE KNOT SEQUENCE.
  * ***DESCRIPTION

  *   B2INK determines the parameters of a function that interpolates the
  *   two-dimensional gridded data (X(i),Y(j),FCN(i,j)) for i=1,..,NX and
  *   j=1,..,NY. The  interpolating  function  and  its  derivatives  may
  *   subsequently be evaluated by the function B2VAL.

  *   The interpolating  function  is  a  piecewise  polynomial  function
  *   represented as a tensor product of one-dimensional  B-splines.  The
  *   form of this function is

  *                          NX   NY
  *              S(x,y)  =  SUM  SUM  a   U (x) V (y)
  *                         i=1  j=1   ij  i     j

  *   where the functions U(i)  and  V(j)  are  one-dimensional  B-spline
  *   basis functions. The coefficients a(i,j) are chosen so that

  *         S(X(i),Y(j)) = FCN(i,j)   for i=1,..,NX and j=1,..,NY

  *   Note that  for  each  fixed  value  of  y  S(x,y)  is  a  piecewise
  *   polynomial function of x alone, and for each fixed value of x  S(x,
  *   y) is a piecewise polynomial function of y alone. In one  dimension
  *   a piecewise polynomial may  be  created  by  partitioning  a  given
  *   interval into subintervals and defining a distinct polynomial piece
  *   on each one. The points where adjacent subintervals meet are called
  *   knots. Each of the functions U(i) and V(j)  above  is  a  piecewise
  *   polynomial.

  *   Users of B2INK choose the order (degree+1) of the polynomial pieces
  *   used to define the piecewise polynomial in each  of  the  x  and  y
  *   directions (KX and KY).  Users  also  may  define  their  own  knot
  *   sequence in x and y separately (TX and TY).  If  IFLAG=0,  however,
  *   B2INK will choose sequences of knots that  result  in  a  piecewise
  *   polynomial interpolant with KX-2 continuous partial derivatives  in
  *   x and KY-2 continuous partial derivatives in y. (KX knots are taken
  *   near each endpoint, not-a-knot end conditions  are  used,  and  the
  *   remaining knots are placed at data points  if  KX  is  even  or  at
  *   midpoints between data points if KX is  odd.  The  y  direction  is
  *   treated similarly.)

  *   After a call to B2INK, all  information  necessary  to  define  the
  *   interpolating function are contained in the parameters NX, NY,  KX,
  *   KY, TX, TY, and BCOEF. These quantities should not be altered until
  *   after the last call of the evaluation routine B2VAL.


  *   I N P U T
  *   ---------

  *   X       Real 1D array (size NX)
  *           Array of x abcissae. Must be strictly increasing.

  *   NX      Integer scalar (.GE. 3)
  *           Number of x abcissae.

  *   Y       Real 1D array (size NY)
  *           Array of y abcissae. Must be strictly increasing.

  *   NY      Integer scalar (.GE. 3)
  *           Number of y abcissae.

  *   FCN     Real 2D array (size LDF by NY)
  *           Array of function values to interpolate. FCN(I,J) should
  *           contain the function value at the point (X(I),Y(J))

  *   LDF     Integer scalar (.GE. NX)
  *           The actual leading dimension of FCN used in the calling
  *           calling program.

  *   KX      Integer scalar (.GE. 2, .LT. NX)
  *           The order of spline pieces in x.
  *           (Order = polynomial degree + 1)

  *   KY      Integer scalar (.GE. 2, .LT. NY)
  *           The order of spline pieces in y.
  *           (Order = polynomial degree + 1)


  *   I N P U T   O R   O U T P U T
  *   -----------------------------

  *   TX      Real 1D array (size NX+KX)
  *           The knots in the x direction for the spline interpolant.
  *           If IFLAG=0 these are chosen by B2INK.
  *           If IFLAG=1 these are specified by the user.
  *                      (Must be non-decreasing.)

  *   TY      Real 1D array (size NY+KY)
  *           The knots in the y direction for the spline interpolant.
  *           If IFLAG=0 these are chosen by B2INK.
  *           If IFLAG=1 these are specified by the user.
  *                      (Must be non-decreasing.)


  *   O U T P U T
  *   -----------

  *   BCOEF   Real 2D array (size NX by NY)
  *           Array of coefficients of the B-spline interpolant.
  *           This may be the same array as FCN.


  *   M I S C E L L A N E O U S
  *   -------------------------

  *   WORK    Real 1D array (size NX*NY + max( 2*KX*(NX+1),
  *                                  2*KY*(NY+1) ))
  *           Array of working storage.

  *   IFLAG   Integer scalar.
  *           On input:  0 == knot sequence chosen by B2INK
  *                      1 == knot sequence chosen by user.
  *           On output: 1 == successful execution
  *                      2 == IFLAG out of range
  *                      3 == NX out of range
  *                      4 == KX out of range
  *                      5 == X not strictly increasing
  *                      6 == TX not non-decreasing
  *                      7 == NY out of range
  *                      8 == KY out of range
  *                      9 == Y not strictly increasing
  *                     10 == TY not non-decreasing

  * ***REFERENCES  CARL DE BOOR, A PRACTICAL GUIDE TO SPLINES,
  *                 SPRINGER-VERLAG, NEW YORK, 1978.
  *               CARL DE BOOR, EFFICIENT COMPUTER MANIPULATION OF TENSOR
  *                 PRODUCTS, ACM TRANSACTIONS ON MATHEMATICAL SOFTWARE,
  *                 VOL. 5 (1979), PP. 173-182.
  * ***ROUTINES CALLED  BTPCF,BKNOT
  * ***END PROLOGUE  B2INK
  */
  extern int b2ink_( double *x, int *nx, double *y, int *ny, double *fcn, int *ldf, int *kx, int *ky, double *tx, double *ty, double *bcoef, double *work, int *iflag );

  /** ***BEGIN PROLOGUE  B2VAL
  * ***DATE WRITTEN   25 MAY 1982
  * ***REVISION DATE  25 MAY 1982
  * ***CATEGORY NO.  E1A
  * ***KEYWORDS  INTERPOLATION, TWO-DIMENSIONS, GRIDDED DATA, SPLINES,
  *             PIECEWISE POLYNOMIALS
  * ***AUTHOR  BOISVERT, RONALD, NBS
  *             SCIENTIFIC COMPUTING DIVISION
  *             NATIONAL BUREAU OF STANDARDS
  *             WASHINGTON, DC 20234
  * ***PURPOSE  B2VAL EVALUATES THE PIECEWISE POLYNOMIAL INTERPOLATING
  *            FUNCTION CONSTRUCTED BY THE ROUTINE B2INK OR ONE OF ITS
  *            PARTIAL DERIVATIVES.
  * ***DESCRIPTION

  *   B2VAL evaluates the tensor product piecewise polynomial interpolant
  *   constructed by the routine B2INK or one of its derivatives  at  the
  *   point (XVAL,YVAL). To evaluate the interpolant itself, set IDX=IDY=
  *   0, to evaluate the first partial with respect to x, set  IDX=1,IDY=
  *   0, and so on.

  *   B2VAL returns 0.0E0 if (XVAL,YVAL) is out of range. That is, if
  *            XVAL.LT.TX(1) .OR. XVAL.GT.TX(NX+KX) .OR.
  *            YVAL.LT.TY(1) .OR. YVAL.GT.TY(NY+NY)
  *   If the knots  TX  and  TY  were  chosen  by  B2INK,  then  this  is
  *   equivalent to
  *            XVAL.LT.X(1) .OR. XVAL.GT.X(NX)+EPSX .OR.
  *            YVAL.LT.Y(1) .OR. YVAL.GT.Y(NY)+EPSY
  *   where EPSX = 0.1*(X(NX)-X(NX-1)) and EPSY = 0.1*(Y(NY)-Y(NY-1)).

  *   The input quantities TX, TY, NX, NY, KX, KY, and  BCOEF  should  be
  *   unchanged since the last call of B2INK.


  *   I N P U T
  *   ---------

  *   XVAL    Real scalar
  *           X coordinate of evaluation point.

  *   YVAL    Real scalar
  *           Y coordinate of evaluation point.

  *   IDX     Integer scalar
  *           X derivative of piecewise polynomial to evaluate.

  *   IDY     Integer scalar
  *           Y derivative of piecewise polynomial to evaluate.

  *   TX      Real 1D array (size NX+KX)
  *           Sequence of knots defining the piecewise polynomial in
  *           the x direction.  (Same as in last call to B2INK.)

  *   TY      Real 1D array (size NY+KY)
  *           Sequence of knots defining the piecewise polynomial in
  *           the y direction.  (Same as in last call to B2INK.)

  *   NX      Integer scalar
  *           The number of interpolation points in x.
  *           (Same as in last call to B2INK.)

  *   NY      Integer scalar
  *           The number of interpolation points in y.
  *           (Same as in last call to B2INK.)

  *   KX      Integer scalar
  *           Order of polynomial pieces in x.
  *           (Same as in last call to B2INK.)

  *   KY      Integer scalar
  *           Order of polynomial pieces in y.
  *           (Same as in last call to B2INK.)

  *   BCOEF   Real 2D array (size NX by NY)
  *           The B-spline coefficients computed by B2INK.

  *   WORK    Real 1D array (size 3*max(KX,KY) + KY)
  *           A working storage array.

  * ***REFERENCES  CARL DE BOOR, A PRACTICAL GUIDE TO SPLINES,
  *                 SPRINGER-VERLAG, NEW YORK, 1978.
  * ***ROUTINES CALLED  INTRV,BVALU
  * ***END PROLOGUE  B2VAL
  */
  extern double b2val___( double *xval, double *yval, int *idx, int *idy, double *tx, double *ty, int *nx, int *ny, int *kx, int *ky, double *bcoef, double *work );

  /** ***BEGIN PROLOGUE  B3INK
  * ***DATE WRITTEN   25 MAY 1982
  * ***REVISION DATE  25 MAY 1982
  * ***REVISION HISTORY  (YYMMDD)
  *   000330  Modified array declarations.  (JEC)
  * ***CATEGORY NO.  E1A
  * ***KEYWORDS  INTERPOLATION, THREE-DIMENSIONS, GRIDDED DATA, SPLINES,
  *             PIECEWISE POLYNOMIALS
  * ***AUTHOR  BOISVERT, RONALD, NBS
  *             SCIENTIFIC COMPUTING DIVISION
  *             NATIONAL BUREAU OF STANDARDS
  *             WASHINGTON, DC 20234
  * ***PURPOSE  B3INK DETERMINES A PIECEWISE POLYNOMIAL FUNCTION THAT
  *            INTERPOLATES THREE-DIMENSIONAL GRIDDED DATA. USERS SPECIFY
  *            THE POLYNOMIAL ORDER (DEGREE+1) OF THE INTERPOLANT AND
  *            (OPTIONALLY) THE KNOT SEQUENCE.
  * ***DESCRIPTION

  *   B3INK determines the parameters of a function that interpolates the
  *   three-dimensional gridded data (X(i),Y(j),Z(k),FCN(i,j,k)) for i=1,
  *   ..,NX, j=1,..,NY, and k=1,..,NZ. The interpolating function and its
  *   derivatives may subsequently be evaluated by the function B3VAL.

  *   The interpolating  function  is  a  piecewise  polynomial  function
  *   represented as a tensor product of one-dimensional  B-splines.  The
  *   form of this function is

  *                      NX   NY   NZ
  *        S(x,y,z)  =  SUM  SUM  SUM  a   U (x) V (y) W (z)
  *                     i=1  j=1  k=1   ij  i     j     k

  *   where the functions U(i), V(j), and  W(k)  are  one-dimensional  B-
  *   spline basis functions. The coefficients a(i,j) are chosen so that

  *   S(X(i),Y(j),Z(k)) = FCN(i,j,k)  for i=1,..,NX, j=1,..,NY, k=1,..,NZ

  *   Note that for fixed values of y  and  z  S(x,y,z)  is  a  piecewise
  *   polynomial function of x alone, for fixed values of x and z  S(x,y,
  *   z) is a piecewise polynomial function of y  alone,  and  for  fixed
  *   values of x and y S(x,y,z)  is  a  function  of  z  alone.  In  one
  *   dimension a piecewise polynomial may be created by  partitioning  a
  *   given interval into subintervals and defining a distinct polynomial
  *   piece on each one. The points where adjacent subintervals meet  are
  *   called knots. Each of the functions U(i), V(j), and W(k) above is a
  *   piecewise polynomial.

  *   Users of B3INK choose the order (degree+1) of the polynomial pieces
  *   used to define the piecewise polynomial in each of the x, y, and  z
  *   directions (KX, KY, and KZ). Users also may define their  own  knot
  *   sequence in x, y, and z separately (TX, TY, and  TZ).  If  IFLAG=0,
  *   however, B3INK will choose sequences of  knots  that  result  in  a
  *   piecewise  polynomial  interpolant  with  KX-2  continuous  partial
  *   derivatives in x, KY-2 continuous partial derivatives in y, and KZ-
  *   2 continuous partial derivatives in z. (KX  knots  are  taken  near
  *   each endpoint in x, not-a-knot end conditions  are  used,  and  the
  *   remaining knots are placed at data points  if  KX  is  even  or  at
  *   midpoints between data points if KX is odd. The y and z  directions
  *   are treated similarly.)

  *   After a call to B3INK, all  information  necessary  to  define  the
  *   interpolating function are contained in the parameters NX, NY,  NZ,
  *   KX, KY, KZ, TX, TY, TZ, and BCOEF. These quantities should  not  be
  *   altered until after the last call of the evaluation routine B3VAL.


  *   I N P U T
  *   ---------

  *   X       Real 1D array (size NX)
  *           Array of x abcissae. Must be strictly increasing.

  *   NX      Integer scalar (.GE. 3)
  *           Number of x abcissae.

  *   Y       Real 1D array (size NY)
  *           Array of y abcissae. Must be strictly increasing.

  *   NY      Integer scalar (.GE. 3)
  *           Number of y abcissae.

  *   Z       Real 1D array (size NZ)
  *           Array of z abcissae. Must be strictly increasing.

  *   NZ      Integer scalar (.GE. 3)
  *           Number of z abcissae.

  *   FCN     Real 3D array (size LDF1 by LDF2 by NY)
  *           Array of function values to interpolate. FCN(I,J,K) should
  *           contain the function value at the point (X(I),Y(J),Z(K))

  *   LDF1    Integer scalar (.GE. NX)
  *           The actual first dimension of FCN used in the
  *           calling program.

  *   LDF2    Integer scalar (.GE. NY)
  *           The actual second dimension of FCN used in the calling
  *           program.

  *   KX      Integer scalar (.GE. 2, .LT. NX)
  *           The order of spline pieces in x.
  *           (Order = polynomial degree + 1)

  *   KY      Integer scalar (.GE. 2, .LT. NY)
  *           The order of spline pieces in y.
  *           (Order = polynomial degree + 1)

  *   KZ      Integer scalar (.GE. 2, .LT. NZ)
  *           The order of spline pieces in z.
  *           (Order = polynomial degree + 1)


  *   I N P U T   O R   O U T P U T
  *   -----------------------------

  *   TX      Real 1D array (size NX+KX)
  *           The knots in the x direction for the spline interpolant.
  *           If IFLAG=0 these are chosen by B3INK.
  *           If IFLAG=1 these are specified by the user.
  *                      (Must be non-decreasing.)

  *   TY      Real 1D array (size NY+KY)
  *           The knots in the y direction for the spline interpolant.
  *           If IFLAG=0 these are chosen by B3INK.
  *           If IFLAG=1 these are specified by the user.
  *                      (Must be non-decreasing.)

  *   TZ      Real 1D array (size NZ+KZ)
  *           The knots in the z direction for the spline interpolant.
  *           If IFLAG=0 these are chosen by B3INK.
  *           If IFLAG=1 these are specified by the user.
  *                      (Must be non-decreasing.)


  *   O U T P U T
  *   -----------

  *   BCOEF   Real 3D array (size NX by NY by NZ)
  *           Array of coefficients of the B-spline interpolant.
  *           This may be the same array as FCN.


  *   M I S C E L L A N E O U S
  *   -------------------------

  *   WORK    Real 1D array (size NX*NY*NZ + max( 2*KX*(NX+1),
  *                             2*KY*(NY+1), 2*KZ*(NZ+1) )
  *           Array of working storage.

  *   IFLAG   Integer scalar.
  *           On input:  0 == knot sequence chosen by B2INK
  *                      1 == knot sequence chosen by user.
  *           On output: 1 == successful execution
  *                      2 == IFLAG out of range
  *                      3 == NX out of range
  *                      4 == KX out of range
  *                      5 == X not strictly increasing
  *                      6 == TX not non-decreasing
  *                      7 == NY out of range
  *                      8 == KY out of range
  *                      9 == Y not strictly increasing
  *                     10 == TY not non-decreasing
  *                     11 == NZ out of range
  *                     12 == KZ out of range
  *                     13 == Z not strictly increasing
  *                     14 == TY not non-decreasing

  * ***REFERENCES  CARL DE BOOR, A PRACTICAL GUIDE TO SPLINES,
  *                 SPRINGER-VERLAG, NEW YORK, 1978.
  *               CARL DE BOOR, EFFICIENT COMPUTER MANIPULATION OF TENSOR
  *                 PRODUCTS, ACM TRANSACTIONS ON MATHEMATICAL SOFTWARE,
  *                 VOL. 5 (1979), PP. 173-182.
  * ***ROUTINES CALLED  BTPCF,BKNOT
  * ***END PROLOGUE  B3INK
  */
  extern int b3ink_( double *x, int *nx, double *y, int *ny, double *z__, int *nz, double *fcn, int *ldf1, int *ldf2, int *kx, int *ky, int *kz, double *tx, double *ty, double *tz, double *bcoef, double *work, int *iflag );

  /** ***BEGIN PROLOGUE  B3VAL
  * ***DATE WRITTEN   25 MAY 1982
  * ***REVISION DATE  25 MAY 1982
  * ***CATEGORY NO.  E1A
  * ***KEYWORDS  INTERPOLATION, THREE-DIMENSIONS, GRIDDED DATA, SPLINES,
  *             PIECEWISE POLYNOMIALS
  * ***AUTHOR  BOISVERT, RONALD, NBS
  *             SCIENTIFIC COMPUTING DIVISION
  *             NATIONAL BUREAU OF STANDARDS
  *             WASHINGTON, DC 20234
  * ***PURPOSE  B3VAL EVALUATES THE PIECEWISE POLYNOMIAL INTERPOLATING
  *            FUNCTION CONSTRUCTED BY THE ROUTINE B3INK OR ONE OF ITS
  *            PARTIAL DERIVATIVES.
  * ***DESCRIPTI/* ***BEGIN PROLOGUE  B3VAL
  * ***DATE WRITTEN   25 MAY 1982
  * ***REVISION DATE  25 MAY 1982
  * ***CATEGORY NO.  E1A
  * ***KEYWORDS  INTERPOLATION, THREE-DIMENSIONS, GRIDDED DATA, SPLINES,
  *             PIECEWISE POLYNOMIALS
  * ***AUTHOR  BOISVERT, RONALD, NBS
  *             SCIENTIFIC COMPUTING DIVISION
  *             NATIONAL BUREAU OF STANDARDS
  *             WASHINGTON, DC 20234
  * ***PURPOSE  B3VAL EVALUATES THE PIECEWISE POLYNOMIAL INTERPOLATING
  *            FUNCTION CONSTRUCTED BY THE ROUTINE B3INK OR ONE OF ITS
  *            PARTIAL DERIVATIVES.
  * ***DESCRIPTION

  *   B3VAL evaluates the tensor product piecewise polynomial interpolant
  *   constructed by the routine B3INK or one of its derivatives  at  the
  *   point (XVAL,YVAL,ZVAL). To evaluate  the  interpolant  itself,  set
  *   IDX=IDY=IDZ=0, to evaluate the first partial with respect to x, set
  *   IDX=1,IDY=IDZ=0, and so on.

  *   B3VAL returns 0.0E0 if (XVAL,YVAL,ZVAL) is out of range. That is,
  *            XVAL.LT.TX(1) .OR. XVAL.GT.TX(NX+KX) .OR.
  *            YVAL.LT.TY(1) .OR. YVAL.GT.TY(NY+NY) .OR.
  *            ZVAL.LT.TZ(1) .OR. ZVAL.GT.TZ(NZ+KZ)
  *   If the knots TX, TY, and TZ were chosen  by  B3INK,  then  this  is
  *   equivalent to
  *            XVAL.LT.X(1) .OR. XVAL.GT.X(NX)+EPSX .OR.
  *            YVAL.LT.Y(1) .OR. YVAL.GT.Y(NY)+EPSY .OR.
  *            ZVAL.LT.Z(1) .OR. ZVAL.GT.Z(NZ)+EPSZ
  *   where EPSX = 0.1*(X(NX)-X(NX-1)), EPSY =  0.1*(Y(NY)-Y(NY-1)),  and
  *   EPSZ = 0.1*(Z(NZ)-Z(NZ-1)).

  *   The input quantities TX, TY, TZ, NX, NY, NZ, KX, KY, KZ, and  BCOEF
  *   should remain unchanged since the last call of B3INK.


  *   I N P U T
  *   ---------

  *   XVAL    Real scalar
  *           X coordinate of evaluation point.

  *   YVAL    Real scalar
  *           Y coordinate of evaluation point.

  *   ZVAL    Real scalar
  *           Z coordinate of evaluation point.

  *   IDX     Integer scalar
  *           X derivative of piecewise polynomial to evaluate.

  *   IDY     Integer scalar
  *           Y derivative ofON

  *   B3VAL evaluates the tensor product piecewise polynomial interpolant
  *   constructed by the routine B3INK or one of its derivatives  at  the
  *   point (XVAL,YVAL,ZVAL). To evaluate  the  interpolant  itself,  set
  *   IDX=IDY=IDZ=0, to evaluate the first partial with respect to x, set
  *   IDX=1,IDY=IDZ=0, and so on.

  *   B3VAL returns 0.0E0 if (XVAL,YVAL,ZVAL) is out of range. That is,
  *            XVAL.LT.TX(1) .OR. XVAL.GT.TX(NX+KX) .OR.
  *            YVAL.LT.TY(1) .OR. YVAL.GT.TY(NY+NY) .OR.
  *            ZVAL.LT.TZ(1) .OR. ZVAL.GT.TZ(NZ+KZ)
  *   If the knots TX, TY, and TZ were chosen  by  B3INK,  then  this  is
  *   equivalent to
  *            XVAL.LT.X(1) .OR. XVAL.GT.X(NX)+EPSX .OR.
  *            YVAL.LT.Y(1) .OR. YVAL.GT.Y(NY)+EPSY .OR.
  *            ZVAL.LT.Z(1) .OR. ZVAL.GT.Z(NZ)+EPSZ
  *   where EPSX = 0.1*(X(NX)-X(NX-1)), EPSY =  0.1*(Y(NY)-Y(NY-1)),  and
  *   EPSZ = 0.1*(Z(NZ)-Z(NZ-1)).

  *   The input quantities TX, TY, TZ, NX, NY, NZ, KX, KY, KZ, and  BCOEF
  *   should remain unchanged since the last call of B3INK.


  *   I N P U T
  *   ---------

  *   XVAL    Real scalar
  *           X coordinate of evaluation point.

  *   YVAL    Real scalar
  *           Y coordinate of evaluation point.

  *   ZVAL    Real scalar
  *           Z coordinate of evaluation point.

  *   IDX     Integer scalar
  *           X derivative of piecewise polynomial to evaluate.

  *   IDY     Integer scalar
  *           Y derivative of piecewise polynomial to evaluate.

  *   IDZ     Integer scalar
  *           Z derivative of piecewise polynomial to evaluate.

  *   TX      Real 1D array (size NX+KX)
  *           Sequence of knots defining the piecewise polynomial in
  *           the x direction.  (Same as in last call to B3INK.)

  *   TY      Real 1D array (size NY+KY)
  *           Sequence of knots defining the piecewise polynomial in
  *           the y direction.  (Same as in last call to B3INK.)

  *   TZ      Real 1D array (size NZ+KZ)
  *           Sequence of knots defining the piecewise polynomial in
  *           the z direction.  (Same as in last call to B3INK.)

  *   NX      Integer scalar
  *           The number of interpolation points in x.
  *           (Same as in last call to B3INK.)

  *   NY      Integer scalar
  *           The number of interpolation points in y.
  *           (Same as in last call to B3INK.)

  *   NZ      Integer scalar
  *           The number of interpolation points in z.
  *           (Same as in last call to B3INK.)

  *   KX      Integer scalar
  *           Order of polynomial pieces in x.
  *           (Same as in last call to B3INK.)

  *   KY      Integer scalar
  *           Order of polynomial pieces in y.
  *           (Same as in last call to B3INK.)

  *   KZ      Integer scalar
  *           Order of polynomial pieces in z.
  *           (Same as in last call to B3INK.)

  *   BCOEF   Real 2D array (size NX by NY by NZ)
  *           The B-spline coefficients computed by B3INK.

  *   WORK    Real 1D array (size KY*KZ+3*max(KX,KY,KZ)+KZ)
  *           A working storage array.

  * ***REFERENCES  CARL DE BOOR, A PRACTICAL GUIDE TO SPLINES,
  *                 SPRINGER-VERLAG, NEW YORK, 1978.
  * ***ROUTINES CALLED  INTRV,BVALU
  * ***END PROLOGUE  B3VAL
  */
  extern double b3val_( double *xval, double *yval, double *zval, int *idx, int *idy, int *idz, double *tx, double *ty, double *tz, int *nx, int *ny, int *nz, int *kx, int *ky, int *kz, double *bcoef, double *work );

  /** ***BEGIN PROLOGUE  BTPCF
  * ***REFER TO  B2INK,B3INK
  * ***ROUTINES CALLED  BINTK,BNSLV
  * ***END PROLOGUE  BTPCF
  *  -----------------------------------------------------------------
  *  BTPCF COMPUTES B-SPLINE INTERPOLATION COEFFICIENTS FOR NF SETS
  *  OF DATA STORED IN THE COLUMNS OF THE ARRAY FCN. THE B-SPLINE
  *  COEFFICIENTS ARE STORED IN THE ROWS OF BCOEF HOWEVER.
  *  EACH INTERPOLATION IS BASED ON THE N ABCISSA STORED IN THE
  *  ARRAY X, AND THE N+K KNOTS STORED IN THE ARRAY T. THE ORDER
  *  OF EACH INTERPOLATION IS K. THE WORK ARRAY MUST BE OF LENGTH
  *  AT LEAST 2*K*(N+1).
  *  -----------------------------------------------------------------
    extern int btpcf_( double *x, int *n, double *fcn, int *ldf, int *nf, double *t, int *k, double *bcoef, double *work );

  * ***BEGIN PROLOGUE  BKNOT
  * ***REFER TO  B2INK,B3INK
  * ***ROUTINES CALLED  (NONE)
  * ***END PROLOGUE  BKNOT

  *  --------------------------------------------------------------------
  *  BKNOT CHOOSES A KNOT SEQUENCE FOR INTERPOLATION OF ORDER K AT THE
  *  DATA POINTS X(I), I=1,..,N.  THE N+K KNOTS ARE PLACED IN THE ARRAY
  *  T.  K KNOTS ARE PLACED AT EACH ENDPOINT AND NOT-A-KNOT END
  *  CONDITIONS ARE USED.  THE REMAINING KNOTS ARE PLACED AT DATA POINTS
  *  IF N IS EVEN AND BETWEEN DATA POINTS IF N IS ODD.  THE RIGHTMOST
  *  KNOT IS SHIFTED SLIGHTLY TO THE RIGHT TO INSURE PROPER INTERPOLATION
  *  AT X(N) (SEE PAGE 350 OF THE REFERENCE).
  *  --------------------------------------------------------------------
  */
  extern int bknot_( double *x, int *n, int *k, double *t );

  extern int xerror_( char *messg, int *nmessg, int *nerr, int *level, int messg_len );

  /** ***BEGIN PROLOGUE  BVALU
  * ***DATE WRITTEN   800901   (YYMMDD)
  * ***REVISION DATE  820801   (YYMMDD)
  * ***CATEGORY NO.  E3,K6
  * ***KEYWORDS  B-SPLINE,DATA FITTING,INTERPOLATION,SPLINE
  * ***AUTHOR  AMOS, D. E., (SNLA)
  * ***PURPOSE  Evaluates the B-representation of a B-spline at X for the
  *            function value or any of its derivatives.
  * ***DESCRIPTION

  *     Written by Carl de Boor and modified by D. E. Amos

  *     Reference
  *         SIAM J. Numerical Analysis, 14, No. 3, June, 1977, pp.441-472.

  *     Abstract
  *         BVALU is the BVALUE function of the reference.

  *         BVALU evaluates the B-representation (T,A,N,K) of a B-spline
  *         at X for the function value on IDERIV = 0 or any of its
  *         derivatives on IDERIV = 1,2,...,K-1.  Right limiting values
  *         (right derivatives) are returned except at the right end
  *         point X=T(N+1) where left limiting values are computed.  The
  *         spline is defined on T(K) .LE. X .LE. T(N+1).  BVALU returns
  *         a fatal error message when X is outside of this interval.

  *         To compute left derivatives or left limiting values at a
  *         knot T(I), replace N by I-1 and set X=T(I), I=K+1,N+1.

  *         BVALU calls INTRV

  *     Description of Arguments
  *         Input
  *          T       - knot vector of length N+K
  *          A       - B-spline coefficient vector of length N
  *          N       - number of B-spline coefficients
  *                    N = sum of knot multiplicities-K
  *          K       - order of the B-spline, K .GE. 1
  *          IDERIV  - order of the derivative, 0 .LE. IDERIV .LE. K-1
  *                    IDERIV=0 returns the B-spline value
  *          X       - argument, T(K) .LE. X .LE. T(N+1)
  *          INBV    - an initialization parameter which must be set
  *                    to 1 the first time BVALU is called.

  *         Output
  *          INBV    - INBV contains information for efficient process-
  *                    ing after the initial call and INBV must not
  *                    be changed by the user.  Distinct splines require
  *                    distinct INBV parameters.
  *          WORK    - work vector of length 3*K.
  *          BVALU   - value of the IDERIV-th derivative at X

  *     Error Conditions
  *         An improper input is a fatal error
  * ***REFERENCES  C. DE BOOR, *PACKAGE FOR CALCULATING WITH B-SPLINES*,
  *                 SIAM JOURNAL ON NUMERICAL ANALYSIS, VOLUME 14, NO. 3,
  *                 JUNE 1977, PP. 441-472.
  * ***ROUTINES CALLED  INTRV,XERROR
  * ***END PROLOGUE  BVALU
  */
  extern double bvalu_( double *t, double *a, int *n, int *k, int *ideriv, double *x, int *inbv, double *work );

#ifdef __cplusplus
}
#endif

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#endif  // #ifndef BSPLINES_H

