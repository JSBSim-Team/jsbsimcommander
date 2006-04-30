/////////////////////////////////////////////////////////////////////////////
// Name:        filter.cpp
// Purpose:     support FGFilter in FCSBuilder
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
#include "filter.h"


IMPLEMENT_DYNAMIC_CLASS (Filter, ComponentShape)

Filter::Filter (double w, double h, const wxString & Type, const wxString & Name )
  :ComponentShape(w, h, Type, Name),
   c1(wxEmptyString),
   c2(wxEmptyString),
   c3(wxEmptyString),
   c4(wxEmptyString),
   c5(wxEmptyString),
   c6(wxEmptyString)
{
  SetAttachmentMode (ATTACHMENT_MODE_EDGE);

  ClearAttachments ();
  GetAttachments ().Append (new wxAttachmentPoint (0, w * 0.5, 0.0));
  GetAttachments ().Append (new wxAttachmentPoint (1, -w * 0.5, 0.0));

  input_sign_list.Append(new bool(false));
}

Filter::~Filter()
{
}

void
Filter::OnDraw (wxDC & dc)
{
  ComponentShape::OnDraw (dc);
  wxPoint c[2];
  int h,w;
  h = (int)m_height/2;
  w = (int)m_width/2;
  c[0].y = 0;
  c[0].x = w - 6;
  c[1].y = c[0].y;
  c[1].x = -c[0].x;
  dc.DrawLines (2, c, WXROUND (m_xpos), WXROUND (m_ypos));
  wxString str1, str2;
  if (type == wxT("LAG_FILTER"))
  {
    str1 = wxString::Format(wxT("%s"), c1.c_str());
    if (c1.IsEmpty() || c1[0] == wxT('-'))
    {
      str2 = wxString::Format(wxT("S%s"), c1.c_str());
    }
    else
    {
      str2 = wxString::Format(wxT("S+%s"), c1.c_str());    
    }
  }
  else if (type == wxT("LEAD_LAG_FILTER"))
  {
    if (c2.IsEmpty() || c2[0] == wxT('-'))
    {
      str1 = wxString::Format(wxT("%sS%s"), c1.c_str(), c2.c_str());
    }
    else
    {
      str1 = wxString::Format(wxT("%sS+%s"), c1.c_str(), c2.c_str());    
    }
    if (c4.IsEmpty() || c4[0] == wxT('-'))
    {
      str2 = wxString::Format(wxT("%sS%s"), c3.c_str(), c4.c_str());
    }
    else
    {
      str2 = wxString::Format(wxT("%sS+%s"), c3.c_str(), c4.c_str());    
    }
  }
  else if (type == wxT("SECOND_ORDER_FILTER"))
  {
    if (c2.IsEmpty() || c2[0] == wxT('-'))
    {
      if (c3.IsEmpty() || c3[0] == wxT('-'))
      {
        str1 = wxString::Format(wxT("%sSS%sS%s"), c1.c_str(), c2.c_str(), c3.c_str());
      }
      else
      {
        str1 = wxString::Format(wxT("%sSS%sS+%s"), c1.c_str(), c2.c_str(), c3.c_str());
      }
    }
    else
    {
      if (c3.IsEmpty() || c3[0] == wxT('-'))
      {
        str1 = wxString::Format(wxT("%sSS+%sS%s"), c1.c_str(), c2.c_str(), c3.c_str());
      }
      else
      {
        str1 = wxString::Format(wxT("%sSS+%sS+%s"), c1.c_str(), c2.c_str(), c3.c_str());
      }
    }
    if (c5.IsEmpty() || c5[0] == wxT('-'))
    {
      if (c6.IsEmpty() || c6[0] == wxT('-'))
      {
        str2 = wxString::Format(wxT("%sSS%sS%s"), c4.c_str(), c5.c_str(), c6.c_str());
      }
      else
      {
        str2 = wxString::Format(wxT("%sSS%sS+%s"), c4.c_str(), c5.c_str(), c6.c_str());
      }
    }
    else
    {
      if (c6.IsEmpty() || c6[0] == wxT('-'))
      {
        str2 = wxString::Format(wxT("%sSS+%sS%s"), c4.c_str(), c5.c_str(), c6.c_str());
      }
      else
      {
        str2 = wxString::Format(wxT("%sSS+%sS+%s"), c4.c_str(), c5.c_str(), c6.c_str());
      }
    }  
  }
  else if (type == wxT("WASHOUT_FILTER"))
  {
    str1 = wxT("S");
    if (c1.IsEmpty() || c1[0] == wxT('-'))
    {
      str2 = wxString(wxT("S%s"), c1.c_str());
    }
    else
    {
      str2 = wxString::Format(wxT("S+%s"), c1.c_str());    
    }
  }
  else if (type == wxT("INTEGRATOR"))
  {
    str1 = c1;
    str2 = wxT("S");
  }
  wxCoord w1,h1,w2,h2;
  dc.GetTextExtent(str1,&w1,&h1);
  dc.GetTextExtent(str2,&w2,&h2);
  w = w1>=w2?w1:w2;
  h = h1+h2+4;
  dc.DrawText(str1,WXROUND (GetX()-w1*0.5),WXROUND (GetY()-h1-2));
  dc.DrawText(str2,WXROUND (GetX()-w2*0.5),WXROUND (GetY()+2));      
  bool flag = false;
  if (w+12>m_width)
    {
      m_width = w+12;
      flag = true;
    }
  if (h+4>m_height)
    {
      m_height = h+4;
      flag = true;
    }
  if (flag)
    {
      SetSize(m_width, m_height);
      Select(false);
    }
}

#if wxUSE_PROLOGIO
void 
Filter::WriteAttributes (wxExpr * clause)
{
  ComponentShape::WriteAttributes (clause);

  clause->AddAttributeValueString (wxT ("c1"), c1);
  clause->AddAttributeValueString (wxT ("c2"), c2);
  clause->AddAttributeValueString (wxT ("c3"), c3);
  clause->AddAttributeValueString (wxT ("c4"), c4);
  clause->AddAttributeValueString (wxT ("c5"), c5);
  clause->AddAttributeValueString (wxT ("c6"), c6);

  clause->AddAttributeValueString (wxT ("trigger"), trigger);
}


void 
Filter::ReadAttributes (wxExpr * clause)
{
  ComponentShape::ReadAttributes (clause);

  double tmp;
  clause->GetAttributeValue(wxT ("c1"), c1);
  if (c1.IsEmpty())
  {
    clause->GetAttributeValue(wxT ("c1"), tmp);
    c1 = wxString::Format(wxT("%g"), tmp);
  }
  clause->GetAttributeValue(wxT ("c2"), c2);
  if (c2.IsEmpty())
  {
    clause->GetAttributeValue(wxT ("c2"), tmp);
    c2 = wxString::Format(wxT("%g"), tmp);
  }
  clause->GetAttributeValue(wxT ("c3"), c3);
  if (c3.IsEmpty())
  {
    clause->GetAttributeValue(wxT ("c3"), tmp);
    c3 = wxString::Format(wxT("%g"), tmp);
  }
  clause->GetAttributeValue(wxT ("c4"), c4);
  if (c4.IsEmpty())
  {
    clause->GetAttributeValue(wxT ("c4"), tmp);
    c4 = wxString::Format(wxT("%g"), tmp);
  }
  clause->GetAttributeValue(wxT ("c5"), c5);
  if (c5.IsEmpty())
  {
    clause->GetAttributeValue(wxT ("c5"), tmp);
    c5 = wxString::Format(wxT("%g"), tmp);
  }
  clause->GetAttributeValue(wxT ("c6"), c6);
  if (c6.IsEmpty())
  {
    clause->GetAttributeValue(wxT ("c6"), tmp);
    c6 = wxString::Format(wxT("%g"), tmp);
  }

  clause->GetAttributeValue (wxT ("trigger"), trigger);
}
#endif


void 
Filter::Copy (wxShape & copy)
{
  ComponentShape::Copy (copy);

  wxASSERT (copy.IsKindOf (CLASSINFO (Filter)));

  Filter & FilterCopy = (Filter &) copy;
  FilterCopy.c1 = c1;
  FilterCopy.c2 = c2;
  FilterCopy.c3 = c3;
  FilterCopy.c4 = c4;
  FilterCopy.c5 = c5;
  FilterCopy.c6 = c6;
  FilterCopy.trigger = trigger;
}


void 
Filter::ExportXML(wxTextOutputStream & stream, const wxString & prefix)
{
  ExportHead(stream, prefix);

  wxString Pre = prefix + wxT("  ");
  
  ExportInputs(stream,Pre);

    stream << Pre << wxT("<c1>")<< c1 << wxT("</c1>") << endl;
    stream << Pre << wxT("<c2>")<< c2 << wxT("</c2>") << endl;
    stream << Pre << wxT("<c3>")<< c3 << wxT("</c3>") << endl;
    stream << Pre << wxT("<c4>")<< c4 << wxT("</c4>") << endl;
    stream << Pre << wxT("<c5>")<< c5 << wxT("</c5>") << endl;
    stream << Pre << wxT("<c6>")<< c6 << wxT("</c6>") << endl;

  if (trigger.Length() !=0 )
    stream << Pre << wxT("<trigger>") << mkName( trigger, true ) << wxT("</trigger>") << endl;

  ExportCliper(stream,Pre);
  ExportOutput(stream,Pre);
  
  ExportTail(stream, prefix);
}

wxArrayString
Filter::ImportXML(JSBSim::Element * el)
{
  wxArrayString strings = ComponentShape::ImportXML(el);

  if (el->FindElement("c1")) c1 = el->FindElementValue("c1");
  if (el->FindElement("c2")) c2 = el->FindElementValue("c2");
  if (el->FindElement("c3")) c3 = el->FindElementValue("c3");
  if (el->FindElement("c4")) c4 = el->FindElementValue("c4");
  if (el->FindElement("c5")) c5 = el->FindElementValue("c5");
  if (el->FindElement("c6")) c6 = el->FindElementValue("c6");
  if (el->FindElement("trigger")) {
    trigger =  el->FindElementValue("trigger").c_str();
  }

  return strings;
}



