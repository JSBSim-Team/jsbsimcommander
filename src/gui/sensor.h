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

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "shape.h"

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
  inline double GetLag(void) const {return lag;}
  inline double GetDriftRate(void) const {return drift_rate;}
  inline double GetBias(void) const {return bias;}
  inline double GetMax(void) const {return max;}
  inline double GetMin(void) const {return min;}
  inline int GetNoiseType(void) const {return NoiseType;}
  inline int GetBits(void) const {return bits;}
  inline int GetQuantized(void) const {return quantized;}
  inline wxString GetQuantProperty(void) const {return quant_property;}

  inline void SetNoise(const double & n) {noise_variance = n;}
  inline void SetLag(double t) {lag = t;}
  inline void SetDriftRate(double t) {drift_rate = t;}
  inline void SetBias(double t) {bias = t;}
  inline void SetMax(double t) {max = t;}
  inline void SetMin(double t) {min = t;}
  inline void SetNoiseType(int t) {NoiseType = t;}
  inline void SetBits(int t) {bits = t;}
  inline void SetQuantized(int t) {quantized = t;}
  inline void SetQuantProperty(wxString t) {quant_property = t;}

  inline bool GetInputIsInverted(void) {return InputIsInverted;}
  inline void SetInputIsInverted(bool s) {InputIsInverted = s;}
};

#endif // _FCSBUILDER_SENSOR_H_
