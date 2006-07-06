/////////////////////////////////////////////////////////////////////////////
// Name:        pid.h
// Purpose:     shape for FGPID
// Author:      Jon Berndt
// Created:     07/02/2006
// Copyright:   (c) Jon Berndt
// Licence:     GPL licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _FCSBUILDER_PID_H_
#define _FCSBUILDER_PID_H_

#if defined(__GNUG__) && !defined(__APPLE__)
// #pragma interface
#endif

/*
 * FGPID Componment
 */

class PID: public ComponentShape
{
  DECLARE_DYNAMIC_CLASS (PID)

protected:
  double Kp;
  double Ki;
  double Kd;
  wxString Trigger;

public:
  PID (double w = 0.0, double h = 0.0, const wxString & Name = wxT("pid"));
  void OnDraw (wxDC & dc);

#if wxUSE_PROLOGIO
  void WriteAttributes (wxExpr * clause);
  void ReadAttributes (wxExpr * clause);
#endif
  virtual void ExportXML(wxTextOutputStream & stream, const wxString & prefix);

  virtual wxArrayString ImportXML(JSBSim::Element * el);

  // Does the copying for this object
  void Copy (wxShape & copy);

  inline double GetKp(void) const {return Kp;}
  inline void SetKp(const double & d) {Kp = d;}
  inline double GetKi(void) const {return Ki;}
  inline void SetKi(const double & d) {Ki = d;}
  inline double GetKd(void) const {return Kd;}
  inline void SetKd(const double & d) {Kd = d;}
  inline wxString GetTrigger(void) const {return Trigger;}
  inline void SetTrigger(wxString str) {Trigger = str;}
};

#endif // _FCSBUILDER_PID_H_
