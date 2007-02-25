/////////////////////////////////////////////////////////////////////////////
// Name:        sensor.cpp
// Purpose:     support FGSensor in FCSBuilder
// Author:      Jon Berndt
// Created:     06/02/2006
// Copyright:   (c) Jon Berndt
// Licence:     GPL licence
//
// Functions:
//
// Sensor - Constructor
// OnDraw
// Copy
// ExportXML
// ImportXML
//
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "wx/deprecated/setup.h"

#if wxUSE_PROLOGIO
#include "wx/deprecated/wxexpr.h"
#endif

#include "wx/ogl/ogl.h"

#include "FGXMLElement.h"
#include "MyApp.h"

#include "shape.h"
#include "sensor.h"


IMPLEMENT_DYNAMIC_CLASS (Sensor, ComponentShape)

/**
* Sensor Constructor ===========================================================
*/

Sensor::Sensor (double w, double h, const wxString & Name )
  :ComponentShape(w, h, wxT("sensor"), Name)
{
  noise_variance = 0;
  NoiseType = 0;
  bits = 0;
  quantized = 0;
  quant_property = "";
  max=0;
  min=0;
  bias=0;
  drift_rate=0;
  lag=0;

  SetAttachmentMode (ATTACHMENT_MODE_EDGE);

  ClearAttachments ();
  GetAttachments ().Append (new wxAttachmentPoint (0, w * 0.5, 0.0));
  GetAttachments ().Append (new wxAttachmentPoint (1, -w * 0.5, 0.0));

//  input_sign_list.Append(new bool(false));
  InputIsInverted = false;
}


/**
* OnDraw =======================================================================
*/

void Sensor::OnDraw (wxDC & dc)
{
  ComponentShape::OnDraw (dc);
  wxCoord  h,w;
  h = (int)m_height/2-arr_size - 2;
  w = (int)m_width/2 - 2;
  wxCoord  r = h<w?h:w;
  r = r<10?r:10;

  wxPoint c[2];

  c[0].y =  0;
  c[0].x =  3-w;
  c[1].y =  0;
  c[1].x =  3+w;
  dc.DrawLines (2, c, WXROUND (m_xpos), WXROUND (m_ypos));

 // left vertical
  c[0].y = -4*h/5;
  c[0].x = -3*w/5;
  c[1].y =  4*h/5;
  c[1].x = -3*w/5;
  dc.DrawLines (2, c, WXROUND (m_xpos), WXROUND (m_ypos));

  // top horizontal
   c[0].y = -4*h/5;
   c[0].x = -3*w/5;
   c[1].y = -4*h/5;
   c[1].x =  3*w/5;
   dc.DrawLines (2, c, WXROUND (m_xpos), WXROUND (m_ypos));

  // bottom horizontal
   c[0].y =  4*h/5;
   c[0].x = -3*w/5;
   c[1].y =  4*h/5;
   c[1].x =  3*w/5+1;
   dc.DrawLines (2, c, WXROUND (m_xpos), WXROUND (m_ypos));

  // right vertical
  c[0].y = -4*h/5;
  c[0].x =  3*w/5;
  c[1].y =  4*h/5;
  c[1].x =  3*w/5;
  dc.DrawLines (2, c, WXROUND (m_xpos), WXROUND (m_ypos));

  dc.SetPen(*wxBLACK_PEN);
  dc.SetBrush(*wxRED_BRUSH);
  dc.DrawRoundedRectangle(WXROUND (m_xpos)-w/5-1, WXROUND (m_ypos)-h,        2*w/5+2, 6*h/10, -0.2);
  dc.SetBrush(*wxGREEN_BRUSH);
  dc.DrawRoundedRectangle(WXROUND (m_xpos)-w/5-1, WXROUND (m_ypos)-3*h/10+1, 2*w/5+2,   6*h/10, -0.2);
  dc.SetBrush(*wxBLUE_BRUSH);
  dc.DrawRoundedRectangle(WXROUND (m_xpos)-w/5-1, WXROUND (m_ypos)+4*h/10+2, 2*w/5+2,   6*h/10, -0.2);

}

/**
*        =======================================================================
*/

#if wxUSE_PROLOGIO
void
Sensor::WriteAttributes (wxExpr * clause)
{
  ComponentShape::WriteAttributes (clause);
}


void
Sensor::ReadAttributes (wxExpr * clause)
{
  ComponentShape::ReadAttributes (clause);
}
#endif

/**
* Copy =========================================================================
*/

void Sensor::Copy (wxShape & copy)
{
  ComponentShape::Copy (copy);

  wxASSERT (copy.IsKindOf (CLASSINFO (Sensor)));

  Sensor & SensorCopy = (Sensor &) copy;
}

/**
* ExportXML ====================================================================
*/

void Sensor::ExportXML(wxTextOutputStream & stream, const wxString & prefix)
{
  ExportHead(stream, prefix);

  wxString Pre = prefix + wxT("  ");

  ExportInputs(stream,Pre);

  if (noise_variance != 0.0) {
    if (NoiseType == 0) {
      stream << Pre << wxT("<noise variation=\"PERCENT\"> ") << noise_variance << wxT(" </noise>") << endl;
    } else {
      stream << Pre << wxT("<noise variation=\"ABSOLUTE\"> ") << noise_variance << wxT(" </noise>") << endl;
    }
  }

  if (!quant_property.empty() !! bits != 0) {
  }

  if (drift_rate != 0.0) {
    stream << Pre << wxT("<drift_rate> ") << drift_rate << wxT(" </drift_rate>") << endl;
  }

  if (bias != 0.0) {
    stream << Pre << wxT("<bias> ") << bias << wxT(" </bias>") << endl;
  }

  if (lag != 0.0) {
    stream << Pre << wxT("<lag> ") << lag << wxT(" </lag>") << endl;
  }

  ExportCliper(stream,Pre);
  ExportOutput(stream,Pre);

  ExportTail(stream, prefix);
}

/**
* ImportXML ====================================================================

<sensor name=”name” rate_group=”name”>
  <input> property </input>
  <lag> number </lag>
  <noise variation=”PERCENT|ABSOLUTE”> number </noise>
  <quantization name="name">
    <bits> number </bits>
    <min> number </min>
    <max> number </max>
  </quantization>
  <drift_rate> number </drift_rate>
  <bias> number </bias>
</sensor>
*/

wxArrayString Sensor::ImportXML(JSBSim::Element * element)
{
  JSBSim::Element* scratch_element=0;
  wxArrayString strings = ComponentShape::ImportXML(element);

  JSBSim::Element* quantization_element = element->FindElement("quantization");
  if ( quantization_element) {
    quant_property = quantization_element->GetAttributeValue("name");
    if ( quantization_element->FindElement("bits") ) {
      bits = (int)quantization_element->FindElementValueAsNumber("bits");
    }
//    divisions = (1<<bits);
    if ( quantization_element->FindElement("min") ) {
      min = quantization_element->FindElementValueAsNumber("min");
    }
    if ( quantization_element->FindElement("max") ) {
      max = quantization_element->FindElementValueAsNumber("max");
    }
 //   span = max - min;
 //   granularity = span/divisions;
  }
  if ( element->FindElement("bias") ) {
    bias = element->FindElementValueAsNumber("bias");
  }
  if ( element->FindElement("drift_rate") ) {
    drift_rate = element->FindElementValueAsNumber("drift_rate");
  }
  if ( element->FindElement("lag") ) {
    lag = element->FindElementValueAsNumber("lag");
 //   denom = 2.00 + dt*lag;
 //   ca = dt*lag / denom;
 //   cb = (2.00 - dt*lag) / denom;
  }
  if ( element->FindElement("noise") ) {
    noise_variance = element->FindElementValueAsNumber("noise");
    string variation = element->FindElement("noise")->GetAttributeValue("variation");
    if (variation == "PERCENT") {
      NoiseType = 0; //ePercent;
    } else if (variation == "ABSOLUTE") {
      NoiseType = 1; //eAbsolute;
    } else {
      NoiseType = 0;
      cerr << "Unknown noise type in sensor: " << variation << endl;
      cerr << "  defaulting to PERCENT." << endl;
    }
  }

  InputIsInverted = GetInputIsInverted();

  return strings;
}
