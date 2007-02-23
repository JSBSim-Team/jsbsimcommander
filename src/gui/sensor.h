/////////////////////////////////////////////////////////////////////////////
// Name:        sensor.h
// Purpose:     shape for FGSensor
// Author:      Jon Berndt
// Created:     07/18/2006
// Copyright:   (c) Jon Berndt
// Licence:     GPL licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _FCSBUILDER_SENSOR_H_
#define _FCSBUILDER_SENSOR_H_

#if defined(__GNUG__) && !defined(__APPLE__)
// #pragma interface
#endif

/*
 * FGSensor Componment
 */

class Sensor: public ComponentShape
{
  DECLARE_DYNAMIC_CLASS (Sensor)

protected:
  double noise_variance;
  int NoiseType;
  int bits;
  int quantized;
  wxString quant_property;
  double max, min;
  double bias;
  double drift_rate;
  double lag;
  bool InputIsInverted;
  
public:
  Sensor (double w = 0.0, double h = 0.0, const wxString & Name = wxT("sensor"));
  void OnDraw (wxDC & dc);

#if wxUSE_PROLOGIO
  void WriteAttributes (wxExpr * clause);
  void ReadAttributes (wxExpr * clause);
#endif
  virtual void ExportXML(wxTextOutputStream & stream, const wxString & prefix);

  virtual wxArrayString ImportXML(JSBSim::Element * el);

  // Does the copying for this object
  void Copy (wxShape & copy);

  inline double GetNoise(void) const {return noise_variance;}
  inline void SetNoise(const double & n) {noise_variance = n;}

  inline bool GetInputIsInverted(void) {return InputIsInverted;}
  inline void SetInputIsInverted(bool s) {InputIsInverted = s;}
};

#endif // _FCSBUILDER_SENSOR_H_
