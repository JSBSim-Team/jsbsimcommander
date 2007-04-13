/////////////////////////////////////////////////////////////////////////////
// Name:        actuator.cpp
// Purpose:     support FGActuator in FCSBuilder
// Author:      Jon Berndt
// Created:     06/02/2006
// Copyright:   (c) Jon Berndt
// Licence:     GPL licence
//
// Functions:
//
// Actuator - Constructor
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
#include "actuator.h"


IMPLEMENT_DYNAMIC_CLASS (Actuator, ComponentShape)

/**
* Actuator Constructor ===========================================================
*/

Actuator::Actuator (double w, double h, const wxString & Name )
  :ComponentShape(w, h, wxT("actuator"), Name),
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

void Actuator::OnDraw (wxDC & dc)
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
Actuator::WriteAttributes (wxExpr * clause)
{
  ComponentShape::WriteAttributes (clause);
}


void
Actuator::ReadAttributes (wxExpr * clause)
{
  ComponentShape::ReadAttributes (clause);
}
#endif

/**
* Copy =========================================================================
*/

void Actuator::Copy (wxShape & copy)
{
  ComponentShape::Copy (copy);

  wxASSERT (copy.IsKindOf (CLASSINFO (Actuator)));

  Actuator & ActuatorCopy = (Actuator &) copy;
}

/**
* ExportXML ====================================================================
*/

void Actuator::ExportXML(wxTextOutputStream & stream, const wxString & prefix)
{
  ExportHead(stream, prefix);

  wxString Pre = prefix + wxT("  ");

  ExportInputs(stream,Pre);

  if (Lag != 0.0) stream << Pre << wxT("<lag> ") << Lag << wxT(" </lag>") << endl;
  if (Rate_Limit != 0.0) stream << Pre << wxT("<rate_limit> ") << Rate_Limit << wxT(" </rate_limit>") << endl;
  if (Bias != 0.0) stream << Pre << wxT("<bias> ") << Bias << wxT(" </bias>") << endl;
  if (Hysteresis_Width != 0.0) stream << Pre << wxT("<hysteresis_width> ") << Hysteresis_Width << wxT(" </hysteresis_width>") << endl;

  ExportCliper(stream,Pre);
  ExportOutput(stream,Pre);

  ExportTail(stream, prefix);
}

/**
* ImportXML ====================================================================

<actuator name=”name”>
  <input> {[-]property} </input>
  <lag> number </lag>
  <rate_limit> number <rate_limit>
  <bias> number </bias>
  <deadband_width> number </deadband_width> // not modeled now
  <hysteresis_width> number </hysteresis_width>
  [<clipto>
    <min> {property name | value} </min>
    <max> {property name | value} </max>
   </clipto>]
  [<output> {property} </output>]
</actuator>

*/

wxArrayString Actuator::ImportXML(JSBSim::Element * el)
{
  JSBSim::Element* scratch_element=0;
  wxArrayString strings = ComponentShape::ImportXML(el);

  if ( el->FindElement("lag") ) Lag = el->FindElementValueAsNumber("lag");
  if ( el->FindElement("rate_limit")) Rate_Limit = el->FindElementValueAsNumber("rate_limit");
  if ( el->FindElement("bias")) Bias = el->FindElementValueAsNumber("bias");
  if ( el->FindElement("hysteresis_width")) Hysteresis_Width = el->FindElementValueAsNumber("hysteresis_width");

  InputIsInverted = *((GetInputSignList().Item(0))->GetData());

  return strings;
}
