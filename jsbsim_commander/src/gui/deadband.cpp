/////////////////////////////////////////////////////////////////////////////
// Name:        deadband.cpp
// Purpose:     support FGDeadBand in FCSBuilder
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

#include <wx/txtstrm.h>

#include "wx/deprecated/setup.h"

#if wxUSE_PROLOGIO
#include "wx/deprecated/wxexpr.h"
#endif

#include "wx/ogl/ogl.h"
#include "FGXMLParse.h"

#include "shape.h"
#include "deadband.h"


IMPLEMENT_DYNAMIC_CLASS (DeadBand, ComponentShape)

DeadBand::DeadBand (double w, double h, const wxString & Name )
  :ComponentShape(w, h, wxT("DEADBAND"), Name),
   deadbandwidth(0),
   gain(1)
{
  SetAttachmentMode (ATTACHMENT_MODE_EDGE);

  ClearAttachments ();
  GetAttachments ().Append (new wxAttachmentPoint (0, w * 0.5, 0.0));
  GetAttachments ().Append (new wxAttachmentPoint (1, -w * 0.5, 0.0));

  input_sign_list.Append(new bool(false));
}

DeadBand::~DeadBand()
{
}

void DeadBand::OnDraw (wxDC & dc)
{
  ComponentShape::OnDraw(dc);
  wxPoint c[3];
  
  int h,w;
  h = (int)m_height/2;
  w = (int)m_width/2;
  
  c[0].x = -2;
  c[0].y = -h + 6;
  c[1].x = 0;
  c[1].y = -h + 2;
  c[2].x = 2;
  c[2].y = c[0].y;
  dc.DrawLines (3, c, WXROUND (m_xpos), WXROUND (m_ypos));
  c[0].x = 0;
  c[0].y = -h + 2;
  c[1].x = c[0].x;
  c[1].y = -c[0].y;
  dc.DrawLines (2, c, WXROUND (m_xpos), WXROUND (m_ypos));
  
  c[0].y = -2;
  c[0].x = w - 10;
  c[1].y = 0;
  c[1].x = w - 6;
  c[2].y = 2;
  c[2].x = c[0].x;
  dc.DrawLines (3, c, WXROUND (m_xpos), WXROUND (m_ypos));
  c[0].y = 0;
  c[0].x = w - 6;
  c[1].y = c[0].y;
  c[1].x = -c[0].x;
  dc.DrawLines (2, c, WXROUND (m_xpos), WXROUND (m_ypos));


  c[0].x = 2;
  c[0].y = 0;
  c[1].y = (h > w - c[0].x)?-w + c[0].x + arr_size: -h + arr_size; 
  c[1].x = -c[1].y + c[0].x; 
  dc.DrawLines (2, c, WXROUND (m_xpos), WXROUND (m_ypos));
  c[0].x = -c[0].x;
  c[0].y = -c[0].y;
  c[1].x = -c[1].x;
  c[1].y = -c[1].y;
  dc.DrawLines (2, c, WXROUND (m_xpos), WXROUND (m_ypos));
}

#if wxUSE_PROLOGIO
void 
DeadBand::WriteAttributes (wxExpr * clause)
{
  ComponentShape::WriteAttributes (clause);

  clause->AddAttributeValue (wxT ("deadbandwidth"), deadbandwidth);
  clause->AddAttributeValue (wxT ("gain"), gain);
}


void 
DeadBand::ReadAttributes (wxExpr * clause)
{
  ComponentShape::ReadAttributes (clause);

  clause->GetAttributeValue(wxT ("deadbandwidth"), deadbandwidth);
  clause->GetAttributeValue(wxT ("gain"), gain);
}
#endif


void 
DeadBand::Copy (wxShape & copy)
{
  ComponentShape::Copy (copy);

  wxASSERT (copy.IsKindOf (CLASSINFO (DeadBand)));

  DeadBand & DeadBandCopy = (DeadBand &) copy;
  DeadBandCopy.deadbandwidth = deadbandwidth;
  DeadBandCopy.gain = deadbandwidth;
}

void 
DeadBand::ExportXML(wxTextOutputStream & stream, const wxString & prefix)
{
  ExportHead(stream, prefix);

  wxString Pre = prefix + wxT("  ");
  
  ExportInputs(stream,Pre);

  if (deadbandwidth != 0)
    stream << Pre << wxT("<width> ")<< wxString::Format(wxT("%g"), deadbandwidth) << wxT(" </width>") << endl;
  if (gain != 1)
    stream << Pre << wxT("<gain> ")<< wxString::Format(wxT("%g"), gain) << wxT(" </gain>") << endl;

  ExportCliper(stream,Pre);
  ExportOutput(stream,Pre);
  
  ExportTail(stream, prefix);
}

wxArrayString 
DeadBand::ImportXML(JSBSim::Element * el)
{
  wxArrayString strings = ComponentShape::ImportXML(el);

  if (el->FindElement("width")) {
    deadbandwidth = el->FindElementValueAsNumber("width");
  }

  if (el->FindElement("gain")) {
    gain = el->FindElementValueAsNumber("gain");
  }

  return strings;
}


