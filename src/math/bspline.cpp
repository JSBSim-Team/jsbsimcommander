/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 Module:       bspline.cpp
 Author:       Matthew Gong
 Date started: 11/10/2005
 Purpose:      a B-spline object to draw function
 Called by:    

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

FUNCTIONAL DESCRIPTION
--------------------------------------------------------------------------------
  This file defines the implementation for the BSpline templates.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
COMMENTS, REFERENCES,  and NOTES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 If we're on WIN32 and the DLL macro has not been set yet, which implies the interface file (BSpline.h) has not been included yet, assume only the implementation is wanted and empty the DLL macro before including the interface.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include <vector>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <cmath>

#include "bspline.h"
#include "math/Matt1D.h"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
DEFINITIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#if WIN32
#   ifndef BSPLINE_DLL_
#      define BSPLINE_DLL_
#   endif
#endif /* WIN32 */

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
NAMESPACE
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

namespace Matt
{
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
GLOBAL DATA
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

    
/**
 * A static value of 1/6
 */
  const double one_sixth = 1.0 / 6.0;

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS IMPLEMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

    template < class T > inline bool BSpline < T >::Debug (int on)
  {
    static bool debug = false;
    if (on >= 0)
        debug = (on > 0);
      return debug;
  }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


  template < class T >
    BSpline < T >::BSpline (const T * x, size_t nx, const T * y):icur2 (0),
    icur (0), OK (false), sampled (false)
  {
    setDomain (x, nx, y);
  }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


  template < class T >
    BSpline < T >::BSpline (std::vector<T> x, std::vector<T> y):icur2 (0),
    icur (0), OK (false), sampled (false)
  {
    setDomain (x, y);
  }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


template < class T > BSpline < T >::BSpline ():
  icur2 (0), icur (0), OK (false), sampled (false)
  {
  }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  template < class T >
    bool BSpline < T >::setDomain (const T * x, size_t nx, const T * y)
  {
    /*
    const T *x1 = x, *x2 = x + 1;

    for (size_t i = 0; i < nx - 1; ++i, ++x1, ++x2)
    {
      if (*x1 >= *x2)
      {
        if (Debug ())
        {
          std::
            cerr << "The x values must be in ascending order." << std::endl;
        }
        return false;
      }
    }
    */
    OK = false;
    sampled = false;

    // Copy the array into our storage.
    try
    {
      node_list.resize (nx);
      const T *x1 = x, *x2 = y;
      const T *y1 = x+nx-1, *y2 = y+nx-1;
      if (x1 < y1)
      {
        for (size_t i = 0; i < nx; ++i, ++x1, ++x2)
        {
          node_list[i].x = *x1;
          node_list[i].y = *x2;
        }
      }
      else
      {
        for (size_t i = 0; i < nx; ++i, --y1, --y2)
        {
          node_list[i].x = *y1;
          node_list[i].y = *y2;
        }      
      }
    }
    catch (...)
    {
      if (Debug ())
      {
        std::
          cerr <<
          "fail to copy the array into our storage. x & y must be in the same size."
          << std::endl;
      }
      return false;
    }
    if (nx < 2u)
    {
      if (Debug ())
      {
        std::
          cerr << "fail to set domain when node is less than 2." << std::endl;
      }
      return false;
    }

    return setDomain ();
  }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  template < class T >
    bool BSpline < T >::setDomain(const std::vector<double> &x, const std::vector<double> &y)
  {
    /*
    const T *x1 = x, *x2 = x + 1;

    for (size_t i = 0; i < nx - 1; ++i, ++x1, ++x2)
    {
      if (*x1 >= *x2)
      {
        if (Debug ())
        {
          std::
            cerr << "The x values must be in ascending order." << std::endl;
        }
        return false;
      }
    }
    */
    OK = false;
    sampled = false;

    // Copy the array into our storage.
    try
    {
      node_list.resize (x.size());
      std::vector<double>::const_iterator x1= x.begin();
      std::vector<double>::const_iterator x2= y.begin();
      std::vector<double>::const_reverse_iterator y1= x.rbegin();
      std::vector<double>::const_reverse_iterator y2= y.rbegin();
      if (*x1 < *y1)
      {
        for (size_t i=0u; x1 != x.end(); ++i, ++x1, ++x2)
        {
          node_list[i].x = *x1;
          node_list[i].y = *x2;
        }
      }
      else
      {
        for (size_t i=0u; y1 != x.rend(); ++i, ++y1, ++y2)
        {
          node_list[i].x = *y1;
          node_list[i].y = *y2;
        }      
      }
    }
    catch (...)
    {
      if (Debug ())
      {
        std::
          cerr <<
          "fail to copy the array into our storage. x & y must be in the same size."
          << std::endl;
      }
      return false;
    }
    if (x.size() < 2u)
    {
      if (Debug ())
      {
        std::
          cerr << "fail to set domain when node is less than 2." << std::endl;
      }
      return false;
    }

    return setDomain ();
  }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  template < class T > bool BSpline < T >::setDomain (void)
  {
    OK = false;
    sampled = false;

    if (!calculate_pdot ())
    {
      if (Debug ())
      {
        std::cerr << "fail to calculate pdot." << std::endl;
      }
      return false;
    }

    // Now perform the LU factorization on Q
    if (!calculate_vertex_list ())
    {
      if (Debug ())
      {
        std::cerr << "fail to calculate vertex list." << std::endl;
      }
      return false;
    }

    bool first = true;

    for (size_t i = 0; i < node_list.size (); ++i)
    {
      if (first)
      {
        first = false;
        xmax = xmin = x_ymax = x_ymin = node_list[i].x;
        ymax = ymin = node_list[i].y;
      }
      else
      {
        if (node_list[i].y > ymax)
        {
          ymax = node_list[i].y;
          x_ymax = node_list[i].x;
        }
        else if (node_list[i].y < ymin)
        {
          ymin = node_list[i].y;
          x_ymin = node_list[i].x;
        }
        if (node_list[i].x > xmax)
        {
          xmax = node_list[i].x;
        }
        else if (node_list[i].x < xmin)
        {
          xmin = node_list[i].x;
        }
      }
    }
    return (OK = true);
  }


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/**
 * Calculate the P_s_dot and P_e_dot
 */
  template < class T > bool BSpline < T >::calculate_pdot (void)
  {
    if (node_list.size () == 1u)
    {
      if (Debug ())
      {
        std::cerr << "there's only 1 points. Error!" << std::endl;
      }
      return false;
    }
    else if (node_list.size () == 2u)
    {
      //there're only 2 points.
      const T & x0 = node_list[0].x;
      const T & y0 = node_list[0].y;
      const T & x1 = node_list[1].x;
      const T & y1 = node_list[1].y;
      T m0 = (y1 - y0) / (x1 - x0);
      T m = std::sqrt (1 + m0 * m0);

      pdot_s.x = 1 / m;
      pdot_s.y = m0 / m;
      pdot_e = pdot_s;
    }
    else
    {
      //there're more than 2 points.
      const T & x0 = node_list[0].x;
      const T & y0 = node_list[0].y;
      const T & x1 = node_list[1].x;
      const T & y1 = node_list[1].y;
      const T & x2 = node_list[2].x;
      const T & y2 = node_list[2].y;
      T m0 = y0 * (2 * x0 - x1 - x2) / ((x0 - x1) * (x0 - x2))
        + y1 * (x0 - x2) / ((x1 - x0) * (x1 - x2))
        + y2 * (x0 - x1) / ((x2 - x0) * (x2 - x1));
      T m = std::sqrt (1 + m0 * m0);

      //TODO
      //pdot_s.x = 1 / m;
      //pdot_s.y = m0 / m;
      pdot_s.x = 0;
      pdot_s.y = 0;
      size_t n = node_list.size () - 1;
      const T & xn0 = node_list[n].x;
      const T & yn0 = node_list[n].y;
      const T & xn1 = node_list[n - 1].x;
      const T & yn1 = node_list[n - 1].y;
      const T & xn2 = node_list[n - 2].x;
      const T & yn2 = node_list[n - 2].y;

      m0 = yn0 * (2 * xn0 - xn1 - xn2) / ((xn0 - xn1) * (xn0 - xn2))
        + yn1 * (xn0 - xn2) / ((xn1 - xn0) * (xn1 - xn2))
        + yn2 * (xn0 - xn1) / ((xn2 - xn0) * (xn2 - xn1));
      m = std::sqrt (1 + m0 * m0);
      //TODO
      //pdot_e.x = 1 / m;
      //pdot_e.y = m0 / m;
      pdot_e.x = 0;
      pdot_e.y = 0;
    }

    return true;
  }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/**
 * Calculate the vertex list
 * Solve function like this : @n
 * | 2/6  4/6                      | |V(0)| |P(0)-P_s_dot/3| @n
 * | 1/6  4/6  1/6                 | |V(1)| |P(0)          | @n
 * |      1/6  4/6  1/6            |*|V(2)|=|P(1)          | @n
 * |           1/6  4/6  1/6       | |V(3)| |P(2)          | @n
 * |                1/6  4/6  1/6  | |V(4)| |P(3)          | @n
 * |                     4/6  2/6  | |V(5)| |P(3)-P_e_dot/3| @n
 */
  template < class T > bool BSpline < T >::calculate_vertex_list (void)
  {
    size_t s1 = node_list.size ();
    size_t n = s1 + 2;

    vertex_list.resize (n);
    TLIST r;
    PLIST y, d;

    r.resize (n);
    y.resize (n);
    d.resize (n);

    //load d vector
    d[0] = node_list[0] - pdot_s / 3.0;
    for (size_t i = 0; i < s1; ++i)
    {
      d[i + 1] = node_list[i];
    }
    d[n - 1] = node_list[s1 - 1] + pdot_e / 3.0;

    //LU matrix coeff
    T c1, ci, ai, an, b1, bi, bn;

    ai = ci = one_sixth;
    an = c1 = bi = 4 * one_sixth;
    b1 = bn = 2 * one_sixth;

    r[1] = c1 / b1;
    y[0] = d[0] / b1;

    for (size_t k = 2; k <= n - 1; ++k)
    {
      T tmp = bi - r[k - 1] * ai;

      r[k] = ci / tmp;
      y[k - 1] = (d[k - 1] - y[k - 2] * ai) / tmp;
    }
    y[n - 1] = (d[n - 1] - y[n - 2] * an) / (bn - r[n - 1] * an);

    vertex_list[n - 1] = y[n - 1];
    for (int k = n - 1; k >= 1; --k)
    {
      vertex_list[k - 1] = y[k - 1] - vertex_list[k] * r[k];
    }

    return true;
  }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  template < class T > bool BSpline < T >::sample (size_t s)
  {
    if (!OK)
      return false;

    if (s < 2)
      s = 2;
    T delta = T (s);

    delta = 1.0 / delta;
    sample_list.clear ();
    bool first = true;

    size_t n = vertex_list.size ();

    for (size_t i = 0; i <= n - 4; ++i)
    {
      T u = 0;

      for (T j = 0; j < s; ++j)
      {
        T u2 = u * u;
        T u3 = u2 * u;
        T c[4];

        c[0] = (1 - 3 * u + 3 * u2 - u3) * one_sixth;
        c[1] = (4 - 6 * u2 + 3 * u3) * one_sixth;
        c[2] = (1 + 3 * u + 3 * u2 - 3 * u3) * one_sixth;
        c[3] = (u3) * one_sixth;
        const point & V0 = vertex_list[i];
        const point & V1 = vertex_list[i + 1];
        const point & V2 = vertex_list[i + 2];
        const point & V3 = vertex_list[i + 3];
        point p = V0 * c[0] + V1 * c[1] + V2 * c[2] + V3 * c[3];

        sample_list.push_back (p);
        if (first)
        {
          first = false;
          xmax = xmin = x_ymax = x_ymin = p.x;
          ymax = ymin = p.y;
        }
        else
        {
          if (p.y > ymax)
          {
            ymax = p.y;
            x_ymax = p.x;
          }
          else if (p.y < ymin)
          {
            ymin = p.y;
            x_ymin = p.x;
          }
          if (p.x > xmax)
          {
            xmax = p.x;
          }
          else if (p.x < xmin)
          {
            xmin = p.x;
          }
        }
        u += delta;
      }
    }
    sample_list.push_back (node_list.back ());
    icur = 0u;
    icur2 = 0u;
    sampled = true;

    return true;
  }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  template < class T > T BSpline < T >::evaluate (T x)
  {
    if (!OK)
      return 0;

    if (sampled)
    {
      while (sample_list[icur].x > x && icur > 0u)
        --icur;
      size_t n = sample_list.size ();

      while (sample_list[icur + 1].x < x && icur + 1 < n - 1)
        ++icur;
      T x1 = sample_list[icur].x;
      T x2 = sample_list[icur + 1].x;
      T y1 = sample_list[icur].y;
      T y2 = sample_list[icur + 1].y;
      T scale = (x - x1) / (x2 - x1);

      return y1 + scale * (y2 - y1);
    }
    else
    {
      while (node_list[icur].x > x && icur > 0u)
        --icur;
      size_t n = node_list.size ();

      while (node_list[icur + 1].x < x && icur + 1 < n - 1)
        ++icur;
      T x1 = node_list[icur].x;
      T x2 = node_list[icur + 1].x;
      T y1 = node_list[icur].y;
      T y2 = node_list[icur + 1].y;
      T scale = (x - x1) / (x2 - x1);

      return y1 + scale * (y2 - y1);
    }
  }


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  static double V0, V1, V2, V3, DX;
  static void gunshooting (unsigned ndim, const double x[], double &fx)
  {
    double u = x[0];
    double u2 = u * u;
    double u3 = u2 * u;
    double c[4];

    c[0] = (1 - 3 * u + 3 * u2 - u3) * one_sixth;
    c[1] = (4 - 6 * u2 + 3 * u3) * one_sixth;
    c[2] = (1 + 3 * u + 3 * u2 - 3 * u3) * one_sixth;
    c[3] = (u3) * one_sixth;
    double tx = V0 * c[0] + V1 * c[1] + V2 * c[2] + V3 * c[3];

    fx = tx - DX;
    fx *= fx;
  }


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  template < class T > T BSpline < T >::evaluate2 (T x)
  {
    if (!OK)
      return 0;

    while (node_list[icur2].x > x && icur2 > 0u)
      --icur2;

    size_t n = node_list.size ();

    while (node_list[icur2 + 1].x < x && icur2 + 1 < n - 1)
      ++icur2;

    T rslt;

    const T & x1 = node_list[icur2].x;
    const T & x2 = node_list[icur2 + 1].x;
    T u = (x - x1) / (x2 - x1);

    if (u <= 0 || u >= 1)
    {
      rslt =
        node_list[icur2].y + u * (node_list[icur2 + 1].y -
                                  node_list[icur2].y);
    }
    else
    {
      MattOpt1D objfcn (gunshooting, u, 0, 1, MattOpt1D::tDot618M);

      V0 = vertex_list[icur2].x;
      V1 = vertex_list[icur2 + 1].x;
      V2 = vertex_list[icur2 + 2].x;
      V3 = vertex_list[icur2 + 3].x;
      DX = x;
      objfcn.SetMaxStep (0.05);
      objfcn.SetMinStep (1e-7);
      objfcn.SetMaxFeval (200);
      objfcn.optimize ();
      T u0 = u;
      u = objfcn.getX ();
      T f = objfcn.getF();
      if (std::abs(f) > 1e-6)
      {
        std::cerr << "fail to find u" << u << '\t'  << u0 << '\t' << f << '\n';
      }
      T u2 = u * u;
      T u3 = u2 * u;
      T c[4];

      c[0] = (1 - 3 * u + 3 * u2 - u3) * one_sixth;
      c[1] = (4 - 6 * u2 + 3 * u3) * one_sixth;
      c[2] = (1 + 3 * u + 3 * u2 - 3 * u3) * one_sixth;
      c[3] = (u3) * one_sixth;
      rslt = vertex_list[icur2].y * c[0]
        + vertex_list[icur2 + 1].y * c[1]
        + vertex_list[icur2 + 2].y * c[2] + vertex_list[icur2 + 3].y * c[3];
    }
    return rslt;
  }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    
/**
 * @example bspline_test.cpp
 * This is an example of how to use the BSpline class.
 * More details about this example. 
 */

  template class BSpline < double >;

}                               // namespace Matt
