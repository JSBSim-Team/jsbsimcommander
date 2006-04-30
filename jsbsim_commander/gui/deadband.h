/////////////////////////////////////////////////////////////////////////////
// Name:        deadband.h
// Purpose:     shape for FGDeadBand
// Author:      Matthew Gong
// Created:     02/05/2005
// Copyright:   (c) Matthew Gong
// Licence:     GPL licence
/////////////////////////////////////////////////////////////////////////////


#ifndef _FCSBUILDER_DEADBAND_H_
#define _FCSBUILDER_DEADBAND_H_

#if defined(__GNUG__) && !defined(__APPLE__)
// #pragma interface
#endif

/*
 * FGFilter Componment
 */

class DeadBand: public ComponentShape
{
  DECLARE_DYNAMIC_CLASS (DeadBand)

protected:
  double deadbandwidth;
  double gain;

public:
  DeadBand (double w = 0.0, double h = 0.0,  const wxString & Name = wxT("deadband"));
  virtual ~DeadBand();
  void OnDraw (wxDC & dc);
  
#if wxUSE_PROLOGIO
  void WriteAttributes (wxExpr * clause);
  void ReadAttributes (wxExpr * clause);
#endif
  virtual void ExportXML(wxTextOutputStream & stream, const wxString & prefix);

  virtual wxArrayString ImportXML(JSBSim::Element * el);

  // Does the copying for this object
  void Copy (wxShape & copy);

  inline double GetDeadBandWidth() const
    {
      return deadbandwidth;
    }
  inline void SetDeadBandWidth(const double &d)
    {
      deadbandwidth = d;
    }
  inline double GetGain() const
    {
      return gain;
    }
  inline void SetGain(const double &d)
    {
      gain = d;
    }
};

#endif // _FCSBUILDER_DEADBAND_H_

