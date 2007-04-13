/////////////////////////////////////////////////////////////////////////////
// Name:        actuator.h
// Purpose:     shape for FGActuator
// Author:      Jon Berndt
// Created:     07/18/2006
// Copyright:   (c) Jon Berndt
// Licence:     GPL licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _FCSBUILDER_ACTUATOR_H_
#define _FCSBUILDER_ACTUATOR_H_

#if defined(__GNUG__) && !defined(__APPLE__)
// #pragma interface
#endif

/*
 * FGActuator Componment
 */

class Actuator: public ComponentShape
{
  DECLARE_DYNAMIC_CLASS (Actuator)

protected:
  double Lag;
  double Bias;
  double Rate_Limit;
  double Hysteresis_Width;
//  double Deadband_Width;
  bool InputIsInverted;

public:
  Actuator (double w = 0.0, double h = 0.0, const wxString & Name = wxT("actuator"));
  void OnDraw (wxDC & dc);

#if wxUSE_PROLOGIO
  void WriteAttributes (wxExpr * clause);
  void ReadAttributes (wxExpr * clause);
#endif
  virtual void ExportXML(wxTextOutputStream & stream, const wxString & prefix);

  virtual wxArrayString ImportXML(JSBSim::Element * el);

  // Does the copying for this object
  void Copy (wxShape & copy);

  double GetLag(void) const {return Lag;}
  double GetBias(void) const {return Bias;}
  double GetRate_Limit(void) const {return Rate_Limit;}
  double GetHysteresis_Width(void) const {return Hysteresis_Width;}
//  double GetDeadband_Width(void) const {return Deadband_Width;}

  void SetLag(const double n) {Lag = n;}
  void SetBias(const double n) {Bias = n;}
  void SetRate_Limit(const double n) {Rate_Limit = n;}
  void SetHysteresis_Width(const double n) {Hysteresis_Width = n;}
//  void SetDeadband_Width(const double & n) {Deadband_Width = n;}

  inline bool GetInputIsInverted(void) {return InputIsInverted;}
  inline void SetInputIsInverted(bool s) {InputIsInverted = s;}
};

#endif // _FCSBUILDER_ACTUATOR_H_
