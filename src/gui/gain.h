/////////////////////////////////////////////////////////////////////////////
// Name:        gain.h
// Purpose:     shape for FGGain
// Author:      Matthew Gong
// Created:     02/05/2005
// Copyright:   (c) Matthew Gong
// Licence:     GPL licence
/////////////////////////////////////////////////////////////////////////////


#ifndef _FCSBUILDER_GAIN_H_
#define _FCSBUILDER_GAIN_H_

#if defined(__GNUG__) && !defined(__APPLE__)
// #pragma interface
#endif

struct MyPair
{
  double x;
  double y;
};

WX_DECLARE_LIST(MyPair,MyPairList);

/*
 * FGGAIN Componment
 */

class Gain: public ComponentShape
{
  DECLARE_DYNAMIC_CLASS (Gain)

protected:
  double gain;
  double asMin, asMax;
  wxString indep;
  MyPairList Table;

public:
  Gain (double w = 0.0, double h = 0.0, const wxString & Name = wxT("gain"));
  virtual ~Gain();
  void OnDraw (wxDC & dc);
  
#if wxUSE_PROLOGIO
  void WriteAttributes (wxExpr * clause);
  void ReadAttributes (wxExpr * clause);
#endif
  virtual void ExportXML(wxTextOutputStream & stream, const wxString & prefix);

  virtual wxArrayString ImportXML(JSBSim::Element * el);

  // Does the copying for this object
  void Copy (wxShape & copy);

  inline double GetGain() const
    {
      return gain;
    }
  inline void SetGain(const double & g)
    {
      gain = g;
    }
  inline double GetAeroSurfaceMax() const
    {
      return asMax;
    }
  inline void SetAeroSurfaceMax(const double & Max)
    {
      asMax = Max;
    }
  inline double GetAeroSurfaceMin() const
    {
      return asMin;
    }
  inline void SetAeroSurfaceMin(const double & Min)
    {
      asMin = Min;
    }
  inline wxString GetIndependVar() const
    {
      return indep;
    }
  inline void SetIndepentVar(const wxString & v)
    {
      indep = v;
    }
  inline MyPairList & GetTable()
    {
      return Table;
    }

};


#endif // _FCSBUILDER_GAIN_H_
