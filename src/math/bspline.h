/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 
 Header:       bspline.h
 Author:       Matthew Gong
 Date started: 11/10/2005
 
 ------ Copyright (C) 2005 Matthew Gong (matthewzhenggong@yahoo.com.cn) ------
 
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
11/10/2005  Matt   Created based on Gary Granger's frame (http://www.atd.ucar.edu/~granger)
 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/** 
 * @file bspline.h 
 * Provides a class to manage a B-Spline object.
 * Use it to create a B-Spline, and find out points on it.
 * And you can 'sample it' to get a similar polyline
 */

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
SENTRY
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#ifndef MATT_BSPLINE_H
#   define MATT_BSPLINE_H

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#   include <vector>

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
DEFINITIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
NAMESPACE
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/**
 * Matt's Programmes are all in the Matt's namespace.
 */
   
namespace Matt
{

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
FORWARD DECLARATIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DOCUMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/**
 * @class BSpline
 *
 * The class for a spline object containing the nodes for a given
 * domain.
 *
 * The ok() method returns false when a BSpline could not
 * complete any operation successfully.  In particular, as mentioned above,
 * ok() will return false if some problem was detected with the domain
 * values or if no reasonable number of nodes could be found.
 * The interface for the BSpline and BSpline templates is defined in 
 * the header file bspline.h.  The implementation is defined in bspline.cpp.
 * program frame is based on : Gary Granger (http://www.atd.ucar.edu/~granger)
 *                            Copyright (c) 1998-2003
 *                      University Corporation for Atmospheric Research, UCAR
 */

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DECLARATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

  template < class T > class BSpline
  {
  public:
    /**
     * @class point
     *
     * The class stands for a node point.
     *
     * This class is in the template class BSpline.
     * It mantain the coordinate x & y for the node.
     **/
    class point
    {
    public:
      /**
       * Coordinate X
       *
       * The coordinate x of the node.
       **/
      T x;

      /**
       * Coordinate Y
       *
       * The coordinate y of the node.
       **/
      T y;

      /**
       * Default Constructor
       *
       * The default constructor. The default pos is (0,0).
       **/
        point ():x (0), y (0)
      {
      }

      point & operator += (const point & b)
      {
        this->x += b.x;
        this->y += b.y;
        return *this;
      }
      
      point & operator -= (const point & b)
      {
        this->x -= b.x;
        this->y -= b.y;
        return *this;
      }

      point operator + (const point & b) const
      {
        point a = *this;
          a.x += b.x;
          a.y += b.y;
          return a;
      }
      
      point operator - (const point & b) const
      {
        point a = *this;
          a.x -= b.x;
          a.y -= b.y;
          return a;
      }
      
      point operator * (const T & b) const
      {
        point a = *this;
          a.x *= b;
          a.y *= b;
          return a;
      }
      
      point operator / (const T & b) const
      {
        point a = *this;
          a.x /= b;
          a.y /= b;
          return a;
      }
    };

    /**
     * @typedef Value List.
     *
     * Use std::vector to maintain the value list.
     */
    typedef std::vector < T > TLIST;
    
    /**
     * @typedef Point List.
     *
     * Use std::vector to maintain the point list.
     */
    typedef std::vector < point > PLIST;

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
    BSpline (const T * x, size_t nx, const T * y);

    
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
    BSpline (TLIST x, TLIST y);

    /**
     * The default construct a spline domain.
     * Call setDomain() later to setup B-spline.
     *
     * the state will be set to not ok.
     *
     * @see setDomain(const T * x, size_t nx, const T * y).
     */
    BSpline ();

    /**
     * Call this class method with a value greater than zero to enable
     * debug messages, or with zero to disable messages.  Calling with
     * no arguments returns true if debugging enabled, else false.
     */
    static bool Debug (int on = -1);

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
    bool setDomain (const T * x, size_t nx, const T * y);
    bool setDomain (const std::vector<double> &x, const std::vector<double> &y);

    /**
     * Change the domain of this base after changing the nodes' pos.  
     * The setup of the domain may fail because  the resulting matrix
     * could not be factored.  If setup fails, the method returns false.
     */
    bool setDomain ();

    /**
     * Sample to generate sample_list.
     * Get a similar polyline instead of the B-spline.
     *
     * @param s	 How many interpolating points will be generated
     *           between two nodes.
     *
     */
    bool sample (size_t s = 15u);

    /**
     * Return the evaluation of the sample curve 
     * at a particular @p x value.  If current state is not ok(), returns 0.
     * for big number of evaluation. for speed.
     *
     * @param x	 evaluate at the @p x value.
     */
    T evaluate (T x);

    /**
     * Return the number of sample list.
     * If not sampled, it will sample first.
     * If not ok, it will return zero.
     */
    size_t nX ()
    {
      if (!sampled && !sample ())
        return 0;
      return sample_list.size ();
    }

    /**
     * Return the x values at i-th node in the sample list.
     * If not sampled, it will sample first.
     * If not ok, it will return zero.
     *
     * @param i	 evaluate at the @p i-th node.
     */
    T X (size_t i)
    {
      if (!sampled && !sample ())
        return 0;
      return sample_list[i].x;
    }

    /**
     * Return the y values at i-th node in the sample list.
     * If not sampled, it will sample first.
     * If not ok, it will return zero.
     *
     * @param i	 evaluate at the @p i-th node.
     */
    T Y (size_t i)
    {
      if (!sampled && !sample ())
        return 0;
      return sample_list[i].y;
    }

    /**
     * Return the minimum value of y.
     * If not sampled, it will the minimum value of y in the node list.
     * Else, it will the minimum value of y in the sample list.
     */
    T Ymin ()
    {
      return ymin;
    }

    T X_ymin ()
    {
      return x_ymin;
    }

    /**
     * Return the maximum value of y.
     * If not sampled, it will the maximum value of y in the node list.
     * Else, it will the maximum value of y in the sample list.
     */
    T Ymax ()
    {
      return ymax;
    }

    T X_ymax ()
    {
      return x_ymax;
    }

    /**
     * Return the minimum value of x.
     * If not sampled, it will the minimum value of x in the node list.
     * Else, it will the minimum value of x in the sample list.
     */
    T Xmin ()
    {
      return xmin;
    }

    /**
     * Return the maximum value of x.
     * If not sampled, it will the maximum value of x in the node list.
     * Else, it will the maximum value of x in the sample list.
     */
    T Xmax ()
    {
      return xmax;
    }

    /**
     * Return the evaluation of the smoothed curve 
     * at a particular @p x value.  If current state is not ok(), returns 0.
     */
    T evaluate2 (T x);

    /**
     * Return the current state of the object, either ok or not ok.
     * Use this method to test for valid state after construction or after
     * a call to setDomain().  ok() will return false if either fail, such
     * as when an appropriate number of nodes and node interval cannot be
     * found for a given wavelength, or when the linear equation for the
     * coefficients cannot be solved.
     */
    bool ok () const
    {
      return OK;
    }

    /**
     * Return the number of node list.
     */
    size_t nNode () const
    {
      return node_list.size ();
    }

    /**
     * Return the x value at i-th point in the node list.
     * The i-th point should be in the node list.
     *
     * @param i	 x value at the @p i-th node.
     */
    T xNode (size_t i) const
    {
      return node_list[i].x;
    }

    /**
     * Set the x value at i-th point in the node list.
     * The i-th point should be in the node list.
     * Need to setDomain again.
     * Need to sample again.
     *
     * @param i	 x value at the @p i-th node.
     */
    T xNode (size_t i, T x)
    {
      T tmp = node_list[i].x;

      node_list[i].x = x;
      OK = false;
      sampled = false;
      return tmp;
    }

    /**
     * Return the y value at i-th point in the node list.
     * The i-th point should be in the node list.
     *
     * @param i	 y value at the @p i-th node.
     */
    T yNode (size_t i) const
    {
      return node_list[i].y;
    }

    /**
     * Set the y value at i-th point in the node list.
     * The i-th point should be in the node list.
     * Need to setDomain again.
     * Need to sample again.
     *
     * @param i	 y value at the @p i-th node.
     */
    T yNode (size_t i, T y)
    {
      T tmp = node_list[i].y;

      node_list[i].y = y;
      OK = false;
      sampled = false;
      return tmp;
    }

    /**
     * Return the number of vertex list.
     */
    size_t nVertex () const
    {
      return vertex_list.size ();
    }

   /**
     * Return the x value of the i-th vertex.
     *
     * @param i Stand for the @p i-th vertex.
     */
    T xVertex (size_t i) const
    {
      return vertex_list[i].x;
    }

   /**
     * Return the y value of the i-th vertex.
     *
     * @param i Stand for the @p i-th vertex.
     */
    T yVertex (size_t i) const
    {
      return vertex_list[i].y;
    }


    void SetIter2(const size_t & i ){icur2 = i;}

  protected:

   /**
     * Node list.
     *
     * Maintain the input node list.
     */
    PLIST node_list;

   /**
     * The direction of the curve at the start point.
     *
     * It is set to zero point right now.
     */
    point pdot_s;
    
   /**
     * The direction of the curve at the end point.
     *
     * It is set to zero point right now.
     */
    point pdot_e;

   /**
     * Vertex list.
     *
     * Maintain the vertex list.
     */
    PLIST vertex_list;
    size_t icur2;

   /**
     * sample list.
     *
     * Maintain the polyline similar to the B-spline.
     */
    PLIST sample_list;
    size_t icur;

    
    T xmax;
    T xmin;
    T ymax;
    T ymin;
    T x_ymax;
    T x_ymin;

   /**
     * State flag.
     *
     * Maintain the state for the B-spline.
     * 
     * @see ok().
     */
    bool OK;

   /**
     * State flag whether sampled or not.
     *
     * Maintain the state of sample.
     */
    bool sampled;

   /**
     * Calculate the value of pdot_s and pdot_e.
     *
     * @see pdot_s
     * @see pdot_e
     */
    bool calculate_pdot (void);

   /**
     * Calculate the vertex list.
     */
    bool calculate_vertex_list (void);
  };

  /**
   * @typedef the double type of B-spline.
   *
   * If use BsplineD only. You can include file bspline.h only.
   * Otherwise, you should include bspline.cpp too.
   */
  typedef BSpline < double > BSplineD;

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
}                               // namespace Matt

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#endif // MATT_BSPLINE_H
