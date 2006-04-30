/////////////////////////////////////////////////////////////////////////////
// Name:        sum.h
// Purpose:     shape for FGSummer
// Author:      Matthew Gong
// Created:     02/05/2005
// Copyright:   (c) Matthew Gong
// Licence:     GPL licence
/////////////////////////////////////////////////////////////////////////////


#ifndef _FCSBUILDER_SUM_H_
#define _FCSBUILDER_SUM_H_

#if defined(__GNUG__) && !defined(__APPLE__)
// #pragma interface
#endif

/*
 * FGSummer Componment
 */

class Summer: public ComponentShape
{
  DECLARE_DYNAMIC_CLASS (Summer)

protected:
  double bias;

public:
  Summer (double w = 0.0, double h = 0.0, const wxString & Name = wxT("sum"));
  void OnDraw (wxDC & dc);
  
#if wxUSE_PROLOGIO
  void WriteAttributes (wxExpr * clause);
  void ReadAttributes (wxExpr * clause);
#endif
  virtual void ExportXML(wxTextOutputStream & stream, const wxString & prefix);

  virtual wxArrayString ImportXML(JSBSim::Element * el);

  // Does the copying for this object
  void Copy (wxShape & copy);

  inline double GetBias() const
    {
      return bias;
    }
  inline void SetBias(const double & d)
    {
      bias = d;
    }
};

#endif // _FCSBUILDER_SUM_H_
