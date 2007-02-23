/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 Header:       Matt1D.cpp
 Author:       Matthew Gong
 Date started: 08/09/2005

 ---- Copyright (C) 2005  Matthew Gong (matthewzhenggong@yahoo.com.cn) --------

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

This class wraps up the 1D optimizing mothods.

HISTORY
--------------------------------------------------------------------------------
08/09/05   Matt   Created

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
COMMENTS, REFERENCES,  and NOTES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include <vector>
#include <cmath>

#include "Matt1D.h"

namespace Matt
{

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
GLOBAL DECLARATIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS IMPLEMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

MattOpt1D::MattOpt1D():
    m_origin( 0 ),
    m_max( 1 ),
    m_min( -1 ),
    m_gun( 0ul ),
    m_method( tDot618M ),
    m_func( 0 ),
    m_x( 0 ),
    m_maxstep( 0.5 ),
    m_minstep( 0.01 ), m_maxfeval( 50 ), m_upper( 1 ), m_lower( -1 ), m_feval( 0 )
{}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

MattOpt1D::MattOpt1D( Matt1DGun gun,
                      const double &origin, const double &max,
                      const double &min, const METHOD & method ):
    m_origin( origin ),
    m_max( max ),
    m_min( min ),
    m_gun( gun ),
    m_method( method ),
    m_func( 0 ),
    m_x( 0 ),
    m_maxstep( 0.5 ),
    m_minstep( 0.01 ),
    m_maxfeval( 50 ),
    m_upper( max ),
    m_lower( min ),
    m_feval( 0 )
{}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int
MattOpt1D::optimize()
{
  int result;

  m_feval = 0;
  result = searchBrackets();
  if ( result == 1 )
  {
    return result;
  }
  else if ( result != 0 )
  {
    m_upper = m_max;
    m_lower = m_min;
  }
  else if ( std::abs( m_upper - m_lower ) < m_minstep )
  {
    return result;
  }
  switch ( m_method )
  {
  case tLinear:
    result = optLinear();
    break;
  case tDot618M:
    result = optDot618M();
    break;
  case tFibonacci:
    result = optFibonacci();
    break;
  case tParabola:
    result = optParabola();
    break;
  }
  return result;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int
MattOpt1D::searchBrackets()
{
  if ( m_method == tLinear )
  {
    return 2;
  }

  int dir = 1;                  //direction
  double h = m_maxstep;
  double x1 = m_origin;
  double x2 = x1 + h;

  if ( x2 > m_max )
    x2 = m_max;
  double x3;
  double f1, f2, f3;

  ( *m_gun )( 1, &x1, m_func );
  f1 = m_func;
  ( *m_gun )( 1, &x2, m_func );
  f2 = m_func;
  m_feval += 2u;

  if ( f1 <= f2 )
  {
    x3 = x2;                    // backup x2 value
    h = -h;                     // backward
    dir = 2;                    // set backward flag
    x2 = x1 + h;                // new x2 value
    if ( x2 < m_min )
      x2 = m_min;
    ( *m_gun )( 1, &x2, m_func );
    f2 = m_func;
    ++m_feval;
    if ( f1 <= f2 )             // I find the brackets
    {
      m_upper = x3;
      m_lower = x2;
      return 0;
    }
  }

  bool flag = true;
  int result = -1;

  while ( m_feval < m_maxfeval && flag )
  {
    h += h;                     // double searching range
    x3 = m_origin + h;
    if ( x3 > m_max )
    {
      x3 = m_max;
      flag = false;
      m_x = x3;
    }
    else if ( x3 < m_min )
    {
      x3 = m_min;
      flag = false;
    }
    ( *m_gun )( 1, &x3, m_func );
    f3 = m_func;
    ++m_feval;
    if ( !flag )
    {
      m_x = x3;
      m_func = f3;
    }
    if ( f3 < f2 )              // go on searching
    {
      x1 = x2;
      f1 = f2;
      x2 = x3;
      f2 = f3;
    }
    else                        // find brackets
    {
      if ( dir == 1 )
      {
        m_lower = x1;
        m_upper = x3;
      }
      else
      {
        m_lower = x3;
        m_upper = x1;
      }
      result = 0;
      break;
    }
  }
  if ( !flag && result != 0 )
  {
    result = 1;
  }
  return result;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int
MattOpt1D::optLinear()
{
  double min = m_lower;
  double max = m_upper;

  if ( max < min )
  {
    double tmp = max;

    max = min;
    min = tmp;
  }

  double step = ( max - min ) / double( m_maxfeval );

  if ( step < m_minstep )
    step = m_minstep;

  double x_min = min;
  double f_min;

  ( *m_gun )( 1, &x_min, f_min );

  double func;

  for ( double x = min + step; x <= max; x += step )
  {
    ( *m_gun )( 1, &x, func );
    if ( func < f_min )
    {
      x_min = x;
      f_min = func;
    }
  }

  m_x = x_min;
  m_func = f_min;
  ( *m_gun )( 1, &m_x, m_func );

  return 0;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int
MattOpt1D::optDot618M()         // 0.618 method
{
  int rslt = 0;

  double A = m_lower;
  double B = m_upper;

  if ( A > B )
  {
    double tmp = A;

    A = B;
    B = tmp;
  }
  int M = 0;

  double T1, F1, T2, F2;

  T1 = A + 0.381966011 * ( B - A );
  ( *m_gun )( 1, &T1, F1 );
  ++m_feval;

  while ( B - A > m_minstep && m_feval < m_maxfeval )
  {
    if ( M == 0 )
    {
      T2 = B + A - T1;
      ( *m_gun )( 1, &T2, F2 );
    }
    else
    {
      T1 = B + A - T2;
      ( *m_gun )( 1, &T1, F1 );
    }
    ++m_feval;
    if ( T1 < T2 )
    {
      if ( F1 < F2 )
      {
        B = T2;
        T2 = T1;
        F2 = F1;
        M = 1;
      }
      else
      {
        A = T1;
        T1 = T2;
        F1 = F2;
        M = 0;
      }
    }
    else
    {
      T1 = A + 0.381966011 * ( B - A );
      ( *m_gun )( 1, &T1, F1 );
      ++m_feval;
      M = 0;
    }
  }

  m_x = ( A + B ) * 0.5;
  ( *m_gun )( 1, &m_x, m_func );

  return rslt;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int
MattOpt1D::optFibonacci()       // Fibonacci Method
{
  int rslt = 0;

  double A = m_lower;
  double B = m_upper;

  if ( A > B )
  {
    double tmp = A;

    A = B;
    B = tmp;
  }

  int I = -1;
  std::vector < double >W;

  W.reserve( 10 );
  W.push_back( 1 );
  W.push_back( 1 );
  double W1, W2;

  W1 = W2 = 1;
  do
  {
    ++I;
    W.push_back( W1 + W2 );
    W1 = W2;
    W2 = W.back();
  }
  while ( W.back() <= ( B - A ) / m_minstep );

  double T1, T2, F1, F2;

  T1 = A + ( B - A ) * W[I] / W[I + 2];
  ( *m_gun )( 1, &T1, F1 );
  ++m_feval;

  int N = I + 2;
  int M = 0;

  for ( int K = 1; K < N - 1 && m_feval < m_maxfeval; ++K )
  {
    if ( M == 0 )
    {
      T2 = A + ( B - A ) * W[N - K] / W[N - K + 1];
      ( *m_gun )( 1, &T2, F2 );
    }
    else
    {
      T1 = A + ( B - A ) * W[N - K - 1] / W[N - K + 1];
      ( *m_gun )( 1, &T1, F1 );
    }
    ++m_feval;
    if ( F1 < F2 )
    {
      B = T2;
      T2 = T1;
      F2 = F1;
      M = 1;
    }
    else
    {
      A = T1;
      T1 = T2;
      F1 = F2;
      M = 0;
    }
  }

  m_x = ( A + B ) * 0.5;
  ( *m_gun )( 1, &m_x, m_func );

  return rslt;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int
MattOpt1D::optParabola()        // Parabola Method
{
  int rslt = 0;

  double a = m_lower;
  double b = m_upper;
  double c = m_origin;

  if ( a > b )
  {
    double tmp = a;

    a = b;
    b = tmp;
  }
  if ( c >= b || a >= c )
  {
    c = ( b + a ) * 0.5;
  }

  double f1, f2, f3, f4;

  ( *m_gun )( 1, &a, f1 );
  ( *m_gun )( 1, &c, f2 );
  ( *m_gun )( 1, &b, f3 );
  m_feval += 3;

  while ( b - a >= m_minstep && m_feval < m_maxfeval )
  {
    double A = a * a;
    double B = b * b;
    double C = c * c;

    double x1 = 0.5 * ( f1 * ( C - B ) + f2 * ( B - A ) + f3 * ( A - C ) );
    double x2 = f1 * ( c - b ) + f2 * ( b - a ) + f3 * ( a - c );

    if ( std::abs( x2 ) < 1e-12 )
    {
      a = b = c;
      break;
    }

    double x = x1 / x2;

    if ( x <= a || x >= b )
    {
      a = b = c;
      break;
    }

    if ( std::abs( x - c ) < m_minstep )
    {
      double d = ( a - b ) / 50;

      a = c - d;
      b = c + d;
      ( *m_gun )( 1, &a, f1 );
      ( *m_gun )( 1, &c, f2 );
      ( *m_gun )( 1, &b, f3 );
      m_feval += 3;
    }
    else
    {
      ( *m_gun )( 1, &x, f4 );
      ++m_feval;
      if ( x > c )
      {
        if ( f4 < f2 )
        {
          a = c;
          c = x;
          f1 = f2;
          f2 = f4;
        }
        else
        {
          b = x;
          f3 = f4;
        }
      }
      else
      {
        if ( f4 < f2 )
        {
          b = c;
          c = x;
          f3 = f2;
          f2 = f4;
        }
        else
        {
          a = x;
          f1 = f4;
        }
      }
    }
    if ( b - a < m_minstep )
    {
      a = b = x;
      break;
    }
    if ( f3 - f2 > 50 *( f1 - f2 ) || f1 - f2 > 50 *( f3 - f2 ) )
    {
      double d = ( a - b ) / 50;

      a = c - d;
      b = c + d;
      ( *m_gun )( 1, &a, f1 );
      ( *m_gun )( 1, &c, f2 );
      ( *m_gun )( 1, &b, f3 );
      m_feval += 3;
    }
  }

  m_x = ( a + b ) * 0.5;
  ( *m_gun )( 1, &m_x, m_func );

  return rslt;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

} // namespace Matt
