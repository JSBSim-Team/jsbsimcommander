/////////////////////////////////////////////////////////////////////////////
// Name:        sum.cpp
// Purpose:     support FGSummer in FCSBuilder
// Author:      Matthew Gong
// Created:     02/05/2005
// Copyright:   (c) Matthew Gong
// Licence:     GPL licence
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

#include "shape.h"
#include "sum.h"


IMPLEMENT_DYNAMIC_CLASS (Summer, ComponentShape)

Summer::Summer (double w, double h, const wxString & Name )
  :ComponentShape(w, h, wxT("SUMMER"), Name),
   bias(0)
{
  SetAttachmentMode (ATTACHMENT_MODE_EDGE);

  ClearAttachments ();
  GetAttachments ().Append (new wxAttachmentPoint (0, w * 0.5, 0.0));
  GetAttachments ().Append (new wxAttachmentPoint (1, -w * 0.5, 15.0));
  GetAttachments ().Append (new wxAttachmentPoint (2, -w * 0.5, -15.0));

  input_sign_list.Append(new bool(false));
  input_sign_list.Append(new bool(false));
}


void
Summer::OnDraw (wxDC & dc)
{
  ComponentShape::OnDraw (dc);
  wxCoord  h,w;
  h = (int)m_height/2-arr_size - 2;
  w = (int)m_width/2 - 2;
  wxCoord  r = h<w?h:w;
  r = r<10?r:10;
  dc.DrawCircle(WXROUND (m_xpos), WXROUND (m_ypos), r);
  wxPoint c[2];
  c[0].y = 0;
  c[0].x = r;
  c[1].y = c[0].y;
  c[1].x = -c[0].x;
  dc.DrawLines (2, c, WXROUND (m_xpos), WXROUND (m_ypos));
  c[0].y = r;
  c[0].x = 0;
  c[1].y = -c[0].y;
  c[1].x = c[0].x;
  dc.DrawLines (2, c, WXROUND (m_xpos), WXROUND (m_ypos));
}

#if wxUSE_PROLOGIO
void 
Summer::WriteAttributes (wxExpr * clause)
{
  ComponentShape::WriteAttributes (clause);

  clause->AddAttributeValue (wxT ("bias"), bias);
}


void 
Summer::ReadAttributes (wxExpr * clause)
{
  ComponentShape::ReadAttributes (clause);

  clause->GetAttributeValue(wxT ("bias"), bias);
}
#endif

void 
Summer::Copy (wxShape & copy)
{
  ComponentShape::Copy (copy);

  wxASSERT (copy.IsKindOf (CLASSINFO (Summer)));

  Summer & SummerCopy = (Summer &) copy;
  SummerCopy.bias = bias;
}


void 
Summer::ExportXML(wxTextOutputStream & stream, const wxString & prefix)
{
  ExportHead(stream, prefix);

  wxString Pre = prefix + wxT("  ");
  
  ExportInputs(stream,Pre);
  
  if (bias != 0)
    stream << Pre << wxT("<bias> ")<< wxString::Format(wxT("%g"), bias) << wxT(" </bias>") << endl;

  ExportCliper(stream,Pre);
  ExportOutput(stream,Pre);
  
  ExportTail(stream, prefix);
}


wxArrayString
Summer::ImportXML(JSBSim::Element * el)
{
  wxArrayString strings = ComponentShape::ImportXML(el);

  if (el->FindElement("bias")) bias = el->FindElementValueAsNumber("bias");

  return strings;
}


