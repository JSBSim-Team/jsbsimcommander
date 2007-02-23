/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 Header:       MattMD.h
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

#ifndef MATTMD_H
#define MATTMD_H

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include <vector>
#include <string>
#include <fstream>

#include "Matt1D.h"

#ifdef MATT_JSBSIM_GUI
#include <wx/thread.h>
#endif

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
DEFINITIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#define ID_MATTMD "$Id$"

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

typedef void( *MattMDGun )( unsigned ndim, const double x[], double &fx );
void MattMDGunfor1D( unsigned ndim, const double x[], double &fx );
double sq( const double & x );

class MattOptMD
{
public:
  enum METHOD{
    tGrid = 0,
    tTakeTurns,
    tRosenbrock,
    tHooksJeeves,
    tDSC,
    tSimple,
    tPowell
  };
#ifdef MATT_JSBSIM_GUI
  wxThread * thread;
#endif
  MattOptMD( MattMDGun gun, const unsigned & ndim,
             const double origin[], const double max[], const double min[],
             const METHOD &method = tHooksJeeves );

  void   SetGun( MattMDGun gun )
  {
    m_gun = gun;
  }
  double GetOrigin( const unsigned &i )
  {
    return m_vars[i].m_origin;
  }
  void   SetOrigin( const unsigned &i, const double &origin )
  {
    m_vars[i].m_origin = origin;
  }
  double GetMax( const unsigned &i )
  {
    return m_vars[i].m_max;
  }
  void   SetMax( const unsigned &i, const double &max )
  {
    m_vars[i].m_max = max;
  }
  double GetMin( const unsigned &i )
  {
    return m_vars[i].m_min;
  }
  void   SetMin( const unsigned &i, const double &min )
  {
    m_vars[i].m_min = min;
  }
  METHOD GetMethod() const
  {
    return m_method;
  }
  std::string GetMethodName() const
  {
    switch ( m_method )
    {
    case tGrid :
      return "Grid Method";
    case  tTakeTurns :
      return "TakeTurns Method";
    case  tRosenbrock :
      return "Rosenbrock Method";
    case  tHooksJeeves :
      return "HooksJeeves Method";
    case  tDSC :
      return "DSC Method";
    case  tSimple :
      return "Simple Method";
    case  tPowell :
      return "Powell Method";
    }
    return "Unknow Method";
  }
  void   SetMethod( const METHOD & method )
  {
    m_method = method;
  }
  double GetMaxStep() const
  {
    return m_maxstep;
  }
  void   SetMaxStep( const double & maxstep )
  {
    m_maxstep = maxstep;
  }
  double GetMinStep() const
  {
    return m_minstep;
  }
  void   SetMinStep( const double & minstep )
  {
    m_minstep = minstep;
  }
  unsigned GetMaxIter() const
  {
    return m_maxiter;
  }
  void   SetMaxIter( const unsigned & maxiter )
  {
    m_maxiter = maxiter;
  }
  unsigned GetFeval() const
  {
    return m_feval;
  }
  void   SetFeval( const unsigned & num )
  {
    m_feval = num;
  }
  unsigned GetIter() const
  {
    return m_iter;
  }
  void   SetIter( const unsigned & num )
  {
    m_iter = num;
  }
  double GetFcnTol() const
  {
    return m_fcntol;
  }
  void   SetFcnTol( const double & fcntol )
  {
    m_fcntol = fcntol;
  }
  double GetStepTol() const
  {
    return m_steptol;
  }
  void   SetStepTol( const double & steptol )
  {
    m_steptol = steptol;
  }
  unsigned GetMaxFeval() const
  {
    return m_maxfeval;
  }
  void   SetMaxFeval( const unsigned & maxfeval )
  {
    m_maxfeval = maxfeval;
  }
  std::string GetOutputFileName() const
  {
    return m_filename;
  }
  void   SetOutputFileName( const std::string &filename )
  {
    m_filename = filename;
  }

  int      optimize();
  double   getF()
  {
    return m_func;
  }
  unsigned getXdim()
  {
    return m_vars.size();
  }
  double   getX( const unsigned &i )
  {
    return m_vars[i].m_origin;
  }

  struct VAR
  {
    double m_origin;
    double m_max;
    double m_min;
    double m_dir;
  };
  typedef std::vector<VAR> V_VAR;
  typedef std::vector<double> V_NUM;
  typedef std::vector< V_NUM > M_NUM;
  typedef std::vector< V_VAR > M_VAR;

protected:
  MattMDGun m_gun;
  V_VAR m_vars;
  METHOD m_method;
  MattOpt1D m_opt1d;

  double   m_func;
  double   m_maxstep;
  double   m_minstep;
  double   m_steptol;
  unsigned m_maxiter;
  unsigned m_maxfeval;
  double   m_fcntol;
  unsigned m_feval;
  unsigned m_iter;
  int m_result;
  std::string m_filename;
  std::ofstream m_output;

  int search1D( double &x );
  double getFcn( const V_VAR &vars );
  void modify_search_dir( int n, M_NUM &dirs, V_NUM &d );
  void set_dir( V_VAR & vars, const V_NUM &dirs );
  void outputSolution();
  void outputHeader();
  void outputLine();

  //Mathods
  int optGrid( void );
  int optTakeTurns( void );
  int optRosenbrock( void );
  int optHooksJeeves( void );
  int optDSC( void );
  int optSimple( void );
  int optPowell( void );

  friend void MattMDGunfor1D( unsigned ndim, const double x[], double &fx );
};

} // namespace Matt

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#endif
