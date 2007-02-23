/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 Module:                    bsplines.cpp
 Author(fortran edition):   JONES, R. E., (SNLA)
                            BOISVERT, RONALD, NBS
 Transformed and Wraped By: Matthew Gong
 Date started:              09/27/2006
 Purpose:                   Provide the convenient B-Spline interpolation
                            functions and classes.
 Called by:

 -------- Copyright (C) 2006  Matthew Gong (matthewzhenggong@gmail.com) -------

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
Provide the convenient B-Spline interpolation functions and classes.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
COMMENTS, REFERENCES,  and NOTES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include "bsplines.h"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
DEFINITIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
GLOBAL DATA
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

static int * ErrNo = NULL;

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS IMPLEMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

  inline int max( int a, int b )
  {
    return a > b ? a : b;
  }

  inline int max( int a, int b, int c )
  {
    return a > b ? (a > c ? a : c) : (b > c ? b : c);
  }

  inline int min( int a, int b )
  {
    return a < b ? a : b;
  }

  inline int min( int a, int b, int c )
  {
    return a < b ? (a < c ? a : c) : (b < c ? b : c);
  }

int BSpline1D::ReturnCode(void) const 
{
  return ErrNo;
}

int BSpline2D::ReturnCode(void) const
{
  return ErrNo;
}

int BSpline3D::ReturnCode(void) const
{
  return ErrNo;
}

BSpline1D::BSpline1D( const double * x, size_t nx, const double * y, const int & order ):
    OK( false ), ErrNo(-1)
{
  setDomain( x, nx, y, order );
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

BSpline1D::BSpline1D( const TLIST &x, const TLIST &y, const int & order ):
    OK( false ), ErrNo(-1)
{
  setDomain( x, y, order );
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

bool BSpline1D::setDomain( const double * x, size_t nx, const double * y, const int & order )
{
  node_x_list.resize( nx );
  node_y_list.resize( nx );
  for ( size_t i = 0; i < nx; ++i, ++x, ++y )
  {
    node_x_list[i] = *x;
    node_y_list[i] = *y;
  }
  return setDomain( order );
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

bool BSpline1D::setDomain( const TLIST &x, const TLIST &y, const int & order )
{
  node_x_list = x;
  node_y_list = y;
  return setDomain( order );
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

bool BSpline1D::setDomain( const int & order )
{
  OK = false;
  ErrNo = -1;

  /// make sure the same length and enough number
  if ( node_y_list.size() != node_x_list.size() || node_x_list.size() < 2u )
    return false;

  n = node_x_list.size();
  k = order;

  /// make the knots list
  knots_list.resize( n + k );
  bknot_( &node_x_list.front(), &n, &k, &knots_list.front() );

  /// make bcoef list
  work_list.resize( 3*k );
  q_list.resize(( 2*k - 1 )*n );
  bcoef_list.resize( n );
  ErrNo = bintk_( &node_x_list.front(), &node_y_list.front(), &knots_list.front(), &n, &k, &bcoef_list.front(), &q_list.front(), &work_list.front() );

  if ( ErrNo == 0 )
  {
    inbv = 1;
    OK = true;
  }
  return OK;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

double BSpline1D::evaluate( double x , int ideriv )
{
  if ( !OK )
    return 0.0;

  if ( x < node_x_list.front() )
    if ( ideriv == 0 )
      return node_y_list.front();
    else
      return 0.0;
  if ( x > node_x_list.back() )
    if ( ideriv == 0 )
      return node_y_list.back();
    else
      return 0.0;

  ::ErrNo = &ErrNo;

  return bvalu_( &knots_list.front(), &bcoef_list.front(), &n, &k, &ideriv, &x, &inbv, &work_list.front() );
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

BSpline2D::BSpline2D( const double * x, size_t nx, const double * y, size_t ny, const double * z, const int & order ):
    OK( false ), ErrNo(-1)
{
  setDomain( x, nx, y, ny, z, order );
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

BSpline2D::BSpline2D( const TLIST &x, const TLIST &y, const TLIST &z, const int & order ):
    OK( false ), ErrNo(-1)
{
  setDomain( x, y, z, order );
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

bool BSpline2D::setDomain( const double * x, size_t nx, const double * y, size_t ny, const double * z, const int & order )
{
  node_x_list.resize( nx );
  for ( size_t i = 0; i < nx; ++i, ++x )
  {
    node_x_list[i] = *x;
  }
  node_y_list.resize( ny );
  for ( size_t i = 0; i < ny; ++i, ++y )
  {
    node_y_list[i] = *y;
  }
  size_t nz = nx * ny;
  node_z_list.resize( nz );
  for ( size_t i = 0; i < nz; ++i, ++z )
  {
    node_z_list[i] = *z;
  }

  return setDomain( order );
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

bool BSpline2D::setDomain( const TLIST &x, const TLIST &y, const TLIST &z, const int & order )
{
  node_x_list = x;
  node_y_list = y;
  node_z_list = z;
  return setDomain( order );
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

bool BSpline2D::setDomain( const int & order )
{
  OK = false;
  ErrNo = 0;

  /// make sure the same length and enough number
  if ( node_y_list.size() * node_x_list.size() != node_z_list.size() || node_x_list.size() < 3u || node_y_list.size() < 3u )
    return false;

  nx = node_x_list.size();
  ny = node_y_list.size();
  ldf = nx;
  
  kx = order;
  ky = order;

  /// make bcoef list
  tx.resize(nx+kx);
  ty.resize(ny+ky);
  work_list.resize( max( 3*nx*ny + max( 2*kx*(nx+1), 2*ky*(ny+1) ), 3*max(kx,ky)+ky));
  bcoef_list.resize( nx*ny );
  b2ink_( &node_x_list.front(), &nx, &node_y_list.front(), &ny, &node_z_list.front(), &ldf, &kx, &ky, &tx.front(), &ty.front(), &bcoef_list.front(), &work_list.front(), &ErrNo );

  --ErrNo;
  if ( ErrNo == 0 )
  {
    OK = true;
  }
  return OK;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

double BSpline2D::evaluate( double x , double y, int ideriv )
{
  if ( !OK )
    return 0.0;

  ::ErrNo = &ErrNo;

  return b2val___( &x, &y, &ideriv, &ideriv, &tx.front(), &ty.front(), &nx, &ny, &kx, &ky, &bcoef_list.front(), &work_list.front() );
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

BSpline3D::BSpline3D( const double * x, size_t nx, const double * y, size_t ny, const double * z, size_t nz, const double * f, const int & order ):
    OK( false ), ErrNo(-1)
{
  setDomain( x, nx, y, ny, z, nz, f, order );
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

BSpline3D::BSpline3D( const TLIST &x, const TLIST &y, const TLIST &z, const TLIST &f, const int & order ):
    OK( false ), ErrNo(-1)
{
  setDomain( x, y, z, f, order );
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

bool BSpline3D::setDomain( const double * x, size_t nx, const double * y, size_t ny, const double * z, size_t nz, const double * f, const int & order )
{
  node_x_list.resize( nx );
  for ( size_t i = 0; i < nx; ++i, ++x )
  {
    node_x_list[i] = *x;
  }
  node_y_list.resize( ny );
  for ( size_t i = 0; i < ny; ++i, ++y )
  {
    node_y_list[i] = *y;
  }
  node_z_list.resize( nz );
  for ( size_t i = 0; i < nz; ++i, ++z )
  {
    node_z_list[i] = *z;
  }
  size_t nf = nx * ny * nf;
  node_f_list.resize( nf );
  for ( size_t i = 0; i < nz; ++i, ++f )
  {
    node_f_list[i] = *f;
  }

  return setDomain( order );
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

bool BSpline3D::setDomain( const TLIST &x, const TLIST &y, const TLIST &z, const TLIST &f, const int & order )
{
  node_x_list = x;
  node_y_list = y;
  node_z_list = z;
  node_f_list = f;
  return setDomain( order );
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

bool BSpline3D::setDomain( const int & order )
{
  OK = false;
  ErrNo = 0;

  /// make sure the same length and enough number
  if ( node_y_list.size() * node_x_list.size() * node_z_list.size() != node_f_list.size() || node_x_list.size() < 3u || node_y_list.size() < 3u || node_z_list.size() < 3u)
    return false;

  nx = node_x_list.size();
  ny = node_y_list.size();
  nz = node_z_list.size();
  ldf1 = nx;
  ldf2 = ny;
  
  kx = order;
  ky = order;
  kz = order;

  /// make bcoef list
  tx.resize(nx+kx);
  ty.resize(ny+ky);
  tz.resize(nz+kz);
  work_list.resize( max( nx*ny*nz + max( 2*kx*(nx+1), 2*ky*(ny+1), 2*kz*(nz+1) ), 3*max(kx,ky,kz)+kz+ky*kz));
  bcoef_list.resize( nx*ny*nz );
  b3ink_( &node_x_list.front(), &nx, &node_y_list.front(), &ny, &node_z_list.front(), &nz, &node_f_list.front(), &ldf1, &ldf2, &kx, &ky, &kz, &tx.front(), &ty.front(), &tz.front(), &bcoef_list.front(), &work_list.front(), &ErrNo );

  --ErrNo;
  if ( ErrNo == 0 )
  {
    OK = true;
  }
  return OK;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

double BSpline3D::evaluate( double x, double y, double z, int ideriv )
{
  if ( !OK )
    return 0.0;

  ::ErrNo = &ErrNo;

  return b3val_( &x, &y, &z, &ideriv, &ideriv, &ideriv, &tx.front(), &ty.front(), &tz.front(), &nx, &ny, &nz, &kx, &ky, &kz, &bcoef_list.front(), &work_list.front() );
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%





//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
FUNCTION IMPLEMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/* bspline.f -- translated by f2c (version 20060506). */

#ifdef __cplusplus
extern "C"
{
#endif

  /* Table of constant values */

  static int c__1 = 1;

  /* Subroutine */ int bintk_( double *x, double *y, double *t, int *n, int *k,
                               double *bcoef, double *q, double *work )
  {
    /* System generated locals */
    int i__1, i__2;

    /* Local variables */
    static int i__, j, jj;
    static double xi;
    static int km1, np1, left, lenq, kpkm2;
    extern /* Subroutine */ int bnfac_( double *, int *, int *, int *
                                          , int *, int * );
    static int iflag;
    extern /* Subroutine */ int bnslv_( double *, int *, int *, int *
                                          , int *, double * ), bspvn_( double *, int *, int *,
                                                                       int *, double *, int *, double *, double *, int * );
    static int iwork, ilp1mx;

    /* ***BEGIN PROLOGUE  BINTK */
    /* ***DATE WRITTEN   800901   (YYMMDD) */
    /* ***REVISION DATE  820801   (YYMMDD) */
    /* ***REVISION HISTORY  (YYMMDD) */
    /*   000330  Modified array declarations.  (JEC) */

    /* ***CATEGORY NO.  E1A */
    /* ***KEYWORDS  B-SPLINE,DATA FITTING,INTERPOLATION,SPLINE */
    /* ***AUTHOR  AMOS, D. E., (SNLA) */
    /* ***PURPOSE  Produces the B-spline coefficients, BCOEF, of the */
    /*            B-spline of order K with knots T(I), I=1,...,N+K, which */
    /*            takes on the value Y(I) at X(I), I=1,...,N. */
    /* ***DESCRIPTION */

    /*     Written by Carl de Boor and modified by D. E. Amos */

    /*     References */

    /*          A Practical Guide to Splines by C. de Boor, Applied */
    /*          Mathematics Series 27, Springer, 1979. */

    /*     Abstract */

    /*         BINTK is the SPLINT routine of the reference. */

    /*         BINTK produces the B-spline coefficients, BCOEF, of the */
    /*         B-spline of order K with knots T(I), I=1,...,N+K, which */
    /*         takes on the value Y(I) at X(I), I=1,...,N.  The spline or */
    /*         any of its derivatives can be evaluated by calls to BVALU. */
    /*         The I-th equation of the linear system A*BCOEF = B for the */
    /*         coefficients of the interpolant enforces interpolation at */
    /*         X(I)), I=1,...,N.  Hence, B(I) = Y(I), all I, and A is */
    /*         a band matrix with 2K-1 bands if A is invertible. The matrix */
    /*         A is generated row by row and stored, diagonal by diagonal, */
    /*         in the rows of Q, with the main diagonal going into row K. */
    /*         The banded system is then solved by a call to BNFAC (which */
    /*         constructs the triangular factorization for A and stores it */
    /*         again in Q), followed by a call to BNSLV (which then */
    /*         obtains the solution BCOEF by substitution). BNFAC does no */
    /*         pivoting, since the total positivity of the matrix A makes */
    /*         this unnecessary.  The linear system to be solved is */
    /*         (theoretically) invertible if and only if */
    /*                 T(I) .LT. X(I)) .LT. T(I+K),        all I. */
    /*         Equality is permitted on the left for I=1 and on the right */
    /*         for I=N when K knots are used at X(1) or X(N).  Otherwise, */
    /*         violation of this condition is certain to lead to an error. */

    /*         BINTK calls BSPVN, BNFAC, BNSLV, XERROR */

    /*     Description of Arguments */
    /*         Input */
    /*           X       - vector of length N containing data point abscissa */
    /*                     in strictly increasing order. */
    /*           Y       - corresponding vector of length N containing data */
    /*                     point ordinates. */
    /*           T       - knot vector of length N+K */
    /*                     since T(1),..,T(K) .LE. X(1) and T(N+1),..,T(N+K) */
    /*                     .GE. X(N), this leaves only N-K knots (not nec- */
    /*                     essarily X(I)) values) interior to (X(1),X(N)) */
    /*           N       - number of data points, N .GE. K */
    /*           K       - order of the spline, K .GE. 1 */

    /*         Output */
    /*           BCOEF   - a vector of length N containing the B-spline */
    /*                     coefficients */
    /*           Q       - a work vector of length (2*K-1)*N, containing */
    /*                     the triangular factorization of the coefficient */
    /*                     matrix of the linear system being solved.  The */
    /*                     coefficients for the interpolant of an */
    /*                     additional data set (X(I)),YY(I)), I=1,...,N */
    /*                     with the same abscissa can be obtained by loading */
    /*                     YY into BCOEF and then executing */
    /*                         call BNSLV(Q,2K-1,N,K-1,K-1,BCOEF) */
    /*           WORK    - work vector of length 2*K */

    /*     Error Conditions */
    /*         Improper  input is a fatal error */
    /*         Singular system of equations is a fatal error */
    /* ***REFERENCES  D.E. AMOS, *COMPUTATION WITH SPLINES AND B-SPLINES*, */
    /*                 SAND78-1968,SANDIA LABORATORIES,MARCH,1979. */
    /*               C. DE BOOR, *PACKAGE FOR CALCULATING WITH B-SPLINES*, */
    /*                 SIAM JOURNAL ON NUMERICAL ANALYSIS, VOLUME 14, NO. 3, */
    /*                 JUNE 1977, PP. 441-472. */
    /*               C. DE BOOR, *A PRACTICAL GUIDE TO SPLINES*, APPLIED */
    /*                 MATHEMATICS SERIES 27, SPRINGER, 1979. */
    /* ***ROUTINES CALLED  BNFAC,BNSLV,BSPVN,XERROR */
    /* ***END PROLOGUE  BINTK */


    /*     DIMENSION Q(2*K-1,N), T(N+K) */
    /* ***FIRST EXECUTABLE STATEMENT  BINTK */
    /* Parameter adjustments */
    --t;
    --bcoef;
    --y;
    --x;
    --q;
    --work;

    /* Function Body */
    if ( *k < 1 )
    {
      goto L100;
    }
    if ( *n < *k )
    {
      goto L105;
    }
    jj = *n - 1;
    if ( jj == 0 )
    {
      goto L6;
    }
    i__1 = jj;
    for ( i__ = 1; i__ <= i__1; ++i__ )
    {
      if ( x[i__] >= x[i__ + 1] )
      {
        goto L110;
      }
      /* L5: */
    }
L6:
    np1 = *n + 1;
    km1 = *k - 1;
    kpkm2 = km1 << 1;
    left = *k;
    /*                ZERO OUT ALL ENTRIES OF Q */
    lenq = *n * ( *k + km1 );
    i__1 = lenq;
    for ( i__ = 1; i__ <= i__1; ++i__ )
    {
      q[i__] = ( float )0.;
      /* L10: */
    }

    /*  ***   LOOP OVER I TO CONSTRUCT THE  N  INTERPOLATION EQUATIONS */
    i__1 = *n;
    for ( i__ = 1; i__ <= i__1; ++i__ )
    {
      xi = x[i__];
      /* Computing MIN */
      i__2 = i__ + *k;
      ilp1mx = min( i__2, np1 );
      /*        *** FIND  LEFT  IN THE CLOSED INTERVAL (I,I+K-1) SUCH THAT */
      /*                T(LEFT) .LE. X(I) .LT. T(LEFT+1) */
      /*        MATRIX IS SINGULAR IF THIS IS NOT POSSIBLE */
      left = max( left, i__ );
      if ( xi < t[left] )
      {
        goto L80;
      }
L20:
      if ( xi < t[left + 1] )
      {
        goto L30;
      }
      ++left;
      if ( left < ilp1mx )
      {
        goto L20;
      }
      --left;
      if ( xi > t[left + 1] )
      {
        goto L80;
      }
      /*        *** THE I-TH EQUATION ENFORCES INTERPOLATION AT XI, HENCE */
      /*        A(I,J) = B(J,K,T)(XI), ALL J. ONLY THE  K  ENTRIES WITH  J = */
      /*        LEFT-K+1,...,LEFT ACTUALLY MIGHT BE NONZERO. THESE  K  NUMBERS */
      /*        ARE RETURNED, IN  BCOEF (USED FOR TEMP.STORAGE HERE), BY THE */
      /*        FOLLOWING */
L30:
      bspvn_( &t[1], k, k, &c__1, &xi, &left, &bcoef[1], &work[1], &iwork );
      /*        WE THEREFORE WANT  BCOEF(J) = B(LEFT-K+J)(XI) TO GO INTO */
      /*        A(I,LEFT-K+J), I.E., INTO  Q(I-(LEFT+J)+2*K,(LEFT+J)-K) SINCE */
      /*        A(I+J,J)  IS TO GO INTO  Q(I+K,J), ALL I,J,  IF WE CONSIDER  Q */
      /*        AS A TWO-DIM. ARRAY , WITH  2*K-1  ROWS (SEE COMMENTS IN */
      /*        BNFAC). IN THE PRESENT PROGRAM, WE TREAT  Q  AS AN EQUIVALENT */
      /*        ONE-DIMENSIONAL ARRAY (BECAUSE OF FORTRAN RESTRICTIONS ON */
      /*        DIMENSION STATEMENTS) . WE THEREFORE WANT  BCOEF(J) TO GO INTO */
      /*        ENTRY */
      /*            I -(LEFT+J) + 2*K + ((LEFT+J) - K-1)*(2*K-1) */
      /*                   =  I-LEFT+1 + (LEFT -K)*(2*K-1) + (2*K-2)*J */
      /*        OF  Q . */
      jj = i__ - left + 1 + ( left - *k ) * ( *k + km1 );
      i__2 = *k;
      for ( j = 1; j <= i__2; ++j )
      {
        jj += kpkm2;
        q[jj] = bcoef[j];
        /* L40: */
      }
      /* L50: */
    }

    /*     ***OBTAIN FACTORIZATION OF  A  , STORED AGAIN IN  Q. */
    i__1 = *k + km1;
    bnfac_( &q[1], &i__1, n, &km1, &km1, &iflag );
    switch ( iflag )
    {
    case 1:
      goto L60;
    case 2:
      goto L90;
    }
    /*     *** SOLVE  A*BCOEF = Y  BY BACKSUBSTITUTION */
L60:
    i__1 = *n;
    for ( i__ = 1; i__ <= i__1; ++i__ )
    {
      bcoef[i__] = y[i__];
      /* L70: */
    }
    i__1 = *k + km1;
    bnslv_( &q[1], &i__1, n, &km1, &km1, &bcoef[1] );
    return 0;


L80:
    /*      CALL XERROR(  'BINTK,  SOME ABSCISSA WAS NOT IN THE SUPPORT OF THE */
    /*     1 CORRESPONDING BASIS FUNCTION AND THE SYSTEM IS SINGULAR.',108,2,1 */
    /*     2) */
    return 1;
L90:
    /*      CALL XERROR( 'BINTK,  THE SYSTEM OF SOLVER DETECTS A SINGULAR SYST */
    /*     1EM ALTHOUGH THE THEORETICAL CONDITIONS FOR A SOLUTION WERE SATISFI */
    /*     2ED.',121,8,1) */
    return 2;
L100:
    /*      CALL XERROR( ' BINTK,  K DOES NOT SATISFY K.GE.1', 34, 2, 1) */
    return 3;
L105:
    /*      CALL XERROR( ' BINTK,  N DOES NOT SATISFY N.GE.K', 34, 2, 1) */
    return 4;
L110:
    /*      CALL XERROR( ' BINTK,  X(I) DOES NOT SATISFY X(I).LT.X(I+1) FOR SO */
    /*     1ME I', 56, 2, 1) */
    return 5;
  } /* bintk_ */

  /* Subroutine */ int bnfac_( double *w, int *nroww, int *nrow, int *
                               nbandl, int *nbandu, int *iflag )
  {
    /* System generated locals */
    int w_dim1, w_offset, i__1, i__2, i__3;

    /* Local variables */
    static int i__, j, k, ipk, jmax, kmax, midmk;
    static double pivot;
    static int nrowm1, middle;
    static double factor;

    /* ***BEGIN PROLOGUE  BNFAC */
    /* ***REFER TO  BINT4,BINTK */

    /*  BNFAC is the BANFAC routine from */
    /*        * A Practical Guide to Splines *  by C. de Boor */

    /*  Returns in  W  the lu-factorization (without pivoting) of the banded */
    /*  matrix  A  of order  NROW  with  (NBANDL + 1 + NBANDU) bands or diag- */
    /*  onals in the work array  W . */

    /* *****  I N P U T  ****** */
    /*  W.....Work array of size  (NROWW,NROW)  containing the interesting */
    /*        part of a banded matrix  A , with the diagonals or bands of  A */
    /*        stored in the rows of  W , while columns of  A  correspond to */
    /*        columns of  W . This is the storage mode used in  LINPACK  and */
    /*        results in efficient innermost loops. */
    /*           Explicitly,  A  has  NBANDL  bands below the diagonal */
    /*                            +     1     (main) diagonal */
    /*                            +   NBANDU  bands above the diagonal */
    /*        and thus, with    MIDDLE = NBANDU + 1, */
    /*          A(I+J,J)  is in  W(I+MIDDLE,J)  for I=-NBANDU,...,NBANDL */
    /*                                              J=1,...,NROW . */
    /*        For example, the interesting entries of A (1,2)-banded matrix */
    /*        of order  9  would appear in the first  1+1+2 = 4  rows of  W */
    /*        as follows. */
    /*                          13 24 35 46 57 68 79 */
    /*                       12 23 34 45 56 67 78 89 */
    /*                    11 22 33 44 55 66 77 88 99 */
    /*                    21 32 43 54 65 76 87 98 */

    /*        All other entries of  W  not identified in this way with an en- */
    /*        try of  A  are never referenced . */
    /*  NROWW.....Row dimension of the work array  W . */
    /*        must be  .GE.  NBANDL + 1 + NBANDU  . */
    /*  NBANDL.....Number of bands of  A  below the main diagonal */
    /*  NBANDU.....Number of bands of  A  above the main diagonal . */

    /* *****  O U T P U T  ****** */
    /*  IFLAG.....Integer indicating success( = 1) or failure ( = 2) . */
    /*     If  IFLAG = 1, then */
    /*  W.....contains the LU-factorization of  A  into a unit lower triangu- */
    /*        lar matrix  L  and an upper triangular matrix  U (both banded) */
    /*        and stored in customary fashion over the corresponding entries */
    /*        of  A . This makes it possible to solve any particular linear */
    /*        system  A*X = B  for  X  by A */
    /*              CALL BNSLV ( W, NROWW, NROW, NBANDL, NBANDU, B ) */
    /*        with the solution X  contained in  B  on return . */
    /*     If  IFLAG = 2, then */
    /*        one of  NROW-1, NBANDL,NBANDU failed to be nonnegative, or else */
    /*        one of the potential pivots was found to be zero indicating */
    /*        that  A  does not have an LU-factorization. This implies that */
    /*        A  is singular in case it is totally positive . */

    /* *****  M E T H O D  ****** */
    /*     Gauss elimination  W I T H O U T  pivoting is used. The routine is */
    /*  intended for use with matrices  A  which do not require row inter- */
    /*  changes during factorization, especially for the  T O T A L L Y */
    /*  P O S I T I V E  matrices which occur in spline calculations. */
    /*     The routine should not be used for an arbitrary banded matrix. */
    /* ***ROUTINES CALLED  (NONE) */
    /* ***END PROLOGUE  BNFAC */


    /* ***FIRST EXECUTABLE STATEMENT  BNFAC */
    /* Parameter adjustments */
    w_dim1 = *nroww;
    w_offset = 1 + w_dim1;
    w -= w_offset;

    /* Function Body */
    *iflag = 1;
    middle = *nbandu + 1;
    /*                         W(MIDDLE,.) CONTAINS THE MAIN DIAGONAL OF  A . */
    nrowm1 = *nrow - 1;
    if ( nrowm1 < 0 )
    {
      goto L120;
    }
    else if ( nrowm1 == 0 )
    {
      goto L110;
    }
    else
    {
      goto L10;
    }
L10:
    if ( *nbandl > 0 )
    {
      goto L30;
    }
    /*                A IS UPPER TRIANGULAR. CHECK THAT DIAGONAL IS NONZERO . */
    i__1 = nrowm1;
    for ( i__ = 1; i__ <= i__1; ++i__ )
    {
      if ( w[middle + i__ * w_dim1] == ( float )0. )
      {
        goto L120;
      }
      /* L20: */
    }
    goto L110;
L30:
    if ( *nbandu > 0 )
    {
      goto L60;
    }
    /*              A IS LOWER TRIANGULAR. CHECK THAT DIAGONAL IS NONZERO AND */
    /*                 DIVIDE EACH COLUMN BY ITS DIAGONAL . */
    i__1 = nrowm1;
    for ( i__ = 1; i__ <= i__1; ++i__ )
    {
      pivot = w[middle + i__ * w_dim1];
      if ( pivot == ( float )0. )
      {
        goto L120;
      }
      /* Computing MIN */
      i__2 = *nbandl, i__3 = *nrow - i__;
      jmax = min( i__2, i__3 );
      i__2 = jmax;
      for ( j = 1; j <= i__2; ++j )
      {
        w[middle + j + i__ * w_dim1] /= pivot;
        /* L40: */
      }
      /* L50: */
    }
    return 0;

    /*        A  IS NOT JUST A TRIANGULAR MATRIX. CONSTRUCT LU FACTORIZATION */
L60:
    i__1 = nrowm1;
    for ( i__ = 1; i__ <= i__1; ++i__ )
    {
      /*                                  W(MIDDLE,I)  IS PIVOT FOR I-TH STEP . */
      pivot = w[middle + i__ * w_dim1];
      if ( pivot == ( float )0. )
      {
        goto L120;
      }
      /*                 JMAX  IS THE NUMBER OF (NONZERO) ENTRIES IN COLUMN  I */
      /*                     BELOW THE DIAGONAL . */
      /* Computing MIN */
      i__2 = *nbandl, i__3 = *nrow - i__;
      jmax = min( i__2, i__3 );
      /*              DIVIDE EACH ENTRY IN COLUMN  I  BELOW DIAGONAL BY PIVOT . */
      i__2 = jmax;
      for ( j = 1; j <= i__2; ++j )
      {
        w[middle + j + i__ * w_dim1] /= pivot;
        /* L70: */
      }
      /*                 KMAX  IS THE NUMBER OF (NONZERO) ENTRIES IN ROW  I  TO */
      /*                     THE RIGHT OF THE DIAGONAL . */
      /* Computing MIN */
      i__2 = *nbandu, i__3 = *nrow - i__;
      kmax = min( i__2, i__3 );
      /*                  SUBTRACT  A(I,I+K)*(I-TH COLUMN) FROM (I+K)-TH COLUMN */
      /*                  (BELOW ROW  I ) . */
      i__2 = kmax;
      for ( k = 1; k <= i__2; ++k )
      {
        ipk = i__ + k;
        midmk = middle - k;
        factor = w[midmk + ipk * w_dim1];
        i__3 = jmax;
        for ( j = 1; j <= i__3; ++j )
        {
          w[midmk + j + ipk * w_dim1] -= w[middle + j + i__ * w_dim1] *
                                         factor;
          /* L80: */
        }
        /* L90: */
      }
      /* L100: */
    }
    /*                                       CHECK THE LAST DIAGONAL ENTRY . */
L110:
    if ( w[middle + *nrow * w_dim1] != ( float )0. )
    {
      return 0;
    }
L120:
    *iflag = 2;
    return 0;
  } /* bnfac_ */

  /* Subroutine */ int bnslv_( double *w, int *nroww, int *nrow, int *
                               nbandl, int *nbandu, double *b )
  {
    /* System generated locals */
    int w_dim1, w_offset, i__1, i__2, i__3;

    /* Local variables */
    static int i__, j, jmax, nrowm1, middle;

    /* ***BEGIN PROLOGUE  BNSLV */
    /* ***REFER TO  BINT4,BINTK */

    /*  BNSLV is the BANSLV routine from */
    /*        * A Practical Guide to Splines *  by C. de Boor */

    /*  Companion routine to  BNFAC . It returns the solution  X  of the */
    /*  linear system  A*X = B  in place of  B , given the LU-factorization */
    /*  for  A  in the work array  W from BNFAC. */

    /* *****  I N P U T  ****** */
    /*  W, NROWW,NROW,NBANDL,NBANDU.....describe the LU-factorization of a */
    /*        banded matrix  A  of order  NROW  as constructed in  BNFAC . */
    /*        For details, see  BNFAC . */
    /*  B.....Right side of the system to be solved . */

    /* *****  O U T P U T  ****** */
    /*  B.....Contains the solution  X , of order  NROW . */

    /* *****  M E T H O D  ****** */
    /*     (With  A = L*U, as stored in  W,) the unit lower triangular system */
    /*  L(U*X) = B  is solved for  Y = U*X, and  Y  stored in  B . Then the */
    /*  upper triangular system  U*X = Y  is solved for  X  . The calcul- */
    /*  ations are so arranged that the innermost loops stay within columns. */
    /* ***ROUTINES CALLED  (NONE) */
    /* ***END PROLOGUE  BNSLV */

    /* ***FIRST EXECUTABLE STATEMENT  BNSLV */
    /* Parameter adjustments */
    --b;
    w_dim1 = *nroww;
    w_offset = 1 + w_dim1;
    w -= w_offset;

    /* Function Body */
    middle = *nbandu + 1;
    if ( *nrow == 1 )
    {
      goto L80;
    }
    nrowm1 = *nrow - 1;
    if ( *nbandl == 0 )
    {
      goto L30;
    }
    /*                                 FORWARD PASS */
    /*            FOR I=1,2,...,NROW-1, SUBTRACT  RIGHT SIDE(I)*(I-TH COLUMN */
    /*            OF  L )  FROM RIGHT SIDE  (BELOW I-TH ROW) . */
    i__1 = nrowm1;
    for ( i__ = 1; i__ <= i__1; ++i__ )
    {
      /* Computing MIN */
      i__2 = *nbandl, i__3 = *nrow - i__;
      jmax = min( i__2, i__3 );
      i__2 = jmax;
      for ( j = 1; j <= i__2; ++j )
      {
        b[i__ + j] -= b[i__] * w[middle + j + i__ * w_dim1];
        /* L10: */
      }
      /* L20: */
    }
    /*                                 BACKWARD PASS */
    /*            FOR I=NROW,NROW-1,...,1, DIVIDE RIGHT SIDE(I) BY I-TH DIAG- */
    /*            ONAL ENTRY OF  U, THEN SUBTRACT  RIGHT SIDE(I)*(I-TH COLUMN */
    /*            OF  U)  FROM RIGHT SIDE  (ABOVE I-TH ROW). */
L30:
    if ( *nbandu > 0 )
    {
      goto L50;
    }
    /*                                A  IS LOWER TRIANGULAR . */
    i__1 = *nrow;
    for ( i__ = 1; i__ <= i__1; ++i__ )
    {
      b[i__] /= w[i__ * w_dim1 + 1];
      /* L40: */
    }
    return 0;
L50:
    i__ = *nrow;
L60:
    b[i__] /= w[middle + i__ * w_dim1];
    /* Computing MIN */
    i__1 = *nbandu, i__2 = i__ - 1;
    jmax = min( i__1, i__2 );
    i__1 = jmax;
    for ( j = 1; j <= i__1; ++j )
    {
      b[i__ - j] -= b[i__] * w[middle - j + i__ * w_dim1];
      /* L70: */
    }
    --i__;
    if ( i__ > 1 )
    {
      goto L60;
    }
L80:
    b[1] /= w[middle + w_dim1];
    return 0;
  } /* bnslv_ */

  /* Subroutine */ int bspvn_( double *t, int *jhigh, int *k, int *
                               index, double *x, int *ileft, double *vnikx, double *work, int *
                               iwork )
  {
    /* System generated locals */
    int i__1;

    /* Local variables */
    static int l;
    static double vm;
    static int jp1, ipj, imjp1, jp1ml;
    static double vmprev;

    /* ***BEGIN PROLOGUE  BSPVN */
    /* ***DATE WRITTEN   800901   (YYMMDD) */
    /* ***REVISION DATE  820801   (YYMMDD) */
    /* ***CATEGORY NO.  E3,K6 */
    /* ***KEYWORDS  B-SPLINE,DATA FITTING,INTERPOLATION,SPLINE */
    /* ***AUTHOR  AMOS, D. E., (SNLA) */
    /* ***PURPOSE  Calculates the value of all (possibly) nonzero basis */
    /*            functions at X. */
    /* ***DESCRIPTION */

    /*     Written by Carl de Boor and modified by D. E. Amos */

    /*     Reference */
    /*         SIAM J. Numerical Analysis, 14, No. 3, June, 1977, pp.441-472. */

    /*     Abstract */
    /*         BSPVN is the BSPLVN routine of the reference. */

    /*         BSPVN calculates the value of all (possibly) nonzero basis */
    /*         functions at X of order MAX(JHIGH,(J+1)*(INDEX-1)), where */
    /*         T(K) .LE. X .LE. T(N+1) and J=IWORK is set inside the routine */
    /*         on the first call when INDEX=1.  ILEFT is such that T(ILEFT) */
    /*         .LE. X .LT. T(ILEFT+1).  A call to INTRV(T,N+1,X,ILO,ILEFT, */
    /*         MFLAG) produces the proper ILEFT.  BSPVN calculates using the */
    /*         basic algorithm needed in BSPVD.  If only basis functions are */
    /*         desired, setting JHIGH=K and INDEX=1 can be faster than */
    /*         calling BSPVD, but extra coding is required for derivatives */
    /*         (INDEX=2) and BSPVD is set up for this purpose. */

    /*         Left limiting values are set up as described in BSPVD. */

    /*     Description of Arguments */
    /*         Input */
    /*          T       - knot vector of length N+K, where */
    /*                    N = number of B-spline basis functions */
    /*                    N = sum of knot multiplicities-K */
    /*          JHIGH   - order of B-spline, 1 .LE. JHIGH .LE. K */
    /*          K       - highest possible order */
    /*          INDEX   - INDEX = 1 gives basis functions of order JHIGH */
    /*                          = 2 denotes previous entry with WORK, IWORK */
    /*                              values saved for subsequent calls to */
    /*                              BSPVN. */
    /*          X       - argument of basis functions, */
    /*                    T(K) .LE. X .LE. T(N+1) */
    /*          ILEFT   - largest int such that */
    /*                    T(ILEFT) .LE. X .LT. T(ILEFT+1) */

    /*         Output */
    /*          VNIKX   - vector of length K for spline values. */
    /*          WORK    - a work vector of length 2*K */
    /*          IWORK   - a work parameter.  Both WORK and IWORK contain */
    /*                    information necessary to continue for INDEX = 2. */
    /*                    When INDEX = 1 exclusively, these are scratch */
    /*                    variables and can be used for other purposes. */

    /*     Error Conditions */
    /*         Improper input is a fatal error. */
    /* ***REFERENCES  C. DE BOOR, *PACKAGE FOR CALCULATING WITH B-SPLINES*, */
    /*                 SIAM JOURNAL ON NUMERICAL ANALYSIS, VOLUME 14, NO. 3, */
    /*                 JUNE 1977, PP. 441-472. */
    /* ***ROUTINES CALLED  XERROR */
    /* ***END PROLOGUE  BSPVN */


    /*     DIMENSION T(ILEFT+JHIGH) */
    /*     CONTENT OF J, DELTAM, DELTAP IS EXPECTED UNCHANGED BETWEEN CALLS. */
    /*     WORK(I) = DELTAP(I), WORK(K+I) = DELTAM(I), I = 1,K */
    /* ***FIRST EXECUTABLE STATEMENT  BSPVN */
    /* Parameter adjustments */
    --t;
    --vnikx;
    --work;

    /* Function Body */
    if ( *k < 1 )
    {
      goto L90;
    }
    if ( *jhigh > *k || *jhigh < 1 )
    {
      goto L100;
    }
    if ( *index < 1 || *index > 2 )
    {
      goto L105;
    }
    if ( *x < t[*ileft] || *x > t[*ileft + 1] )
    {
      goto L110;
    }
    switch ( *index )
    {
    case 1:
      goto L10;
    case 2:
      goto L20;
    }
L10:
    *iwork = 1;
    vnikx[1] = ( float )1.;
    if ( *iwork >= *jhigh )
    {
      goto L40;
    }

L20:
    ipj = *ileft + *iwork;
    work[*iwork] = t[ipj] - *x;
    imjp1 = *ileft - *iwork + 1;
    work[*k + *iwork] = *x - t[imjp1];
    vmprev = ( float )0.;
    jp1 = *iwork + 1;
    i__1 = *iwork;
    for ( l = 1; l <= i__1; ++l )
    {
      jp1ml = jp1 - l;
      vm = vnikx[l] / ( work[l] + work[*k + jp1ml] );
      vnikx[l] = vm * work[l] + vmprev;
      vmprev = vm * work[*k + jp1ml];
      /* L30: */
    }
    vnikx[jp1] = vmprev;
    *iwork = jp1;
    if ( *iwork < *jhigh )
    {
      goto L20;
    }

L40:
    return 0;


L90:
    /*      CALL XERROR( ' BSPVN,  K DOES NOT SATISFY K.GE.1', 34, 2, 1) */
    return 0;
L100:
    /*      CALL XERROR( ' BSPVN,  JHIGH DOES NOT SATISFY 1.LE.JHIGH.LE.K', */
    /*     1 47, 2, 1) */
    return 0;
L105:
    /*      CALL XERROR( ' BSPVN,  INDEX IS NOT 1 OR 2',28,2,1) */
    return 0;
L110:
    /*      CALL XERROR( ' BSPVN,  X DOES NOT SATISFY T(ILEFT).LE.X.LE.T(ILEFT */
    /*     1+1)', 55, 2, 1) */
    return 0;
  } /* bspvn_ */

  /* Subroutine */ int intrv_( double *xt, int *lxt, double *x, int *ilo,
                               int *ileft, int *mflag )
  {
    static int ihi, istep, middle;

    /* ***BEGIN PROLOGUE  INTRV */
    /* ***DATE WRITTEN   800901   (YYMMDD) */
    /* ***REVISION DATE  820801   (YYMMDD) */
    /* ***CATEGORY NO.  E3,K6 */
    /* ***KEYWORDS  B-SPLINE,DATA FITTING,INTERPOLATION,SPLINE */
    /* ***AUTHOR  AMOS, D. E., (SNLA) */
    /* ***PURPOSE  Computes the largest int ILEFT in 1.LE.ILEFT.LE.LXT */
    /*            such that XT(ILEFT).LE.X where XT(*) is a subdivision */
    /*            of the X interval. */
    /* ***DESCRIPTION */

    /*     Written by Carl de Boor and modified by D. E. Amos */

    /*     Reference */
    /*         SIAM J. Numerical Analysis, 14, No. 3, June 1977, pp. 441-472. */

    /*     Abstract */
    /*         INTRV is the INTERV routine of the reference. */

    /*         INTRV computes the largest int ILEFT in 1 .LE. ILEFT .LE. */
    /*         LXT such that XT(ILEFT) .LE. X where XT(*) is a subdivision of */
    /*         the X interval.  Precisely, */

    /*                      X .LT. XT(1)                1         -1 */
    /*         if  XT(I) .LE. X .LT. XT(I+1)  then  ILEFT=I  , MFLAG=0 */
    /*           XT(LXT) .LE. X                         LXT        1, */

    /*         That is, when multiplicities are present in the break point */
    /*         to the left of X, the largest index is taken for ILEFT. */

    /*     Description of Arguments */
    /*         Input */
    /*          XT      - XT is a knot or break point vector of length LXT */
    /*          LXT     - length of the XT vector */
    /*          X       - argument */
    /*          ILO     - an initialization parameter which must be set */
    /*                    to 1 the first time the spline array XT is */
    /*                    processed by INTRV. */

    /*         Output */
    /*          ILO     - ILO contains information for efficient process- */
    /*                    ing after the initial call, and ILO must not be */
    /*                    changed by the user.  Distinct splines require */
    /*                    distinct ILO parameters. */
    /*          ILEFT   - largest int satisfying XT(ILEFT) .LE. X */
    /*          MFLAG   - signals when X lies out of bounds */

    /*     Error Conditions */
    /*         None */
    /* ***REFERENCES  C. DE BOOR, *PACKAGE FOR CALCULATING WITH B-SPLINES*, */
    /*                 SIAM JOURNAL ON NUMERICAL ANALYSIS, VOLUME 14, NO. 3, */
    /*                 JUNE 1977, PP. 441-472. */
    /* ***ROUTINES CALLED  (NONE) */
    /* ***END PROLOGUE  INTRV */


    /* ***FIRST EXECUTABLE STATEMENT  INTRV */
    /* Parameter adjustments */
    --xt;

    /* Function Body */
    ihi = *ilo + 1;
    if ( ihi < *lxt )
    {
      goto L10;
    }
    if ( *x >= xt[*lxt] )
    {
      goto L110;
    }
    if ( *lxt <= 1 )
    {
      goto L90;
    }
    *ilo = *lxt - 1;
    ihi = *lxt;

L10:
    if ( *x >= xt[ihi] )
    {
      goto L40;
    }
    if ( *x >= xt[*ilo] )
    {
      goto L100;
    }

    /* *** NOW X .LT. XT(IHI) . FIND LOWER BOUND */
    istep = 1;
L20:
    ihi = *ilo;
    *ilo = ihi - istep;
    if ( *ilo <= 1 )
    {
      goto L30;
    }
    if ( *x >= xt[*ilo] )
    {
      goto L70;
    }
    istep <<= 1;
    goto L20;
L30:
    *ilo = 1;
    if ( *x < xt[1] )
    {
      goto L90;
    }
    goto L70;
    /* *** NOW X .GE. XT(ILO) . FIND UPPER BOUND */
L40:
    istep = 1;
L50:
    *ilo = ihi;
    ihi = *ilo + istep;
    if ( ihi >= *lxt )
    {
      goto L60;
    }
    if ( *x < xt[ihi] )
    {
      goto L70;
    }
    istep <<= 1;
    goto L50;
L60:
    if ( *x >= xt[*lxt] )
    {
      goto L110;
    }
    ihi = *lxt;

    /* *** NOW XT(ILO) .LE. X .LT. XT(IHI) . NARROW THE INTERVAL */
L70:
    middle = ( *ilo + ihi ) / 2;
    if ( middle == *ilo )
    {
      goto L100;
    }
    /*     NOTE. IT IS ASSUMED THAT MIDDLE = ILO IN CASE IHI = ILO+1 */
    if ( *x < xt[middle] )
    {
      goto L80;
    }
    *ilo = middle;
    goto L70;
L80:
    ihi = middle;
    goto L70;
    /* *** SET OUTPUT AND RETURN */
L90:
    *mflag = -1;
    *ileft = 1;
    return 0;
L100:
    *mflag = 0;
    *ileft = *ilo;
    return 0;
L110:
    *mflag = 1;
    *ileft = *lxt;
    return 0;
  } /* intrv_ */

  /* Subroutine */ int b2ink_( double *x, int *nx, double *y, int *ny, double *
                               fcn, int *ldf, int *kx, int *ky, double *tx, double *ty, double
                               *bcoef, double *work, int *iflag )
  {
    /* System generated locals */
    int fcn_dim1, fcn_offset, bcoef_dim1, bcoef_offset, i__1;

    /* Local variables */
    static int i__, iw, npk;
    extern /* Subroutine */ int btpcf_( double *, int *, double *, int *,
                                          int *, double *, int *, double *, double * ), bknot_( double *,
                                              int *, int *, double * );

    /* ***BEGIN PROLOGUE  B2INK */
    /* ***DATE WRITTEN   25 MAY 1982 */
    /* ***REVISION DATE  25 MAY 1982 */
    /* ***REVISION HISTORY  (YYMMDD) */
    /*   000330  Modified array declarations.  (JEC) */
    /* ***CATEGORY NO.  E1A */
    /* ***KEYWORDS  INTERPOLATION, TWO-DIMENSIONS, GRIDDED DATA, SPLINES, */
    /*             PIECEWISE POLYNOMIALS */
    /* ***AUTHOR  BOISVERT, RONALD, NBS */
    /*             SCIENTIFIC COMPUTING DIVISION */
    /*             NATIONAL BUREAU OF STANDARDS */
    /*             WASHINGTON, DC 20234 */
    /* ***PURPOSE  B2INK DETERMINES A PIECEWISE POLYNOMIAL FUNCTION THAT */
    /*            INTERPOLATES TWO-DIMENSIONAL GRIDDED DATA. USERS SPECIFY */
    /*            THE POLYNOMIAL ORDER (DEGREE+1) OF THE INTERPOLANT AND */
    /*            (OPTIONALLY) THE KNOT SEQUENCE. */
    /* ***DESCRIPTION */

    /*   B2INK determines the parameters of a function that interpolates the */
    /*   two-dimensional gridded data (X(i),Y(j),FCN(i,j)) for i=1,..,NX and */
    /*   j=1,..,NY. The  interpolating  function  and  its  derivatives  may */
    /*   subsequently be evaluated by the function B2VAL. */

    /*   The interpolating  function  is  a  piecewise  polynomial  function */
    /*   represented as a tensor product of one-dimensional  B-splines.  The */
    /*   form of this function is */

    /*                          NX   NY */
    /*              S(x,y)  =  SUM  SUM  a   U (x) V (y) */
    /*                         i=1  j=1   ij  i     j */

    /*   where the functions U(i)  and  V(j)  are  one-dimensional  B-spline */
    /*   basis functions. The coefficients a(i,j) are chosen so that */

    /*         S(X(i),Y(j)) = FCN(i,j)   for i=1,..,NX and j=1,..,NY */

    /*   Note that  for  each  fixed  value  of  y  S(x,y)  is  a  piecewise */
    /*   polynomial function of x alone, and for each fixed value of x  S(x, */
    /*   y) is a piecewise polynomial function of y alone. In one  dimension */
    /*   a piecewise polynomial may  be  created  by  partitioning  a  given */
    /*   interval into subintervals and defining a distinct polynomial piece */
    /*   on each one. The points where adjacent subintervals meet are called */
    /*   knots. Each of the functions U(i) and V(j)  above  is  a  piecewise */
    /*   polynomial. */

    /*   Users of B2INK choose the order (degree+1) of the polynomial pieces */
    /*   used to define the piecewise polynomial in each  of  the  x  and  y */
    /*   directions (KX and KY).  Users  also  may  define  their  own  knot */
    /*   sequence in x and y separately (TX and TY).  If  IFLAG=0,  however, */
    /*   B2INK will choose sequences of knots that  result  in  a  piecewise */
    /*   polynomial interpolant with KX-2 continuous partial derivatives  in */
    /*   x and KY-2 continuous partial derivatives in y. (KX knots are taken */
    /*   near each endpoint, not-a-knot end conditions  are  used,  and  the */
    /*   remaining knots are placed at data points  if  KX  is  even  or  at */
    /*   midpoints between data points if KX is  odd.  The  y  direction  is */
    /*   treated similarly.) */

    /*   After a call to B2INK, all  information  necessary  to  define  the */
    /*   interpolating function are contained in the parameters NX, NY,  KX, */
    /*   KY, TX, TY, and BCOEF. These quantities should not be altered until */
    /*   after the last call of the evaluation routine B2VAL. */


    /*   I N P U T */
    /*   --------- */

    /*   X       Real 1D array (size NX) */
    /*           Array of x abcissae. Must be strictly increasing. */

    /*   NX      Integer scalar (.GE. 3) */
    /*           Number of x abcissae. */

    /*   Y       Real 1D array (size NY) */
    /*           Array of y abcissae. Must be strictly increasing. */

    /*   NY      Integer scalar (.GE. 3) */
    /*           Number of y abcissae. */

    /*   FCN     Real 2D array (size LDF by NY) */
    /*           Array of function values to interpolate. FCN(I,J) should */
    /*           contain the function value at the point (X(I),Y(J)) */

    /*   LDF     Integer scalar (.GE. NX) */
    /*           The actual leading dimension of FCN used in the calling */
    /*           calling program. */

    /*   KX      Integer scalar (.GE. 2, .LT. NX) */
    /*           The order of spline pieces in x. */
    /*           (Order = polynomial degree + 1) */

    /*   KY      Integer scalar (.GE. 2, .LT. NY) */
    /*           The order of spline pieces in y. */
    /*           (Order = polynomial degree + 1) */


    /*   I N P U T   O R   O U T P U T */
    /*   ----------------------------- */

    /*   TX      Real 1D array (size NX+KX) */
    /*           The knots in the x direction for the spline interpolant. */
    /*           If IFLAG=0 these are chosen by B2INK. */
    /*           If IFLAG=1 these are specified by the user. */
    /*                      (Must be non-decreasing.) */

    /*   TY      Real 1D array (size NY+KY) */
    /*           The knots in the y direction for the spline interpolant. */
    /*           If IFLAG=0 these are chosen by B2INK. */
    /*           If IFLAG=1 these are specified by the user. */
    /*                      (Must be non-decreasing.) */


    /*   O U T P U T */
    /*   ----------- */

    /*   BCOEF   Real 2D array (size NX by NY) */
    /*           Array of coefficients of the B-spline interpolant. */
    /*           This may be the same array as FCN. */


    /*   M I S C E L L A N E O U S */
    /*   ------------------------- */

    /*   WORK    Real 1D array (size NX*NY + max( 2*KX*(NX+1), */
    /*                                  2*KY*(NY+1) )) */
    /*           Array of working storage. */

    /*   IFLAG   Integer scalar. */
    /*           On input:  0 == knot sequence chosen by B2INK */
    /*                      1 == knot sequence chosen by user. */
    /*           On output: 1 == successful execution */
    /*                      2 == IFLAG out of range */
    /*                      3 == NX out of range */
    /*                      4 == KX out of range */
    /*                      5 == X not strictly increasing */
    /*                      6 == TX not non-decreasing */
    /*                      7 == NY out of range */
    /*                      8 == KY out of range */
    /*                      9 == Y not strictly increasing */
    /*                     10 == TY not non-decreasing */

    /* ***REFERENCES  CARL DE BOOR, A PRACTICAL GUIDE TO SPLINES, */
    /*                 SPRINGER-VERLAG, NEW YORK, 1978. */
    /*               CARL DE BOOR, EFFICIENT COMPUTER MANIPULATION OF TENSOR */
    /*                 PRODUCTS, ACM TRANSACTIONS ON MATHEMATICAL SOFTWARE, */
    /*                 VOL. 5 (1979), PP. 173-182. */
    /* ***ROUTINES CALLED  BTPCF,BKNOT */
    /* ***END PROLOGUE  B2INK */

    /*  ------------ */
    /*  DECLARATIONS */
    /*  ------------ */

    /*  PARAMETERS */


    /*  LOCAL VARIABLES */


    /*  ----------------------- */
    /*  CHECK VALIDITY OF INPUT */
    /*  ----------------------- */

    /* ***FIRST EXECUTABLE STATEMENT */
    /* Parameter adjustments */
    --x;
    bcoef_dim1 = *nx;
    bcoef_offset = 1 + bcoef_dim1;
    bcoef -= bcoef_offset;
    --y;
    fcn_dim1 = *ldf;
    fcn_offset = 1 + fcn_dim1;
    fcn -= fcn_offset;
    --tx;
    --ty;
    --work;

    /* Function Body */
    if ( *iflag < 0 || *iflag > 1 )
    {
      goto L920;
    }
    if ( *nx < 3 )
    {
      goto L930;
    }
    if ( *ny < 3 )
    {
      goto L970;
    }
    if ( *kx < 2 || *kx >= *nx )
    {
      goto L940;
    }
    if ( *ky < 2 || *ky >= *ny )
    {
      goto L980;
    }
    i__1 = *nx;
    for ( i__ = 2; i__ <= i__1; ++i__ )
    {
      if ( x[i__] <= x[i__ - 1] )
      {
        goto L950;
      }
      /* L10: */
    }
    i__1 = *ny;
    for ( i__ = 2; i__ <= i__1; ++i__ )
    {
      if ( y[i__] <= y[i__ - 1] )
      {
        goto L990;
      }
      /* L20: */
    }
    if ( *iflag == 0 )
    {
      goto L50;
    }
    npk = *nx + *kx;
    i__1 = npk;
    for ( i__ = 2; i__ <= i__1; ++i__ )
    {
      if ( tx[i__] < tx[i__ - 1] )
      {
        goto L960;
      }
      /* L30: */
    }
    npk = *ny + *ky;
    i__1 = npk;
    for ( i__ = 2; i__ <= i__1; ++i__ )
    {
      if ( ty[i__] < ty[i__ - 1] )
      {
        goto L1000;
      }
      /* L40: */
    }
L50:

    /*  ------------ */
    /*  CHOOSE KNOTS */
    /*  ------------ */

    if ( *iflag != 0 )
    {
      goto L100;
    }
    bknot_( &x[1], nx, kx, &tx[1] );
    bknot_( &y[1], ny, ky, &ty[1] );
L100:

    /*  ------------------------------- */
    /*  CONSTRUCT B-SPLINE COEFFICIENTS */
    /*  ------------------------------- */

    *iflag = 1;
    iw = *nx * *ny + 1;
    btpcf_( &x[1], nx, &fcn[fcn_offset], ldf, ny, &tx[1], kx, &work[1], &work[
              iw] );
    btpcf_( &y[1], ny, &work[1], ny, nx, &ty[1], ky, &bcoef[bcoef_offset], &
            work[iw] );
    goto L9999;

    /*  ----- */
    /*  EXITS */
    /*  ----- */

L920:
    /* ERR      CALL XERRWV('B2INK -   IFLAG=I1 IS OUT OF RANGE.', */
    /* ERR     *            35,2,1,1,IFLAG,I2,0,R1,R2) */
    *iflag = 2;
    goto L9999;

L930:
    *iflag = 3;
    /* ERR      CALL XERRWV('B2INK -   NX=I1 IS OUT OF RANGE.', */
    /* ERR     *            32,IFLAG,1,1,NX,I2,0,R1,R2) */
    goto L9999;

L940:
    *iflag = 4;
    /* ERR      CALL XERRWV('B2INK -   KX=I1 IS OUT OF RANGE.', */
    /* ERR     *            32,IFLAG,1,1,KX,I2,0,R1,R2) */
    goto L9999;

L950:
    *iflag = 5;
    /* ERR      CALL XERRWV('B2INK -   X ARRAY MUST BE STRICTLY INCREASING.', */
    /* ERR     *            46,IFLAG,1,0,I1,I2,0,R1,R2) */
    goto L9999;

L960:
    *iflag = 6;
    /* ERR      CALL XERRWV('B2INK -   TX ARRAY MUST BE NON-DECREASING.', */
    /* ERR     *            42,IFLAG,1,0,I1,I2,0,R1,R2) */
    goto L9999;

L970:
    *iflag = 7;
    /* ERR      CALL XERRWV('B2INK -   NY=I1 IS OUT OF RANGE.', */
    /* ERR     *            32,IFLAG,1,1,NY,I2,0,R1,R2) */
    goto L9999;

L980:
    *iflag = 8;
    /* ERR      CALL XERRWV('B2INK -   KY=I1 IS OUT OF RANGE.', */
    /* ERR     *            32,IFLAG,1,1,KY,I2,0,R1,R2) */
    goto L9999;

L990:
    *iflag = 9;
    /* ERR      CALL XERRWV('B2INK -   Y ARRAY MUST BE STRICTLY INCREASING.', */
    /* ERR     *            46,IFLAG,1,0,I1,I2,0,R1,R2) */
    goto L9999;

L1000:
    *iflag = 10;
    /* ERR      CALL XERRWV('B2INK -   TY ARRAY MUST BE NON-DECREASING.', */
    /* ERR     *            42,IFLAG,1,0,I1,I2,0,R1,R2) */
    goto L9999;

L9999:
    return 0;
  } /* b2ink_ */

  double b2val___( double *xval, double *yval, int *idx, int *idy, double *
                   tx, double *ty, int *nx, int *ny, int *kx, int *ky,
                   double *bcoef, double *work )
  {
    /* Initialized data */

    static int iloy = 1;
    static int inbvx = 1;

    /* System generated locals */
    int bcoef_dim1, bcoef_offset, i__1;
    double ret_val;

    /* Local variables */
    static int k, iw, kcol, inbv, mflag;
    extern double bvalu_( double *, double *, int *, int *, int *,
                            double *, int *, double * );
    static int lefty;
    extern /* Subroutine */ int intrv_( double *, int *, double *, int *,
                                          int *, int * );

    /* ***BEGIN PROLOGUE  B2VAL */
    /* ***DATE WRITTEN   25 MAY 1982 */
    /* ***REVISION DATE  25 MAY 1982 */
    /* ***CATEGORY NO.  E1A */
    /* ***KEYWORDS  INTERPOLATION, TWO-DIMENSIONS, GRIDDED DATA, SPLINES, */
    /*             PIECEWISE POLYNOMIALS */
    /* ***AUTHOR  BOISVERT, RONALD, NBS */
    /*             SCIENTIFIC COMPUTING DIVISION */
    /*             NATIONAL BUREAU OF STANDARDS */
    /*             WASHINGTON, DC 20234 */
    /* ***PURPOSE  B2VAL EVALUATES THE PIECEWISE POLYNOMIAL INTERPOLATING */
    /*            FUNCTION CONSTRUCTED BY THE ROUTINE B2INK OR ONE OF ITS */
    /*            PARTIAL DERIVATIVES. */
    /* ***DESCRIPTION */

    /*   B2VAL evaluates the tensor product piecewise polynomial interpolant */
    /*   constructed by the routine B2INK or one of its derivatives  at  the */
    /*   point (XVAL,YVAL). To evaluate the interpolant itself, set IDX=IDY= */
    /*   0, to evaluate the first partial with respect to x, set  IDX=1,IDY= */
    /*   0, and so on. */

    /*   B2VAL returns 0.0E0 if (XVAL,YVAL) is out of range. That is, if */
    /*            XVAL.LT.TX(1) .OR. XVAL.GT.TX(NX+KX) .OR. */
    /*            YVAL.LT.TY(1) .OR. YVAL.GT.TY(NY+NY) */
    /*   If the knots  TX  and  TY  were  chosen  by  B2INK,  then  this  is */
    /*   equivalent to */
    /*            XVAL.LT.X(1) .OR. XVAL.GT.X(NX)+EPSX .OR. */
    /*            YVAL.LT.Y(1) .OR. YVAL.GT.Y(NY)+EPSY */
    /*   where EPSX = 0.1*(X(NX)-X(NX-1)) and EPSY = 0.1*(Y(NY)-Y(NY-1)). */

    /*   The input quantities TX, TY, NX, NY, KX, KY, and  BCOEF  should  be */
    /*   unchanged since the last call of B2INK. */


    /*   I N P U T */
    /*   --------- */

    /*   XVAL    Real scalar */
    /*           X coordinate of evaluation point. */

    /*   YVAL    Real scalar */
    /*           Y coordinate of evaluation point. */

    /*   IDX     Integer scalar */
    /*           X derivative of piecewise polynomial to evaluate. */

    /*   IDY     Integer scalar */
    /*           Y derivative of piecewise polynomial to evaluate. */

    /*   TX      Real 1D array (size NX+KX) */
    /*           Sequence of knots defining the piecewise polynomial in */
    /*           the x direction.  (Same as in last call to B2INK.) */

    /*   TY      Real 1D array (size NY+KY) */
    /*           Sequence of knots defining the piecewise polynomial in */
    /*           the y direction.  (Same as in last call to B2INK.) */

    /*   NX      Integer scalar */
    /*           The number of interpolation points in x. */
    /*           (Same as in last call to B2INK.) */

    /*   NY      Integer scalar */
    /*           The number of interpolation points in y. */
    /*           (Same as in last call to B2INK.) */

    /*   KX      Integer scalar */
    /*           Order of polynomial pieces in x. */
    /*           (Same as in last call to B2INK.) */

    /*   KY      Integer scalar */
    /*           Order of polynomial pieces in y. */
    /*           (Same as in last call to B2INK.) */

    /*   BCOEF   Real 2D array (size NX by NY) */
    /*           The B-spline coefficients computed by B2INK. */

    /*   WORK    Real 1D array (size 3*max(KX,KY) + KY) */
    /*           A working storage array. */

    /* ***REFERENCES  CARL DE BOOR, A PRACTICAL GUIDE TO SPLINES, */
    /*                 SPRINGER-VERLAG, NEW YORK, 1978. */
    /* ***ROUTINES CALLED  INTRV,BVALU */
    /* ***END PROLOGUE  B2VAL */

    /*  ------------ */
    /*  DECLARATIONS */
    /*  ------------ */

    /*  PARAMETERS */


    /*  LOCAL VARIABLES */


    /* Parameter adjustments */
    --tx;
    --ty;
    bcoef_dim1 = *nx;
    bcoef_offset = 1 + bcoef_dim1;
    bcoef -= bcoef_offset;
    --work;

    /* Function Body */
    /*     SAVE ILOY    ,  INBVX */


    /* ***FIRST EXECUTABLE STATEMENT */
    ret_val = ( float )0.;
    i__1 = *ny + *ky;
    intrv_( &ty[1], &i__1, yval, &iloy, &lefty, &mflag );
    if ( mflag != 0 )
    {
      *ErrNo = 999;
      goto L100;
    }
    iw = *ky + 1;
    kcol = lefty - *ky;
    i__1 = *ky;
    for ( k = 1; k <= i__1; ++k )
    {
      ++kcol;
      work[k] = bvalu_( &tx[1], &bcoef[kcol * bcoef_dim1 + 1], nx, kx, idx,
                        xval, &inbvx, &work[iw] );
      if (*ErrNo != 0 )
         goto L100;
      /* L50: */
    }
    inbv = 1;
    kcol = lefty - *ky + 1;
    ret_val = bvalu_( &ty[kcol], &work[1], ky, ky, idy, yval, &inbv, &work[iw] )
              ;
L100:
    return ret_val;
  } /* b2val___ */

  /* Subroutine */ int b3ink_( double *x, int *nx, double *y, int *ny, double *
                               z__, int *nz, double *fcn, int *ldf1, int *ldf2, int *
                               kx, int *ky, int *kz, double *tx, double *ty, double *tz, double *
                               bcoef, double *work, int *iflag )
  {
    /* System generated locals */
    int fcn_dim1, fcn_dim2, fcn_offset, bcoef_dim1, bcoef_dim2,
    bcoef_offset, i__1, i__2, i__3;

    /* Local variables */
    static int i__, j, k, iw, loc, npk;
    extern /* Subroutine */ int btpcf_( double *, int *, double *, int *,
                                          int *, double *, int *, double *, double * ), bknot_( double *,
                                              int *, int *, double * );

    /* ***BEGIN PROLOGUE  B3INK */
    /* ***DATE WRITTEN   25 MAY 1982 */
    /* ***REVISION DATE  25 MAY 1982 */
    /* ***REVISION HISTORY  (YYMMDD) */
    /*   000330  Modified array declarations.  (JEC) */
    /* ***CATEGORY NO.  E1A */
    /* ***KEYWORDS  INTERPOLATION, THREE-DIMENSIONS, GRIDDED DATA, SPLINES, */
    /*             PIECEWISE POLYNOMIALS */
    /* ***AUTHOR  BOISVERT, RONALD, NBS */
    /*             SCIENTIFIC COMPUTING DIVISION */
    /*             NATIONAL BUREAU OF STANDARDS */
    /*             WASHINGTON, DC 20234 */
    /* ***PURPOSE  B3INK DETERMINES A PIECEWISE POLYNOMIAL FUNCTION THAT */
    /*            INTERPOLATES THREE-DIMENSIONAL GRIDDED DATA. USERS SPECIFY */
    /*            THE POLYNOMIAL ORDER (DEGREE+1) OF THE INTERPOLANT AND */
    /*            (OPTIONALLY) THE KNOT SEQUENCE. */
    /* ***DESCRIPTION */

    /*   B3INK determines the parameters of a function that interpolates the */
    /*   three-dimensional gridded data (X(i),Y(j),Z(k),FCN(i,j,k)) for i=1, */
    /*   ..,NX, j=1,..,NY, and k=1,..,NZ. The interpolating function and its */
    /*   derivatives may subsequently be evaluated by the function B3VAL. */

    /*   The interpolating  function  is  a  piecewise  polynomial  function */
    /*   represented as a tensor product of one-dimensional  B-splines.  The */
    /*   form of this function is */

    /*                      NX   NY   NZ */
    /*        S(x,y,z)  =  SUM  SUM  SUM  a   U (x) V (y) W (z) */
    /*                     i=1  j=1  k=1   ij  i     j     k */

    /*   where the functions U(i), V(j), and  W(k)  are  one-dimensional  B- */
    /*   spline basis functions. The coefficients a(i,j) are chosen so that */

    /*   S(X(i),Y(j),Z(k)) = FCN(i,j,k)  for i=1,..,NX, j=1,..,NY, k=1,..,NZ */

    /*   Note that for fixed values of y  and  z  S(x,y,z)  is  a  piecewise */
    /*   polynomial function of x alone, for fixed values of x and z  S(x,y, */
    /*   z) is a piecewise polynomial function of y  alone,  and  for  fixed */
    /*   values of x and y S(x,y,z)  is  a  function  of  z  alone.  In  one */
    /*   dimension a piecewise polynomial may be created by  partitioning  a */
    /*   given interval into subintervals and defining a distinct polynomial */
    /*   piece on each one. The points where adjacent subintervals meet  are */
    /*   called knots. Each of the functions U(i), V(j), and W(k) above is a */
    /*   piecewise polynomial. */

    /*   Users of B3INK choose the order (degree+1) of the polynomial pieces */
    /*   used to define the piecewise polynomial in each of the x, y, and  z */
    /*   directions (KX, KY, and KZ). Users also may define their  own  knot */
    /*   sequence in x, y, and z separately (TX, TY, and  TZ).  If  IFLAG=0, */
    /*   however, B3INK will choose sequences of  knots  that  result  in  a */
    /*   piecewise  polynomial  interpolant  with  KX-2  continuous  partial */
    /*   derivatives in x, KY-2 continuous partial derivatives in y, and KZ- */
    /*   2 continuous partial derivatives in z. (KX  knots  are  taken  near */
    /*   each endpoint in x, not-a-knot end conditions  are  used,  and  the */
    /*   remaining knots are placed at data points  if  KX  is  even  or  at */
    /*   midpoints between data points if KX is odd. The y and z  directions */
    /*   are treated similarly.) */

    /*   After a call to B3INK, all  information  necessary  to  define  the */
    /*   interpolating function are contained in the parameters NX, NY,  NZ, */
    /*   KX, KY, KZ, TX, TY, TZ, and BCOEF. These quantities should  not  be */
    /*   altered until after the last call of the evaluation routine B3VAL. */


    /*   I N P U T */
    /*   --------- */

    /*   X       Real 1D array (size NX) */
    /*           Array of x abcissae. Must be strictly increasing. */

    /*   NX      Integer scalar (.GE. 3) */
    /*           Number of x abcissae. */

    /*   Y       Real 1D array (size NY) */
    /*           Array of y abcissae. Must be strictly increasing. */

    /*   NY      Integer scalar (.GE. 3) */
    /*           Number of y abcissae. */

    /*   Z       Real 1D array (size NZ) */
    /*           Array of z abcissae. Must be strictly increasing. */

    /*   NZ      Integer scalar (.GE. 3) */
    /*           Number of z abcissae. */

    /*   FCN     Real 3D array (size LDF1 by LDF2 by NY) */
    /*           Array of function values to interpolate. FCN(I,J,K) should */
    /*           contain the function value at the point (X(I),Y(J),Z(K)) */

    /*   LDF1    Integer scalar (.GE. NX) */
    /*           The actual first dimension of FCN used in the */
    /*           calling program. */

    /*   LDF2    Integer scalar (.GE. NY) */
    /*           The actual second dimension of FCN used in the calling */
    /*           program. */

    /*   KX      Integer scalar (.GE. 2, .LT. NX) */
    /*           The order of spline pieces in x. */
    /*           (Order = polynomial degree + 1) */

    /*   KY      Integer scalar (.GE. 2, .LT. NY) */
    /*           The order of spline pieces in y. */
    /*           (Order = polynomial degree + 1) */

    /*   KZ      Integer scalar (.GE. 2, .LT. NZ) */
    /*           The order of spline pieces in z. */
    /*           (Order = polynomial degree + 1) */


    /*   I N P U T   O R   O U T P U T */
    /*   ----------------------------- */

    /*   TX      Real 1D array (size NX+KX) */
    /*           The knots in the x direction for the spline interpolant. */
    /*           If IFLAG=0 these are chosen by B3INK. */
    /*           If IFLAG=1 these are specified by the user. */
    /*                      (Must be non-decreasing.) */

    /*   TY      Real 1D array (size NY+KY) */
    /*           The knots in the y direction for the spline interpolant. */
    /*           If IFLAG=0 these are chosen by B3INK. */
    /*           If IFLAG=1 these are specified by the user. */
    /*                      (Must be non-decreasing.) */

    /*   TZ      Real 1D array (size NZ+KZ) */
    /*           The knots in the z direction for the spline interpolant. */
    /*           If IFLAG=0 these are chosen by B3INK. */
    /*           If IFLAG=1 these are specified by the user. */
    /*                      (Must be non-decreasing.) */


    /*   O U T P U T */
    /*   ----------- */

    /*   BCOEF   Real 3D array (size NX by NY by NZ) */
    /*           Array of coefficients of the B-spline interpolant. */
    /*           This may be the same array as FCN. */


    /*   M I S C E L L A N E O U S */
    /*   ------------------------- */

    /*   WORK    Real 1D array (size NX*NY*NZ + max( 2*KX*(NX+1), */
    /*                             2*KY*(NY+1), 2*KZ*(NZ+1) ) */
    /*           Array of working storage. */

    /*   IFLAG   Integer scalar. */
    /*           On input:  0 == knot sequence chosen by B2INK */
    /*                      1 == knot sequence chosen by user. */
    /*           On output: 1 == successful execution */
    /*                      2 == IFLAG out of range */
    /*                      3 == NX out of range */
    /*                      4 == KX out of range */
    /*                      5 == X not strictly increasing */
    /*                      6 == TX not non-decreasing */
    /*                      7 == NY out of range */
    /*                      8 == KY out of range */
    /*                      9 == Y not strictly increasing */
    /*                     10 == TY not non-decreasing */
    /*                     11 == NZ out of range */
    /*                     12 == KZ out of range */
    /*                     13 == Z not strictly increasing */
    /*                     14 == TY not non-decreasing */

    /* ***REFERENCES  CARL DE BOOR, A PRACTICAL GUIDE TO SPLINES, */
    /*                 SPRINGER-VERLAG, NEW YORK, 1978. */
    /*               CARL DE BOOR, EFFICIENT COMPUTER MANIPULATION OF TENSOR */
    /*                 PRODUCTS, ACM TRANSACTIONS ON MATHEMATICAL SOFTWARE, */
    /*                 VOL. 5 (1979), PP. 173-182. */
    /* ***ROUTINES CALLED  BTPCF,BKNOT */
    /* ***END PROLOGUE  B3INK */

    /*  ------------ */
    /*  DECLARATIONS */
    /*  ------------ */

    /*  PARAMETERS */


    /*  LOCAL VARIABLES */


    /*  ----------------------- */
    /*  CHECK VALIDITY OF INPUT */
    /*  ----------------------- */

    /* ***FIRST EXECUTABLE STATEMENT */
    /* Parameter adjustments */
    --x;
    --y;
    bcoef_dim1 = *nx;
    bcoef_dim2 = *ny;
    bcoef_offset = 1 + bcoef_dim1 * ( 1 + bcoef_dim2 );
    bcoef -= bcoef_offset;
    --z__;
    fcn_dim1 = *ldf1;
    fcn_dim2 = *ldf2;
    fcn_offset = 1 + fcn_dim1 * ( 1 + fcn_dim2 );
    fcn -= fcn_offset;
    --tx;
    --ty;
    --tz;
    --work;

    /* Function Body */
    if ( *iflag < 0 || *iflag > 1 )
    {
      goto L920;
    }
    if ( *nx < 3 )
    {
      goto L930;
    }
    if ( *ny < 3 )
    {
      goto L970;
    }
    if ( *nz < 3 )
    {
      goto L1010;
    }
    if ( *kx < 2 || *kx >= *nx )
    {
      goto L940;
    }
    if ( *ky < 2 || *ky >= *ny )
    {
      goto L980;
    }
    if ( *kz < 2 || *kz >= *nz )
    {
      goto L1020;
    }
    i__1 = *nx;
    for ( i__ = 2; i__ <= i__1; ++i__ )
    {
      if ( x[i__] <= x[i__ - 1] )
      {
        goto L950;
      }
      /* L10: */
    }
    i__1 = *ny;
    for ( i__ = 2; i__ <= i__1; ++i__ )
    {
      if ( y[i__] <= y[i__ - 1] )
      {
        goto L990;
      }
      /* L20: */
    }
    i__1 = *nz;
    for ( i__ = 2; i__ <= i__1; ++i__ )
    {
      if ( z__[i__] <= z__[i__ - 1] )
      {
        goto L1030;
      }
      /* L30: */
    }
    if ( *iflag == 0 )
    {
      goto L70;
    }
    npk = *nx + *kx;
    i__1 = npk;
    for ( i__ = 2; i__ <= i__1; ++i__ )
    {
      if ( tx[i__] < tx[i__ - 1] )
      {
        goto L960;
      }
      /* L40: */
    }
    npk = *ny + *ky;
    i__1 = npk;
    for ( i__ = 2; i__ <= i__1; ++i__ )
    {
      if ( ty[i__] < ty[i__ - 1] )
      {
        goto L1000;
      }
      /* L50: */
    }
    npk = *nz + *kz;
    i__1 = npk;
    for ( i__ = 2; i__ <= i__1; ++i__ )
    {
      if ( tz[i__] < tz[i__ - 1] )
      {
        goto L1040;
      }
      /* L60: */
    }
L70:

    /*  ------------ */
    /*  CHOOSE KNOTS */
    /*  ------------ */

    if ( *iflag != 0 )
    {
      goto L100;
    }
    bknot_( &x[1], nx, kx, &tx[1] );
    bknot_( &y[1], ny, ky, &ty[1] );
    bknot_( &z__[1], nz, kz, &tz[1] );
L100:

    /*  ------------------------------- */
    /*  CONSTRUCT B-SPLINE COEFFICIENTS */
    /*  ------------------------------- */

    *iflag = 1;
    iw = *nx * *ny * *nz + 1;

    /*     COPY FCN TO WORK IN PACKED FOR BTPCF */
    loc = 0;
    i__1 = *nz;
    for ( k = 1; k <= i__1; ++k )
    {
      i__2 = *ny;
      for ( j = 1; j <= i__2; ++j )
      {
        i__3 = *nx;
        for ( i__ = 1; i__ <= i__3; ++i__ )
        {
          ++loc;
          work[loc] = fcn[i__ + ( j + k * fcn_dim2 ) * fcn_dim1];
          /* L510: */
        }
      }
    }

    i__3 = *ny * *nz;
    btpcf_( &x[1], nx, &work[1], nx, &i__3, &tx[1], kx, &bcoef[bcoef_offset], &
            work[iw] );
    i__3 = *nx * *nz;
    btpcf_( &y[1], ny, &bcoef[bcoef_offset], ny, &i__3, &ty[1], ky, &work[1], &
            work[iw] );
    i__3 = *nx * *ny;
    btpcf_( &z__[1], nz, &work[1], nz, &i__3, &tz[1], kz, &bcoef[bcoef_offset],
            &work[iw] );
    goto L9999;

    /*  ----- */
    /*  EXITS */
    /*  ----- */

L920:
    /* ERR      CALL XERRWV('B3INK -   IFLAG=I1 IS OUT OF RANGE.', */
    /* ERR     *            35,2,1,1,IFLAG,I2,0,R1,R2) */
    *iflag = 2;
    goto L9999;

L930:
    *iflag = 3;
    /* ERR      CALL XERRWV('B3INK -   NX=I1 IS OUT OF RANGE.', */
    /* ERR     *            32,IFLAG,1,1,NX,I2,0,R1,R2) */
    goto L9999;

L940:
    *iflag = 4;
    /* ERR      CALL XERRWV('B3INK -   KX=I1 IS OUT OF RANGE.', */
    /* ERR     *            32,IFLAG,1,1,KX,I2,0,R1,R2) */
    goto L9999;

L950:
    *iflag = 5;
    /* ERR      CALL XERRWV('B3INK -   X ARRAY MUST BE STRICTLY INCREASING.', */
    /* ERR     *            46,IFLAG,1,0,I1,I2,0,R1,R2) */
    goto L9999;

L960:
    *iflag = 6;
    /* ERR      CALL XERRWV('B3INK -   TX ARRAY MUST BE NON-DECREASING.', */
    /* ERR     *            42,IFLAG,1,0,I1,I2,0,R1,R2) */
    goto L9999;

L970:
    *iflag = 7;
    /* ERR      CALL XERRWV('B3INK -   NY=I1 IS OUT OF RANGE.', */
    /* ERR     *            32,IFLAG,1,1,NY,I2,0,R1,R2) */
    goto L9999;

L980:
    *iflag = 8;
    /* ERR      CALL XERRWV('B3INK -   KY=I1 IS OUT OF RANGE.', */
    /* ERR     *            32,IFLAG,1,1,KY,I2,0,R1,R2) */
    goto L9999;

L990:
    *iflag = 9;
    /* ERR      CALL XERRWV('B3INK -   Y ARRAY MUST BE STRICTLY INCREASING.', */
    /* ERR     *            46,IFLAG,1,0,I1,I2,0,R1,R2) */
    goto L9999;

L1000:
    *iflag = 10;
    /* ERR      CALL XERRWV('B3INK -   TY ARRAY MUST BE NON-DECREASING.', */
    /* ERR     *            42,IFLAG,1,0,I1,I2,0,R1,R2) */
    goto L9999;

L1010:
    *iflag = 11;
    /* ERR      CALL XERRWV('B3INK -   NZ=I1 IS OUT OF RANGE.', */
    /* ERR     *            32,IFLAG,1,1,NZ,I2,0,R1,R2) */
    goto L9999;

L1020:
    *iflag = 12;
    /* ERR      CALL XERRWV('B3INK -   KZ=I1 IS OUT OF RANGE.', */
    /* ERR     *            32,IFLAG,1,1,KZ,I2,0,R1,R2) */
    goto L9999;

L1030:
    *iflag = 13;
    /* ERR      CALL XERRWV('B3INK -   Z ARRAY MUST BE STRICTLY INCREASING.', */
    /* ERR     *            46,IFLAG,1,0,I1,I2,0,R1,R2) */
    goto L9999;

L1040:
    *iflag = 14;
    /* ERR      CALL XERRWV('B3INK -   TZ ARRAY MUST BE NON-DECREASING.', */
    /* ERR     *            42,IFLAG,1,0,I1,I2,0,R1,R2) */
    goto L9999;

L9999:
    return 0;
  } /* b3ink_ */

  double b3val_( double *xval, double *yval, double *zval, int *idx, int *
                 idy, int *idz, double *tx, double *ty, double *tz, int *nx, int
                 *ny, int *nz, int *kx, int *ky, int *kz, double *bcoef,
                 double *work )
  {
    /* Initialized data */

    static int iloy = 1;
    static int iloz = 1;
    static int inbvx = 1;

    /* System generated locals */
    int bcoef_dim1, bcoef_dim2, bcoef_offset, i__1, i__2;
    double ret_val;

    /* Local variables */
    static int i__, j, k, iw, iz, izm1, inbv1, inbv2, mflag;
    extern double bvalu_( double *, double *, int *, int *, int *,
                            double *, int *, double * );
    static int kcoly, kcolz, lefty, leftz;
    extern /* Subroutine */ int intrv_( double *, int *, double *, int *,
                                          int *, int * );

    /* ***BEGIN PROLOGUE  B3VAL */
    /* ***DATE WRITTEN   25 MAY 1982 */
    /* ***REVISION DATE  25 MAY 1982 */
    /* ***CATEGORY NO.  E1A */
    /* ***KEYWORDS  INTERPOLATION, THREE-DIMENSIONS, GRIDDED DATA, SPLINES, */
    /*             PIECEWISE POLYNOMIALS */
    /* ***AUTHOR  BOISVERT, RONALD, NBS */
    /*             SCIENTIFIC COMPUTING DIVISION */
    /*             NATIONAL BUREAU OF STANDARDS */
    /*             WASHINGTON, DC 20234 */
    /* ***PURPOSE  B3VAL EVALUATES THE PIECEWISE POLYNOMIAL INTERPOLATING */
    /*            FUNCTION CONSTRUCTED BY THE ROUTINE B3INK OR ONE OF ITS */
    /*            PARTIAL DERIVATIVES. */
    /* ***DESCRIPTION */

    /*   B3VAL evaluates the tensor product piecewise polynomial interpolant */
    /*   constructed by the routine B3INK or one of its derivatives  at  the */
    /*   point (XVAL,YVAL,ZVAL). To evaluate  the  interpolant  itself,  set */
    /*   IDX=IDY=IDZ=0, to evaluate the first partial with respect to x, set */
    /*   IDX=1,IDY=IDZ=0, and so on. */

    /*   B3VAL returns 0.0E0 if (XVAL,YVAL,ZVAL) is out of range. That is, */
    /*            XVAL.LT.TX(1) .OR. XVAL.GT.TX(NX+KX) .OR. */
    /*            YVAL.LT.TY(1) .OR. YVAL.GT.TY(NY+NY) .OR. */
    /*            ZVAL.LT.TZ(1) .OR. ZVAL.GT.TZ(NZ+KZ) */
    /*   If the knots TX, TY, and TZ were chosen  by  B3INK,  then  this  is */
    /*   equivalent to */
    /*            XVAL.LT.X(1) .OR. XVAL.GT.X(NX)+EPSX .OR. */
    /*            YVAL.LT.Y(1) .OR. YVAL.GT.Y(NY)+EPSY .OR. */
    /*            ZVAL.LT.Z(1) .OR. ZVAL.GT.Z(NZ)+EPSZ */
    /*   where EPSX = 0.1*(X(NX)-X(NX-1)), EPSY =  0.1*(Y(NY)-Y(NY-1)),  and */
    /*   EPSZ = 0.1*(Z(NZ)-Z(NZ-1)). */

    /*   The input quantities TX, TY, TZ, NX, NY, NZ, KX, KY, KZ, and  BCOEF */
    /*   should remain unchanged since the last call of B3INK. */


    /*   I N P U T */
    /*   --------- */

    /*   XVAL    Real scalar */
    /*           X coordinate of evaluation point. */

    /*   YVAL    Real scalar */
    /*           Y coordinate of evaluation point. */

    /*   ZVAL    Real scalar */
    /*           Z coordinate of evaluation point. */

    /*   IDX     Integer scalar */
    /*           X derivative of piecewise polynomial to evaluate. */

    /*   IDY     Integer scalar */
    /*           Y derivative of piecewise polynomial to evaluate. */

    /*   IDZ     Integer scalar */
    /*           Z derivative of piecewise polynomial to evaluate. */

    /*   TX      Real 1D array (size NX+KX) */
    /*           Sequence of knots defining the piecewise polynomial in */
    /*           the x direction.  (Same as in last call to B3INK.) */

    /*   TY      Real 1D array (size NY+KY) */
    /*           Sequence of knots defining the piecewise polynomial in */
    /*           the y direction.  (Same as in last call to B3INK.) */

    /*   TZ      Real 1D array (size NZ+KZ) */
    /*           Sequence of knots defining the piecewise polynomial in */
    /*           the z direction.  (Same as in last call to B3INK.) */

    /*   NX      Integer scalar */
    /*           The number of interpolation points in x. */
    /*           (Same as in last call to B3INK.) */

    /*   NY      Integer scalar */
    /*           The number of interpolation points in y. */
    /*           (Same as in last call to B3INK.) */

    /*   NZ      Integer scalar */
    /*           The number of interpolation points in z. */
    /*           (Same as in last call to B3INK.) */

    /*   KX      Integer scalar */
    /*           Order of polynomial pieces in x. */
    /*           (Same as in last call to B3INK.) */

    /*   KY      Integer scalar */
    /*           Order of polynomial pieces in y. */
    /*           (Same as in last call to B3INK.) */

    /*   KZ      Integer scalar */
    /*           Order of polynomial pieces in z. */
    /*           (Same as in last call to B3INK.) */

    /*   BCOEF   Real 2D array (size NX by NY by NZ) */
    /*           The B-spline coefficients computed by B3INK. */

    /*   WORK    Real 1D array (size KY*KZ+3*max(KX,KY,KZ)+KZ) */
    /*           A working storage array. */

    /* ***REFERENCES  CARL DE BOOR, A PRACTICAL GUIDE TO SPLINES, */
    /*                 SPRINGER-VERLAG, NEW YORK, 1978. */
    /* ***ROUTINES CALLED  INTRV,BVALU */
    /* ***END PROLOGUE  B3VAL */

    /*  ------------ */
    /*  DECLARATIONS */
    /*  ------------ */

    /*  PARAMETERS */


    /*  LOCAL VARIABLES */


    /* Parameter adjustments */
    --tx;
    --ty;
    --tz;
    bcoef_dim1 = *nx;
    bcoef_dim2 = *ny;
    bcoef_offset = 1 + bcoef_dim1 * ( 1 + bcoef_dim2 );
    bcoef -= bcoef_offset;
    --work;

    /* Function Body */
    /*     SAVE ILOY    ,  ILOZ    ,  INBVX */


    /* ***FIRST EXECUTABLE STATEMENT */
    ret_val = ( float )0.;
    i__1 = *ny + *ky;
    intrv_( &ty[1], &i__1, yval, &iloy, &lefty, &mflag );
    if ( mflag != 0 )
    {
      *ErrNo = 999;
      goto L100;
    }
    i__1 = *nz + *kz;
    intrv_( &tz[1], &i__1, zval, &iloz, &leftz, &mflag );
    if ( mflag != 0 )
    {
      *ErrNo = 999;
      goto L100;
    }
    iz = *ky * *kz + 1;
    iw = iz + *kz;
    kcolz = leftz - *kz;
    i__ = 0;
    i__1 = *kz;
    for ( k = 1; k <= i__1; ++k )
    {
      ++kcolz;
      kcoly = lefty - *ky;
      i__2 = *ky;
      for ( j = 1; j <= i__2; ++j )
      {
        ++i__;
        ++kcoly;
        work[i__] = bvalu_( &tx[1], &bcoef[( kcoly + kcolz * bcoef_dim2 ) *
                                           bcoef_dim1 + 1], nx, kx, idx, xval, &inbvx, &work[iw] );
        if (*ErrNo != 0 )
          goto L100;
        /* L50: */
      }
    }
    inbv1 = 1;
    izm1 = iz - 1;
    kcoly = lefty - *ky + 1;
    i__2 = *kz;
    for ( k = 1; k <= i__2; ++k )
    {
      i__ = ( k - 1 ) * *ky + 1;
      j = izm1 + k;
      work[j] = bvalu_( &ty[kcoly], &work[i__], ky, ky, idy, yval, &inbv1, &
                        work[iw] );
      if (*ErrNo != 0 )
        goto L100;
      /* L60: */
    }
    inbv2 = 1;
    kcolz = leftz - *kz + 1;
    ret_val = bvalu_( &tz[kcolz], &work[iz], kz, kz, idz, zval, &inbv2, &work[
                        iw] );
L100:
    return ret_val;
  } /* b3val_ */

  /* Subroutine */ int btpcf_( double *x, int *n, double *fcn, int *ldf,
                               int *nf, double *t, int *k, double *bcoef, double *work )
  {
    /* System generated locals */
    int fcn_dim1, fcn_offset, bcoef_dim1, bcoef_offset, i__1, i__2;

    /* Local variables */
    static int i__, j, k1, k2, iq, iw;
    extern /* Subroutine */ int bintk_( double *, double *, double *, int *,
                                          int *, double *, double *, double * ), bnslv_( double *, int *,
                                              int *, int *, int *, double * );

    /* ***BEGIN PROLOGUE  BTPCF */
    /* ***REFER TO  B2INK,B3INK */
    /* ***ROUTINES CALLED  BINTK,BNSLV */
    /* ***END PROLOGUE  BTPCF */

    /*  ----------------------------------------------------------------- */
    /*  BTPCF COMPUTES B-SPLINE INTERPOLATION COEFFICIENTS FOR NF SETS */
    /*  OF DATA STORED IN THE COLUMNS OF THE ARRAY FCN. THE B-SPLINE */
    /*  COEFFICIENTS ARE STORED IN THE ROWS OF BCOEF HOWEVER. */
    /*  EACH INTERPOLATION IS BASED ON THE N ABCISSA STORED IN THE */
    /*  ARRAY X, AND THE N+K KNOTS STORED IN THE ARRAY T. THE ORDER */
    /*  OF EACH INTERPOLATION IS K. THE WORK ARRAY MUST BE OF LENGTH */
    /*  AT LEAST 2*K*(N+1). */
    /*  ----------------------------------------------------------------- */

    /*  ------------ */
    /*  DECLARATIONS */
    /*  ------------ */

    /*  PARAMETERS */


    /*  LOCAL VARIABLES */


    /*  --------------------------------------------- */
    /*  CHECK FOR NULL INPUT AND PARTITION WORK ARRAY */
    /*  --------------------------------------------- */

    /* ***FIRST EXECUTABLE STATEMENT */
    /* Parameter adjustments */
    --x;
    bcoef_dim1 = *nf;
    bcoef_offset = 1 + bcoef_dim1;
    bcoef -= bcoef_offset;
    fcn_dim1 = *ldf;
    fcn_offset = 1 + fcn_dim1;
    fcn -= fcn_offset;
    --t;
    --work;

    /* Function Body */
    if ( *nf <= 0 )
    {
      goto L500;
    }
    k1 = *k - 1;
    k2 = k1 + *k;
    iq = *n + 1;
    iw = iq + k2 * *n + 1;

    /*  ----------------------------- */
    /*  COMPUTE B-SPLINE COEFFICIENTS */
    /*  ----------------------------- */


    /*   FIRST DATA SET */

    bintk_( &x[1], &fcn[fcn_offset], &t[1], n, k, &work[1], &work[iq], &work[
              iw] );
    i__1 = *n;
    for ( i__ = 1; i__ <= i__1; ++i__ )
    {
      bcoef[i__ * bcoef_dim1 + 1] = work[i__];
      /* L20: */
    }

    /*  ALL REMAINING DATA SETS BY BACK-SUBSTITUTION */

    if ( *nf == 1 )
    {
      goto L500;
    }
    i__1 = *nf;
    for ( j = 2; j <= i__1; ++j )
    {
      i__2 = *n;
      for ( i__ = 1; i__ <= i__2; ++i__ )
      {
        work[i__] = fcn[i__ + j * fcn_dim1];
        /* L50: */
      }
      bnslv_( &work[iq], &k2, n, &k1, &k1, &work[1] );
      i__2 = *n;
      for ( i__ = 1; i__ <= i__2; ++i__ )
      {
        bcoef[j + i__ * bcoef_dim1] = work[i__];
        /* L60: */
      }
      /* L100: */
    }

    /*  ---- */
    /*  EXIT */
    /*  ---- */

L500:
    return 0;
  } /* btpcf_ */

  /* Subroutine */ int bknot_( double *x, int *n, int *k, double *t )
  {
    /* System generated locals */
    int i__1;

    /* Local variables */
    static int i__, j, ip1, ipj, npj;
    static double rnot;
    static int jstrt;

    /* ***BEGIN PROLOGUE  BKNOT */
    /* ***REFER TO  B2INK,B3INK */
    /* ***ROUTINES CALLED  (NONE) */
    /* ***END PROLOGUE  BKNOT */

    /*  -------------------------------------------------------------------- */
    /*  BKNOT CHOOSES A KNOT SEQUENCE FOR INTERPOLATION OF ORDER K AT THE */
    /*  DATA POINTS X(I), I=1,..,N.  THE N+K KNOTS ARE PLACED IN THE ARRAY */
    /*  T.  K KNOTS ARE PLACED AT EACH ENDPOINT AND NOT-A-KNOT END */
    /*  CONDITIONS ARE USED.  THE REMAINING KNOTS ARE PLACED AT DATA POINTS */
    /*  IF N IS EVEN AND BETWEEN DATA POINTS IF N IS ODD.  THE RIGHTMOST */
    /*  KNOT IS SHIFTED SLIGHTLY TO THE RIGHT TO INSURE PROPER INTERPOLATION */
    /*  AT X(N) (SEE PAGE 350 OF THE REFERENCE). */
    /*  -------------------------------------------------------------------- */

    /*  ------------ */
    /*  DECLARATIONS */
    /*  ------------ */

    /*  PARAMETERS */


    /*  LOCAL VARIABLES */



    /*  ---------------------------- */
    /*  PUT K KNOTS AT EACH ENDPOINT */
    /*  ---------------------------- */

    /*     (SHIFT RIGHT ENPOINTS SLIGHTLY -- SEE PG 350 OF REFERENCE) */
    /* Parameter adjustments */
    --x;
    --t;

    /* Function Body */
    rnot = x[*n] + ( x[*n] - x[*n - 1] ) * ( float ).1;
    i__1 = *k;
    for ( j = 1; j <= i__1; ++j )
    {
      t[j] = x[1];
      npj = *n + j;
      t[npj] = rnot;
      /* L110: */
    }

    /*  -------------------------- */
    /*  DISTRIBUTE REMAINING KNOTS */
    /*  -------------------------- */

    if ( *k % 2 == 1 )
    {
      goto L150;
    }

    /*     CASE OF EVEN K --  KNOTS AT DATA POINTS */

    i__ = *k / 2 - *k;
    jstrt = *k + 1;
    i__1 = *n;
    for ( j = jstrt; j <= i__1; ++j )
    {
      ipj = i__ + j;
      t[j] = x[ipj];
      /* L120: */
    }
    goto L200;

    /*     CASE OF ODD K --  KNOTS BETWEEN DATA POINTS */

L150:
    i__ = ( *k - 1 ) / 2 - *k;
    ip1 = i__ + 1;
    jstrt = *k + 1;
    i__1 = *n;
    for ( j = jstrt; j <= i__1; ++j )
    {
      ipj = i__ + j;
      t[j] = ( x[ipj] + x[ipj + 1] ) * ( float ).5;
      /* L160: */
    }
L200:

    return 0;
  } /* bknot_ */

  /* Subroutine */ int xerror_( char *messg, int *nmessg, int *nerr,
                                int *level, int messg_len )
  {
    /* ***BEGIN PROLOGUE  XERROR */
    /* ***DATE WRITTEN   790801   (YYMMDD) */
    /* ***REVISION DATE  820801   (YYMMDD) */
    /* ***CATEGORY NO.  R3C */
    /* ***KEYWORDS  ERROR,XERROR PACKAGE */
    /* ***AUTHOR  JONES, R. E., (SNLA) */
    /* ***PURPOSE  Processes an error (diagnostic) message. */
    /* ***DESCRIPTION */
    /*     Abstract */
    /*        XERROR processes a diagnostic message, in a manner */
    /*        determined by the value of LEVEL and the current value */
    /*        of the library error control flag, KONTRL. */
    /*        (See subroutine XSETF for details.) */

    /*     Description of Parameters */
    /*      --Input-- */
    /*        MESSG - the Hollerith message to be processed, containing */
    /*                no more than 72 characters. */
    /*        NMESSG- the actual number of characters in MESSG. */
    /*        NERR  - the error number associated with this message. */
    /*                NERR must not be zero. */
    /*        LEVEL - error category. */
    /*                =2 means this is an unconditionally fatal error. */
    /*                =1 means this is a recoverable error.  (I.e., it is */
    /*                   non-fatal if XSETF has been appropriately called.) */
    /*                =0 means this is a warning message only. */
    /*                =-1 means this is a warning message which is to be */
    /*                   printed at most once, regardless of how many */
    /*                   times this call is executed. */

    /*     Examples */
    /*        CALL XERROR('SMOOTH -- NUM WAS ZERO.',23,1,2) */
    /*        CALL XERROR('INTEG  -- LESS THAN FULL ACCURACY ACHIEVED.', */
    /*                    43,2,1) */
    /*        CALL XERROR('ROOTER -- ACTUAL ZERO OF F FOUND BEFORE INTERVAL F */
    /*    1ULLY COLLAPSED.',65,3,0) */
    /*        CALL XERROR('EXP    -- UNDERFLOWS BEING SET TO ZERO.',39,1,-1) */

    /*     Latest revision ---  19 MAR 1980 */
    /*     Written by Ron Jones, with SLATEC Common Math Library Subcommittee */
    /* ***REFERENCES  JONES R.E., KAHANER D.K., "XERROR, THE SLATEC ERROR- */
    /*                 HANDLING PACKAGE", SAND82-0800, SANDIA LABORATORIES, */
    /*                 1982. */
    /* ***ROUTINES CALLED  XERRWV */
    /* ***END PROLOGUE  XERROR */
    /* ***FIRST EXECUTABLE STATEMENT  XERROR */
    /* ERR      CALL XERRWV(MESSG,NMESSG,NERR,LEVEL,0,0,0,0,0.,0.) */
    return 0;
  } /* xerror_ */

  double bvalu_( double *t, double *a, int *n, int *k, int *ideriv,
                 double *x, int *inbv, double *work )
  {
    /* System generated locals */
    int i__1, i__2;
    double ret_val;

    /* Local variables */
    static int i__, j, j1, j2, jj, km1, ip1, ihi, imk, kmj, ipj, ilo, kpk;
    static double fkmj;
    static int ip1mj, mflag, imkpj;
    extern /* Subroutine */ int intrv_( double *, int *, double *, int *,
                                          int *, int * );
    static int iderp1, kmider, ihmkmj;

    /* ***BEGIN PROLOGUE  BVALU */
    /* ***DATE WRITTEN   800901   (YYMMDD) */
    /* ***REVISION DATE  820801   (YYMMDD) */
    /* ***CATEGORY NO.  E3,K6 */
    /* ***KEYWORDS  B-SPLINE,DATA FITTING,INTERPOLATION,SPLINE */
    /* ***AUTHOR  AMOS, D. E., (SNLA) */
    /* ***PURPOSE  Evaluates the B-representation of a B-spline at X for the */
    /*            function value or any of its derivatives. */
    /* ***DESCRIPTION */

    /*     Written by Carl de Boor and modified by D. E. Amos */

    /*     Reference */
    /*         SIAM J. Numerical Analysis, 14, No. 3, June, 1977, pp.441-472. */

    /*     Abstract */
    /*         BVALU is the BVALUE function of the reference. */

    /*         BVALU evaluates the B-representation (T,A,N,K) of a B-spline */
    /*         at X for the function value on IDERIV = 0 or any of its */
    /*         derivatives on IDERIV = 1,2,...,K-1.  Right limiting values */
    /*         (right derivatives) are returned except at the right end */
    /*         point X=T(N+1) where left limiting values are computed.  The */
    /*         spline is defined on T(K) .LE. X .LE. T(N+1).  BVALU returns */
    /*         a fatal error message when X is outside of this interval. */

    /*         To compute left derivatives or left limiting values at a */
    /*         knot T(I), replace N by I-1 and set X=T(I), I=K+1,N+1. */

    /*         BVALU calls INTRV */

    /*     Description of Arguments */
    /*         Input */
    /*          T       - knot vector of length N+K */
    /*          A       - B-spline coefficient vector of length N */
    /*          N       - number of B-spline coefficients */
    /*                    N = sum of knot multiplicities-K */
    /*          K       - order of the B-spline, K .GE. 1 */
    /*          IDERIV  - order of the derivative, 0 .LE. IDERIV .LE. K-1 */
    /*                    IDERIV=0 returns the B-spline value */
    /*          X       - argument, T(K) .LE. X .LE. T(N+1) */
    /*          INBV    - an initialization parameter which must be set */
    /*                    to 1 the first time BVALU is called. */

    /*         Output */
    /*          INBV    - INBV contains information for efficient process- */
    /*                    ing after the initial call and INBV must not */
    /*                    be changed by the user.  Distinct splines require */
    /*                    distinct INBV parameters. */
    /*          WORK    - work vector of length 3*K. */
    /*          BVALU   - value of the IDERIV-th derivative at X */

    /*     Error Conditions */
    /*         An improper input is a fatal error */
    /* ***REFERENCES  C. DE BOOR, *PACKAGE FOR CALCULATING WITH B-SPLINES*, */
    /*                 SIAM JOURNAL ON NUMERICAL ANALYSIS, VOLUME 14, NO. 3, */
    /*                 JUNE 1977, PP. 441-472. */
    /* ***ROUTINES CALLED  INTRV,XERROR */
    /* ***END PROLOGUE  BVALU */


    /*     DIMENSION T(N+K), WORK(3*K) */
    /* ***FIRST EXECUTABLE STATEMENT  BVALU */
    /* Parameter adjustments */
    --t;
    --a;
    --work;

    /* Function Body */
    ret_val = ( float )0.;
    if ( *k < 1 )
    {
      goto L102;
    }
    if ( *n < *k )
    {
      goto L101;
    }
    if ( *ideriv < 0 || *ideriv >= *k )
    {
      goto L110;
    }
    kmider = *k - *ideriv;

    /* *** FIND *I* IN (K,N) SUCH THAT T(I) .LE. X .LT. T(I+1) */
    /*     (OR, .LE. T(I+1) IF T(I) .LT. T(I+1) = T(N+1)). */
    km1 = *k - 1;
    i__1 = *n + 1;
    intrv_( &t[1], &i__1, x, inbv, &i__, &mflag );
    if ( *x < t[*k] )
    {
      goto L120;
    }
    if ( mflag == 0 )
    {
      goto L20;
    }
    if ( *x > t[i__] )
    {
      goto L130;
    }
L10:
    if ( i__ == *k )
    {
      goto L140;
    }
    --i__;
    if ( *x == t[i__] )
    {
      goto L10;
    }

    /* *** DIFFERENCE THE COEFFICIENTS *IDERIV* TIMES */
    /*     WORK(I) = AJ(I), WORK(K+I) = DP(I), WORK(K+K+I) = DM(I), I=1.K */

L20:
    imk = i__ - *k;
    i__1 = *k;
    for ( j = 1; j <= i__1; ++j )
    {
      imkpj = imk + j;
      work[j] = a[imkpj];
      /* L30: */
    }
    if ( *ideriv == 0 )
    {
      goto L60;
    }
    i__1 = *ideriv;
    for ( j = 1; j <= i__1; ++j )
    {
      kmj = *k - j;
      fkmj = ( double ) kmj;
      i__2 = kmj;
      for ( jj = 1; jj <= i__2; ++jj )
      {
        ihi = i__ + jj;
        ihmkmj = ihi - kmj;
        work[jj] = ( work[jj + 1] - work[jj] ) / ( t[ihi] - t[ihmkmj] ) *
                   fkmj;
        /* L40: */
      }
      /* L50: */
    }

    /* *** COMPUTE VALUE AT *X* IN (T(I),(T(I+1)) OF IDERIV-TH DERIVATIVE, */
    /*     GIVEN ITS RELEVANT B-SPLINE COEFF. IN AJ(1),...,AJ(K-IDERIV). */
L60:
    if ( *ideriv == km1 )
    {
      goto L100;
    }
    ip1 = i__ + 1;
    kpk = *k + *k;
    j1 = *k + 1;
    j2 = kpk + 1;
    i__1 = kmider;
    for ( j = 1; j <= i__1; ++j )
    {
      ipj = i__ + j;
      work[j1] = t[ipj] - *x;
      ip1mj = ip1 - j;
      work[j2] = *x - t[ip1mj];
      ++j1;
      ++j2;
      /* L70: */
    }
    iderp1 = *ideriv + 1;
    i__1 = km1;
    for ( j = iderp1; j <= i__1; ++j )
    {
      kmj = *k - j;
      ilo = kmj;
      i__2 = kmj;
      for ( jj = 1; jj <= i__2; ++jj )
      {
        work[jj] = ( work[jj + 1] * work[kpk + ilo] + work[jj] * work[*k +
                     jj] ) / ( work[kpk + ilo] + work[*k + jj] );
        --ilo;
        /* L80: */
      }
      /* L90: */
    }
L100:
    ret_val = work[1];
    *ErrNo = 0;
    return ret_val;


L101:
    /* ERR      CALL XERROR( ' BVALU,  N DOES NOT SATISFY N.GE.K',34,2,1) */
    *ErrNo = 1;
    return ret_val;
L102:
    /* ERR      CALL XERROR( ' BVALU,  K DOES NOT SATISFY K.GE.1',34,2,1) */
    *ErrNo = 2;
    return ret_val;
L110:
    /* ERR      CALL XERROR( ' BVALU,  IDERIV DOES NOT SATISFY 0.LE.IDERIV.LT.K', */
    /* ERR     1 49, 2, 1) */
    *ErrNo = 3;
    return ret_val;
L120:
    /* ERR      CALL XERROR( ' BVALU,  X IS N0T GREATER THAN OR EQUAL TO T(K)', */
    /* ERR     1 47, 2, 1) */
    *ErrNo = 4;
    return ret_val;
L130:
    /* ERR      CALL XERROR( ' BVALU,  X IS NOT LESS THAN OR EQUAL TO T(N+1)', */
    /* ERR     1 46, 2, 1) */
    *ErrNo = 5;
    return ret_val;
L140:
    /* ERR      CALL XERROR( ' BVALU,  A LEFT LIMITING VALUE CANN0T BE OBTAINED AT */
    /* ERR     1 T(K)',     57, 2, 1) */
    *ErrNo = 6;
    return ret_val;
  } /* bvalu_ */

#ifdef __cplusplus
}
#endif
