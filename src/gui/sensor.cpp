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
  :ComponentShape(w, h, wxT("sensor"), Name),
   Noise(0.0)
{
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

//  if (Noise != 0.0) stream << Pre << wxT("<noise> ") << Noise << wxT(" </noise>") << endl;

  ExportCliper(stream,Pre);
  ExportOutput(stream,Pre);

  ExportTail(stream, prefix);
}

/**
* ImportXML ====================================================================
*/

wxArrayString Sensor::ImportXML(JSBSim::Element * el)
{
  wxArrayString strings = ComponentShape::ImportXML(el);

//  if ( el->FindElement("noise") ) Noise = el->FindElementValueAsNumber("noise");

  InputIsInverted = *((GetInputSignList().Item(0))->GetData());

  return strings;
}
