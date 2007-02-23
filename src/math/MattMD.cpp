/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 Header:       MattMD.cpp
 Author:       Matthew Gong
 Date started: 08/10/2005

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

This class wraps up the MultiDim optimizing mothods.

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
#include <cassert>
#include <iostream>
#include <iomanip>

#include "MattMD.h"


namespace Matt
{

static const char *IdSrc =
  "$Id$";
static const char *IdHdr = ID_MATTMD;

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
GLOBAL DECLARATIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


MattOptMD * g_MattOptMD;

void MattMDGunfor1D( unsigned ndim, const double x[], double &fx )
{
  if ( g_MattOptMD && ndim == 1u )
  {
    unsigned ndim = g_MattOptMD->m_vars.size();
    double *X = new double[ndim];
    int j = 0;
    for ( MattOptMD::V_VAR::iterator i = g_MattOptMD->m_vars.begin();
          i != g_MattOptMD->m_vars.end(); ++i, ++j )
    {
      X[j] = ( *i ).m_origin + ( *i ).m_dir * ( *x );
    }
    g_MattOptMD->m_gun( ndim, X, fx );
    delete []X;
  }
}

MattOptMD::V_NUM operator +( const MattOptMD::V_NUM &a, const MattOptMD::V_NUM &b )
{
  assert( a.size() == b.size() );

  MattOptMD::V_NUM result;
  size_t s = a.capacity();
  result.reserve( s );
  MattOptMD::V_NUM::const_iterator j = b.begin();
  for ( MattOptMD::V_NUM::const_iterator i = a.begin(); i != a.end(); ++i, ++j )
  {
    result.push_back(( *i ) + ( *j ) );
  }
  return result;
}

MattOptMD::V_NUM operator -( const MattOptMD::V_NUM &a, const MattOptMD::V_NUM &b )
{
  assert( a.size() == b.size() );

  MattOptMD::V_NUM result;
  size_t s = a.capacity();
  result.reserve( s );
  MattOptMD::V_NUM::const_iterator j = b.begin();
  for ( MattOptMD::V_NUM::const_iterator i = a.begin(); i != a.end(); ++i, ++j )
  {
    result.push_back(( *i ) - ( *j ) );
  }
  return result;
}

MattOptMD::V_VAR operator +( const MattOptMD::V_VAR &a, const MattOptMD::V_NUM &b )
{
  assert( a.size() == b.size() );

  MattOptMD::V_VAR result = a;
  MattOptMD::V_NUM::const_iterator j = b.begin();
  for ( MattOptMD::V_VAR::iterator i = result.begin(); i != result.end(); ++i, ++j )
  {
    ( *i ).m_origin += *j;
  }
  return result;
}

MattOptMD::V_VAR operator -( const MattOptMD::V_VAR &a, const MattOptMD::V_NUM &b )
{
  assert( a.size() == b.size() );

  MattOptMD::V_VAR result = a;
  MattOptMD::V_NUM::const_iterator j = b.begin();
  for ( MattOptMD::V_VAR::iterator i = result.begin(); i != result.end(); ++i, ++j )
  {
    ( *i ).m_origin -= *j;
  }
  return result;
}

MattOptMD::V_VAR operator +( const MattOptMD::V_NUM &b, const MattOptMD::V_VAR &a )
{
  assert( a.size() == b.size() );

  MattOptMD::V_VAR result = a;
  MattOptMD::V_NUM::const_iterator j = b.begin();
  for ( MattOptMD::V_VAR::iterator i = result.begin(); i != result.end(); ++i, ++j )
  {
    ( *i ).m_origin += *j;
  }
  return result;
}

MattOptMD::V_VAR operator -( const MattOptMD::V_NUM &b, const MattOptMD::V_VAR &a )
{
  assert( a.size() == b.size() );

  MattOptMD::V_VAR result = a;
  MattOptMD::V_NUM::const_iterator j = b.begin();
  for ( MattOptMD::V_VAR::iterator i = result.begin(); i != result.end(); ++i, ++j )
  {
    ( *i ).m_origin = *j - ( *i ).m_origin;
  }
  return result;
}

MattOptMD::V_VAR operator +( const MattOptMD::V_VAR &a, const MattOptMD::V_VAR &b )
{
  assert( a.size() == b.size() );

  MattOptMD::V_VAR result = a;
  MattOptMD::V_VAR::const_iterator j = b.begin();
  for ( MattOptMD::V_VAR::iterator i = result.begin(); i != result.end(); ++i, ++j )
  {
    ( *i ).m_origin += ( *j ).m_origin;
    if (( *i ).m_max > ( *j ).m_max )
      ( *i ).m_max = ( *j ).m_max;
    if (( *i ).m_min < ( *j ).m_min )
      ( *i ).m_min = ( *j ).m_min;
  }
  return result;
}

MattOptMD::V_VAR operator -( const MattOptMD::V_VAR &a, const MattOptMD::V_VAR &b )
{
  assert( a.size() == b.size() );

  MattOptMD::V_VAR result = a;
  MattOptMD::V_VAR::const_iterator j = b.begin();
  for ( MattOptMD::V_VAR::iterator i = result.begin(); i != result.end(); ++i, ++j )
  {
    ( *i ).m_origin -= ( *j ).m_origin;
    if (( *i ).m_max > ( *j ).m_max )
      ( *i ).m_max = ( *j ).m_max;
    if (( *i ).m_min < ( *j ).m_min )
      ( *i ).m_min = ( *j ).m_min;
  }
  return result;
}

static const double ZERO = 1e-15;

bool operator ==( const MattOptMD::V_VAR &a, const MattOptMD::V_VAR &b )
{
  if ( a.size() != b.size() )
  {
    return false;
  }
  MattOptMD::V_VAR::const_iterator j = b.begin();
  for ( MattOptMD::V_VAR::const_iterator i = a.begin(); i != a.end(); ++i, ++j )
  {
    if ( std::abs(( *i ).m_origin - ( *j ).m_origin ) > ZERO )
      return false;
    if ( std::abs(( *i ).m_max - ( *j ).m_max ) > ZERO )
      return false;
    if ( std::abs(( *i ).m_min < ( *j ).m_min ) > ZERO )
      return false;
  }
  return true;
}

double operator *( const MattOptMD::V_NUM &a, const MattOptMD::V_NUM &b )
{
  assert( a.size() == b.size() );

  double result = 0;
  MattOptMD::V_NUM::const_iterator j = b.begin();
  for ( MattOptMD::V_NUM::const_iterator i = a.begin(); i != a.end(); ++i, ++j )
  {
    result += ( *i ) * ( *j );
  }
  return result;
}


MattOptMD::V_NUM vvar_to_vnum( const MattOptMD::V_VAR &a )
{
  MattOptMD::V_NUM result;
  result.reserve( a.capacity() );
  for ( MattOptMD::V_VAR::const_iterator i = a.begin(); i != a.end(); ++i )
  {
    result.push_back(( *i ).m_origin );
  }
  return result;
}

MattOptMD::V_NUM operator *( const double &a, const MattOptMD::V_NUM &b )
{
  MattOptMD::V_NUM result = b;
  for ( MattOptMD::V_NUM::iterator i = result.begin(); i != result.end(); ++i )
  {
    *i = a * ( *i );
  }
  return result;
}

MattOptMD::V_VAR operator *( const double &a, const MattOptMD::V_VAR &b )
{
  MattOptMD::V_VAR result = b;
  for ( MattOptMD::V_VAR::iterator i = result.begin(); i != result.end(); ++i )
  {
    ( *i ).m_origin = a * ( *i ).m_origin;
  }
  return result;
}

double vabs( const MattOptMD::V_NUM &p )
{
  double sum = 0;
  for ( MattOptMD::V_NUM::const_iterator i = p.begin(); i != p.end(); ++i )
  {
    sum += ( *i ) * ( *i );
  }
  return std::sqrt( sum );
}

double vsq( const MattOptMD::V_NUM &p )
{
  double sum = 0;
  for ( MattOptMD::V_NUM::const_iterator i = p.begin(); i != p.end(); ++i )
  {
    sum += ( *i ) * ( *i );
  }
  return sum;
}

double vsq( const MattOptMD::V_VAR &p )
{
  double sum = 0;
  for ( MattOptMD::V_VAR::const_iterator i = p.begin(); i != p.end(); ++i )
  {
    sum += ( *i ).m_origin * ( *i ).m_origin;
  }
  return sum;
}

double sq( const double & x )
{
  return x*x;
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS IMPLEMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

MattOptMD::MattOptMD( MattMDGun gun, const unsigned & ndim,
                      const double origin[], const double max[], const double min[],
                      const METHOD &method ) :
    m_gun( gun ),
    m_method( method ),
    m_opt1d( MattMDGunfor1D, 0, 1, -1, MattOpt1D::tDot618M ),
    m_func( 0 ),
    m_maxstep( 1 ),
    m_minstep( 1e-3 ),
    m_steptol( 1e-3 ),
    m_maxiter( 30 ),
    m_maxfeval( 500 ),
    m_fcntol( 1e-6 ),
    m_feval( 0 ),
    m_iter( 0 ),
    m_result( 0 ),
    m_filename( "MattMD.out" )
#ifdef MATT_JSBSIM_GUI
    , thread( NULL )
#endif
{
#ifdef MATT_JSBSIM_GUI
  m_opt1d.thread = thread;
#endif
  m_vars.reserve( ndim + 1 );
  VAR tmp;
  for ( unsigned i = 0; i < ndim ; ++i )
  {
    tmp.m_origin = origin[i];
    tmp.m_max    = max[i];
    tmp.m_min    = min[i];
    tmp.m_dir    = 1;
    m_vars.push_back( tmp );
  }

}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int MattOptMD::optimize()
{
  int result;
  m_output.open( m_filename.c_str() );
  if ( !m_output )
  {
    std::cerr << "Can not open output file" << std::endl;
  }
  else
  {
    if ( m_method != tGrid )
      outputHeader();
  }
  switch ( m_method )
  {
  case tGrid :
    result = optGrid();
    break;
  case  tTakeTurns :
    result = optTakeTurns();
    break;
  case  tRosenbrock :
    result = optRosenbrock();
    break;
  case  tHooksJeeves :
    result = optHooksJeeves();
    break;
  case  tDSC :
    result = optDSC();
    break;
  case  tSimple :
    result = optSimple();
    break;
  case  tPowell :
    result = optPowell();
    break;
  }
  m_result = result;
  if ( m_output )
  {
    outputSolution();
    m_output.close();
  }
  return result;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void MattOptMD::outputLine()
{
  if ( m_output )
  {
    m_output << std::setw( 10 ) << m_iter << "    ";
    m_output << std::setw( 10 ) << m_feval << "    ";
    m_output << std::setw( 10 ) << m_func << '\n';
  }
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void MattOptMD::outputHeader()
{
  if ( m_output )
  {
    m_output << "======   Iteration Summary   ======\n";
    m_output << "      Iter        Fevals          F(x)\n";
  }
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void MattOptMD::outputSolution()
{
  if ( m_output )
  {
    m_output << '\n';
    m_output << "=========  Solution     ===========\n";
    m_output << "Optimization method       = " << GetMethodName() << '\n';
    m_output << "Dimension of the problem  = " << m_vars.size() << '\n';
    m_output << "Return code               = " << m_result << '\n';
    m_output << "No. iterations taken      = " << m_iter << '\n';
    m_output << "No. function evaluations  = " << m_feval << '\n';
    m_output << '\n';
    m_output << "=========  Solution     ===========\n";
    m_output << "    i         x      \n";
    for ( V_VAR::size_type i = 0u; i < m_vars.size(); ++i )
    {
      m_output << "    " << i << "         " <<  m_vars[i].m_origin << '\n';
    }
    m_output << "Function Value       = " << m_func << '\n';
    m_output << '\n';
    m_output << "==========  Tolerances  ===========\n";
    m_output << "Maximum Step         = " << m_maxstep << '\n';
    m_output << "Minimum Step         = " << m_minstep << '\n';
    m_output << "Maximum Iter         = " << m_maxiter << '\n';
    m_output << "Maximum Fcn Eval     = " << m_maxfeval << '\n';
    m_output << "Step Tolerance       = " << m_steptol << '\n';
    m_output << "Function Tolerance   = " << m_fcntol << '\n';
    m_output << '\n';
  }
}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int MattOptMD::search1D( double &x )
{
  double upper =  99e99;
  double lower = -99e99;
  for ( V_VAR::iterator i = m_vars.begin(); i != m_vars.end(); ++i )
  {
    if ( std::abs(( *i ).m_dir ) > 1e-6 )
    {
      double tmp1 = (( *i ).m_max - ( *i ).m_origin ) / ( *i ).m_dir;
      double tmp2 = (( *i ).m_min - ( *i ).m_origin ) / ( *i ).m_dir;
      if ( tmp1 < tmp2 )  // Make sure that tmp1 > tmp2
      {
        double tmp3;
        tmp3 = tmp2;
        tmp2 = tmp1;
        tmp1 = tmp3;
      }
      if ( tmp1 < upper )
        upper = tmp1;
      if ( tmp2 > lower )
        lower = tmp2;
    }
  }
  if ( std::abs( upper - lower ) < m_minstep )
    return 0;
  m_opt1d.SetOrigin( 0 );
  m_opt1d.SetMax( upper );
  m_opt1d.SetMin( lower );
  m_opt1d.SetMaxStep( m_maxstep );
  m_opt1d.SetMinStep( m_minstep );
  m_opt1d.SetMaxFeval( 16 );
  m_opt1d.SetFeval( 0 );

  MattOptMD * old = g_MattOptMD;
  g_MattOptMD = this;
  int rslt = m_opt1d.optimize();
  g_MattOptMD = old;
  m_feval   += m_opt1d.GetFeval();
  m_func   = m_opt1d.getF();
  x = m_opt1d.getX();
  for ( V_VAR::iterator i = m_vars.begin(); i != m_vars.end(); ++i )
  {
    ( *i ).m_origin = ( *i ).m_origin + ( *i ).m_dir * x;
  }

  return rslt;
}

int MattOptMD::optGrid( void )
{
  unsigned div = ( unsigned )std::sqrt( m_maxfeval ) + 1u;
  double min = 99e99;
  V_VAR vars;
  double dt0 = ( m_vars[0].m_max - m_vars[0].m_min ) / ( double )div;
  double dt1 = ( m_vars[1].m_max - m_vars[1].m_min ) / ( double )div;
  if ( m_output )
  {
    m_output << "          ";
    for ( double tmp = m_vars[1].m_min; tmp <= m_vars[1].m_max; tmp += dt1 )
    {
      m_output << std::setw( 10 ) << tmp;
    }
    m_output << '\n';
  }
  for ( double tmp0 = m_vars[1].m_min; tmp0 <= m_vars[1].m_max; tmp0 += dt0 )
  {
    if ( m_output )
    {
      m_output << std::setw( 10 ) << tmp0;
    }
    for ( double tmp1 = m_vars[1].m_min; tmp1 <= m_vars[1].m_max; tmp1 += dt1 )
    {
#ifdef MATT_JSBSIM_GUI
      if ( thread && thread->TestDestroy() ) return 0;
#endif
      m_vars[0].m_origin = tmp0;
      m_vars[1].m_origin = tmp1;
      m_func = getFcn( m_vars );
      if ( m_output )
      {
        m_output << std::setw( 10 ) << m_func;
      }
      if ( m_func < min )
      {
        min = m_func;
        vars = m_vars;
      }
    }
    if ( m_output )
    {
      m_output << '\n';
    }
  }

  m_func = min;
  m_vars = vars;
  return 0;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int MattOptMD::optTakeTurns( void )
{
  int    result = 0;
  m_iter = 0;
  m_feval = 0;
  int         n = m_vars.size();

  // initial direction
  M_NUM dirs;
  {
    V_NUM dir;
    dir.reserve( n + 1 );
    for ( int i = 0; i < n; ++i )
      dir.push_back( 0 );
    for ( int i = 0; i < n; ++i )
    {
      dir[i] = 1;
      if ( i != 0 )
        dir[i-1] = 0;
      dirs.push_back( dir );
    }
  }

  double ramda;
  double f = 99e99;
  while ( m_iter < m_maxiter && m_feval < m_maxfeval )
  {
    for ( int K = 0; K < n; ++K )
    {
      set_dir( m_vars, dirs[K] );
      search1D( ramda );
    }
    if ( std::abs( f - m_func ) < m_fcntol )
    {
      result = 1;
      break;
    }
    f = m_func;
    ++m_iter;
    outputLine();
#ifdef MATT_JSBSIM_GUI
    if ( thread && thread->TestDestroy() ) return 0;
#endif
  }

  return result;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int MattOptMD::optRosenbrock( void )
{
  int result = 0;
  m_iter = 0;
  m_feval = 0;
  int      n = m_vars.size();
  double  h0 = m_maxstep;
  double eps = m_minstep;

  // initial direction
  V_NUM dir;
  dir.reserve( n + 1 );
  for ( int i = 0; i < n; ++i )
    dir.push_back( 0 );
  M_NUM dirs;
  for ( int i = 0; i < n; ++i )
  {
    dir[i] = 1;
    if ( i != 0 )
      dir[i-1] = 0;
    dirs.push_back( dir );
  }

  // initial point
  V_VAR z = m_vars;
  double f0 = getFcn( z );
  ++m_feval;

  bool flag_I_find_it = false;
  while ( !flag_I_find_it && m_iter < m_maxiter && m_feval < m_maxfeval )
  {
    V_NUM h;
    V_NUM d;
    V_NUM Q;
    h.reserve( n + 1 );
    d.reserve( n + 1 );
    Q.reserve( n + 1 );
    for ( int i = 0; i < n; ++i )
    {
      h.push_back( h0 );
      d.push_back( 0 );
      Q.push_back( 0 );
    }
    double S = 0;
    double SS = n * h0;

    bool flag_modify_search_dir = false;

#ifdef MATT_JSBSIM_GUI
    if ( thread && thread->TestDestroy() ) return 0;
#endif
    while ( !flag_modify_search_dir && !flag_I_find_it
            && m_iter < m_maxiter && m_feval < m_maxfeval )
    {
      for ( int K = 0; K < n; ++K )
      {
#ifdef MATT_JSBSIM_GUI
        if ( thread && thread->TestDestroy() ) return 0;
#endif
        SS -= std::abs( h[K] );
        S  -= Q[K];
        V_VAR x = z + h[K] * dirs[K];
        double f1 = getFcn( x );
        ++m_feval;
        if ( f1 < f0 )
        {
          z = x;
          f0 = f1;
          d[K] += h[K];
          h[K] = 3 * h[K];
          if ( Q[K] == 0 )
          {
            Q[K] = 1;
          }
        }
        else
        {
          h[K] = -0.5 * h[K];
          if ( Q[K] == 1 )
          {
            Q[K] = 2;
          }
        }
        SS += std::abs( h[K] );
        S += Q[K];
        if ( SS < eps )
        {
          flag_I_find_it = true;
          result = 1;
          break;
        }
        if ( S == 2*n )
        {
          ++m_iter;
          m_func = f0;
          outputLine();
#ifdef MATT_JSBSIM_GUI
          if ( thread && thread->TestDestroy() ) return 0;
#endif
          m_vars = z;
          modify_search_dir( n, dirs, d );
          flag_modify_search_dir = true;
          break;
        }
      }
    }
  }

  m_vars = z;
  m_func = f0;
  return result;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void MattOptMD::modify_search_dir( int n, M_NUM &dirs, V_NUM &d )
{
  M_NUM p = dirs;
  p[n-1] = d[n-1] * dirs[n-1];
  for ( int i = 1; i < n; ++i )
  {
    p[n-1-i] = p[n-i] + d[n-1-i] * dirs[n-1-i];
  }
  M_NUM q = dirs;
  q[0] = 1.0 / vabs( p[0] ) * p[0];
  for ( int i = 1; i < n; ++i )
  {
    int k = 0;
    V_NUM r = p[i];
    do
    {
      r = r - ( q[k] * p[i] ) * q[k];
      ++k;
    }
    while ( k != i );
    q[i] = 1.0 / vabs( r ) * r;
  }
  dirs = q;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

double MattOptMD::getFcn( const V_VAR &vars )
{
  double fx;
  unsigned ndim = vars.size();
  double *X = new double[ndim];
  bool flag = true;
  for ( size_t i = 0; i < vars.size(); ++i )
  {
    X[i] = vars[i].m_origin;
    if ( vars[i].m_origin > vars[i].m_max || vars[i].m_origin < vars[i].m_min )
    {
      flag = false;
      break;
    }
  }
  if ( flag )
  {
    m_gun( ndim, X, fx );
  }
  else
  {
    fx = 99e99;
  }
  delete []X;
  return fx;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int MattOptMD::optHooksJeeves( void )
{
  int    result = 0;
  m_iter = 0;
  m_feval = 0;
  int         n = m_vars.size();
  double      h = m_maxstep;
  double    eps = m_minstep;

  // initial direction
  V_NUM dir;
  dir.reserve( n + 1 );
  for ( int i = 0; i < n; ++i )
    dir.push_back( 0 );
  M_NUM dirs;
  for ( int i = 0; i < n; ++i )
  {
    dir[i] = 1;
    if ( i != 0 )
      dir[i-1] = 0;
    dirs.push_back( dir );
  }

  // initial point
  V_VAR x0 = m_vars;
  double f0 = getFcn( x0 );
  ++m_feval;

#ifdef MATT_JSBSIM_GUI
  if ( thread && thread->TestDestroy() ) return 0;
#endif
  bool flag_I_find_it = false;
  V_VAR x1 = x0;
  while ( !flag_I_find_it && m_iter < m_maxiter && m_feval < m_maxfeval )
  {
    V_VAR z = x1;
    double f1 = getFcn( z );

    for ( int K = 0; K < n; ++K )
    {
      V_VAR x = z + h * dirs[K];
      double f2 = getFcn( x );
      ++m_feval;
      if ( f2 < f1 )
      {
        z = x;
        f1 = f2;
      }
      else
      {
        x = z - h * dirs[K];
        f2 = getFcn( x );
        if ( f2 < f1 )
        {
          h = -h;
          z = x;
          f1 = f2;
        }
      }
    }
    if ( f1 < f0 )
    {
      x1 = z + ( z - x0 );
      x0 = z;
      f0 = f1;
    }
    else
    {
      if ( x1 == x0 )
      {
        if ( std::abs( h ) < eps )
        {
          flag_I_find_it = true;
          result = 1;
          break;
        }
        else
        {
          h *= 0.1;
        }
      }
      else
      {
        x1 = x0;
      }
    }
    ++m_iter;
    m_func = f0;
    outputLine();
#ifdef MATT_JSBSIM_GUI
    if ( thread && thread->TestDestroy() ) return 0;
#endif
  }

  m_vars = x0;
  m_func = f0;
  return result;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void MattOptMD::set_dir( V_VAR & vars, const V_NUM &dirs )
{
  assert( vars.size() == dirs.size() );

  V_NUM::const_iterator j = dirs.begin();
  for ( V_VAR::iterator i = vars.begin(); i != vars.end(); ++i, ++j )
  {
    ( *i ).m_dir = ( *j );
  }
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int MattOptMD::optDSC( void )
{
  MattOpt1D::METHOD old = m_opt1d.GetMethod();
  m_opt1d.SetMethod( MattOpt1D::tParabola );
  int    result = 0;
  m_iter = 0;
  m_feval = 0;
  int         n = m_vars.size();
  double      h = m_maxstep;
  double    eps = m_minstep;

  // initial direction
  M_NUM dirs;
  {
    V_NUM dir;
    dir.reserve( n + 1 );
    for ( int i = 0; i < n; ++i )
      dir.push_back( 0 );
    for ( int i = 0; i < n; ++i )
    {
      dir[i] = 1;
      if ( i != 0 )
        dir[i-1] = 0;
      dirs.push_back( dir );
    }
  }

  // initial point
  V_VAR x0 = m_vars;
  double f0;
  V_VAR x1;
  double f1;
  V_NUM d;
  d.reserve( n + 1 );
  for ( int i = 0; i < n; ++i )
  {
    d.push_back( 0 );
  }
  bool flag_I_find_it = false;
  int K = 0;
  V_VAR z = x0;
  double f;
  while ( !flag_I_find_it && m_iter < m_maxiter && m_feval < m_maxfeval )
  {
    m_vars = z;
    for ( ; K < n; ++K )
    {
      set_dir( m_vars, dirs[K] );
      search1D( d[K] );
    }
    z = m_vars;
    f = m_func;
    //m_vars = z;
    set_dir( m_vars, vvar_to_vnum( z - x0 ) );
    double ramda;
    search1D( ramda );
    x1 = m_vars;
    f1 = m_func;
    M_NUM p = dirs;
    p[n-1] = d[n-1] * dirs[n-1];
    for ( int i = 1; i < n; ++i )
    {
      p[n-1-i] = p[n-i] + d[n-1-i] * dirs[n-1-i];
    }
    if ( vabs( p[0] ) < h )
    {
      if ( vabs( p[0] ) < eps )
      {
        flag_I_find_it = true;
        result = 1;
        x0 = x1;
        f0 = f1;
        break;
      }
      else
      {
        x0 = x1;
        f0 = f1;
        h *= 0.1;
        K = 0;
        z = x0;
        f = f0;
      }
    }
    else
    {
      for ( int i = n - 1; i >= 1; --i )
      {
        if ( vabs( p[i] ) > ZERO )
        {
          dirs[i] = ( 1.0 / ( vabs( p[i-1] ) ) * vabs( p[2] ) )
                    * ( d[i-1] * p[i] - sq( vabs( p[i] ) ) * dirs[i-1] );
        }
      }
      dirs[0] = 1.0 / vabs( p[0] ) * p[0];
      x0 = z;
      z  = x1;
      d[0] = ramda;
      K = 1;
    }
    ++m_iter;
    m_func = f1;
    outputLine();
#ifdef MATT_JSBSIM_GUI
    if ( thread && thread->TestDestroy() ) return 0;
#endif
  }

  m_vars = x0;
  m_func = f0;
  m_opt1d.SetMethod( old );
  return result;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int MattOptMD::optSimple( void )
{
  int    result = 0;
  m_iter = 0;
  m_feval = 0;
  int         n = m_vars.size();
  double      h = m_maxstep;
  double    eps = m_minstep;

  // initial direction
  M_NUM dirs;
  {
    V_NUM dir;
    dir.reserve( n + 1 );
    for ( int i = 0; i < n; ++i )
      dir.push_back( 0 );
    for ( int i = 0; i < n; ++i )
    {
      dir[i] = 1;
      if ( i != 0 )
        dir[i-1] = 0;
      dirs.push_back( dir );
    }
  }

  // initial point
  V_VAR x0 = m_vars;
  M_VAR X;
  X.push_back( x0 );
  for ( int i = 0; i < n; ++i )
  {
    X.push_back( x0 + h*dirs[i] );
  }

  bool flag_I_find_it = false;
  while ( !flag_I_find_it && m_iter < m_maxiter && m_feval < m_maxfeval )
  {
    V_NUM f;
    for ( M_VAR::iterator i = X.begin(); i != X.end(); ++i )
    {
      f.push_back( getFcn( *i ) );
      ++m_feval;
    }

    bool flag_rebuild = false;
    while ( !flag_rebuild && !flag_I_find_it && m_iter < m_maxiter && m_feval < m_maxfeval )
    {
      int    iL = 0, iH = 0, iG = 0;
      for ( size_t i = 1; i < f.size(); ++i )
      {
        if ( f[i] <= f[iL] )
        {
          iL = i;
        }
        if ( f[i] >= f[iH] )
        {
          iH = i;
        }
        if ( f[i] >= f[iG] && f[i] < f[iH] )
        {
          iG = i;
        }
      }
      for ( size_t i = 1; i < f.size(); ++i )
      {
        if ( f[i] >= f[iG] && f[i] < f[iH] )
        {
          iG = i;
        }
      }

      V_VAR xC;
      for ( M_VAR::iterator i = X.begin(); i != X.end(); ++i )
      {
        if ( i == X.begin() )
        {
          xC = ( *i );
        }
        else
        {
          xC = xC + ( *i );
        }
      }
      xC = xC - X[iH];
      xC = 1.0 / ( double )n * xC;

      double tmp = 0;
      for ( M_VAR::iterator i = X.begin(); i != X.end(); ++i )
      {
        V_VAR xTmp = ( *i ) - xC;
        tmp += vsq( xTmp );
      }
      tmp /= n + 1;
      tmp = std::sqrt( tmp );
      if ( tmp < eps )
      {
        flag_I_find_it = true;
        result = 1;
        m_vars = X[iL];
        m_func = f[iL];
        break;
      }
      else
      {
        V_VAR xR = xC + ( xC - X[iH] );
        double fR = getFcn( xR );
        ++m_feval;
        V_VAR xN;
        double fN;
        if ( fR < f[iL] )
        {
          V_VAR xE = xR + ( xR - xC );
          double fE = getFcn( xE );
          ++m_feval;
          if ( fE < f[iL] )
          {
            xN = xE;
            fN = fE;
          }
          else
          {
            xN = xR;
            fN = fR;
          }
        }
        else
        {
          if ( fR < f[iG] )
          {
            xN = xR;
            fN = fR;
          }
          else
          {
            double ramda;
            if ( fR < f[iH] )
            {
              ramda = 0.75;
            }
            else
            {
              ramda = 0.25;
            }
            xN = ( 1 - ramda ) * X[iH] + ramda * xR;
            fN = getFcn( xN );
            ++m_feval;
          }
        }
        if ( fN < f[iH] )
        {
          X[iH] = xN;
          f[iH] = fN;
        }
        else
        {
          for ( M_VAR::iterator i = X.begin(); i != X.end(); ++i )
          {
            ( *i ) = 0.5 * (( *i ) + X[iL] );
          }
          flag_rebuild = true;
          break;
        }
      }
      m_vars = X[iL];
      m_func = f[iL];
    }
    ++m_iter;
    outputLine();
#ifdef MATT_JSBSIM_GUI
    if ( thread && thread->TestDestroy() ) return 0;
#endif
  }

  return result;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int MattOptMD::optPowell( void )
{
  MattOpt1D::METHOD old = m_opt1d.GetMethod();
  m_opt1d.SetMethod( MattOpt1D::tFibonacci );
  int result = 0;
  m_iter = 0;
  m_feval = 0;
  int      n = m_vars.size();
  double eps = m_minstep;

  // initial direction
  V_NUM dir;
  dir.reserve( n + 1 );
  for ( int i = 0; i < n; ++i )
    dir.push_back( 0 );
  M_NUM dirs;
  for ( int i = 0; i < n; ++i )
  {
    dir[i] = 1;
    if ( i != 0 )
      dir[i-1] = 0;
    dirs.push_back( dir );
  }

  // initial point
  V_VAR x0 = m_vars;
  double f0 = getFcn( x0 );
  ++m_feval;

  bool flag_I_find_it = false;
  while ( !flag_I_find_it && m_iter < m_maxiter && m_feval < m_maxfeval )
  {
    int m = 0;
    V_NUM ramda;
    V_NUM mu;
    V_NUM f;
    double tmp;
    M_VAR X;

    m_vars = x0;
    set_dir( m_vars, dirs[0] );
    search1D( tmp );
    ramda.push_back( tmp );
    X.push_back( m_vars );
    f.push_back( m_func );
    mu.push_back( f.back() - f0 );

    for ( int i = 1; i < n;++i )
    {
      m_vars = X.back();
      set_dir( m_vars, dirs[i] );
      search1D( tmp );
      ramda.push_back( tmp );
      X.push_back( m_vars );
      mu.push_back( f.back() - m_func );
      f.push_back( m_func );
      if ( mu.back() > mu.front() )
      {
        mu.front() = mu.back();
        m = i;
      }
    }
    V_NUM e = vvar_to_vnum( X.back() - x0 );
    m_vars = x0;
    set_dir( m_vars, e );
    search1D( tmp );
    V_VAR xb = m_vars;
    double fb = m_func;
    if ( tmp <= std::sqrt(( f0 - fb ) / mu.front() ) )
    {
      for ( int j = m; j < n - 1 ; ++j )
      {
        dirs[j] = dirs[j+1];
      }
      dirs[n-1] = e;
    }
    m_vars = xb;
    m_func = fb;
    if ( vabs( vvar_to_vnum( xb - x0 ) ) < eps )
    {
      flag_I_find_it = true;
      result = 1;
      break;
    }
    else
    {
      x0 = xb;
    }
    ++m_iter;
    outputLine();
#ifdef MATT_JSBSIM_GUI
    if ( thread && thread->TestDestroy() ) return 0;
#endif
  }

  m_opt1d.SetMethod( old );
  return result;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

} // namespace Matt
