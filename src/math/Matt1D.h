/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 Header:       Matt1D.h
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

HISTORY
--------------------------------------------------------------------------------
08/09/05   Matt   Created

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
SENTRY
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#ifndef MATT1D_H
#   define MATT1D_H

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
DEFINITIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#   define ID_MATT1D "$Id$"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
FORWARD DECLARATIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

namespace Matt
{

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DOCUMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DECLARATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

typedef void( *Matt1DGun )( unsigned ndim, const double x[], double &fx );

class MattOpt1D
{
public:
  enum METHOD
  {
    tLinear = 0,
    tDot618M,
    tFibonacci,
    tParabola
  };
  MattOpt1D();
  MattOpt1D( Matt1DGun gun,
             const double &origin, const double &max, const double &min,
             const METHOD & method = tDot618M );

  void SetGun( Matt1DGun gun )
  {
    m_gun = gun;
  }
  double GetOrigin()
  {
    return m_origin;
  }
  void SetOrigin( const double &origin )
  {
    m_origin = origin;
  }
  double GetMax()
  {
    return m_max;
  }
  void SetMax( const double &max )
  {
    m_max = max;
  }
  double GetMin()
  {
    return m_min;
  }
  void SetMin( const double &min )
  {
    m_min = min;
  }
  METHOD GetMethod() const
  {
    return m_method;
  }
  void SetMethod( const METHOD & method )
  {
    m_method = method;
  }
  double GetMaxStep() const
  {
    return m_maxstep;
  }
  void SetMaxStep( const double &maxstep )
  {
    m_maxstep = maxstep;
  }
  double GetMinStep() const
  {
    return m_minstep;
  }
  void SetMinStep( const double &minstep )
  {
    m_minstep = minstep;
  }
  unsigned GetMaxFeval() const
  {
    return m_maxfeval;
  }
  void SetMaxFeval( const unsigned &maxfeval )
  {
    m_maxfeval = maxfeval;
  }
  unsigned GetFeval() const
  {
    return m_feval;
  }
  void SetFeval( const unsigned &num )
  {
    m_feval = num;
  }

  int optimize();
  double getF()
  {
    return m_func;
  }
  double getX()
  {
    return m_x;
  }

protected:
  double m_origin;
  double m_max;
  double m_min;
  Matt1DGun m_gun;
  METHOD m_method;
  double m_func;
  double m_x;
  double m_maxstep;
  double m_minstep;
  unsigned m_maxfeval;

  double m_upper;
  double m_lower;
  int searchBrackets();
  unsigned m_feval;

  //Mathods
  // tLinear :
  int optLinear( void );

  // tDot618M :
  int optDot618M( void );

  // tFibonacci :
  int optFibonacci( void );

  // tParabola :
  int optParabola( void );
};


} // namespace Matt

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#endif
