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
  
/*
  enum eNoiseType {ePercent=0, eAbsolute} NoiseType;
  double dt;
  double min, max;
  double span;
  double bias;
  double drift_rate;
  double drift;
  double noise_variance;
  double lag;
  double granularity;
  double ca; /// lag filter coefficient "a"
  double cb; /// lag filter coefficient "b"
  double PreviousOutput;
  double PreviousInput;
  int noise_type;
  int bits;
  int quantized;
  int divisions;
  bool fail_low;
  bool fail_high;
  bool fail_stuck;

  void Noise(void);
  void Bias(void);
  void Drift(void);
  void Quantize(void);
  void Lag(void);
  */

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
