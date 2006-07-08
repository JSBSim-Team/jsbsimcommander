/////////////////////////////////////////////////////////////////////////////
// Name:        gain.cpp
// Purpose:     support FGGain in FCSBuilder
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
#include "gain.h"
#include "MyApp.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(MyPairList);

IMPLEMENT_DYNAMIC_CLASS (Gain, ComponentShape)

Gain::Gain (double w, double h, const wxString & Name )
  :ComponentShape(w, h, wxT("pure_gain"), Name),
   gain(1),
   asDomainMin(-1),
   asDomainMax(1),
   asMin(0),
   asMax(0),
   zero_centered(true)
{
  Table.DeleteContents(true);

  SetAttachmentMode (ATTACHMENT_MODE_EDGE);

  ClearAttachments ();
  GetAttachments ().Append (new wxAttachmentPoint (0, w * 0.5, 0.0));
  GetAttachments ().Append (new wxAttachmentPoint (1, -w * 0.5, 0.0));

  input_sign_list.Append(new bool(false));

}

inline
Gain::~Gain()
{
  //Table.Clear();
}

void
Gain::OnDraw (wxDC & dc)
{
  ComponentShape::OnDraw (dc);
  if ( type == wxT("pure_gain") )
    {
      wxString tmp = wxString::Format(wxT("%g"), gain);
      wxCoord w,h;
      dc.GetTextExtent(tmp,&w,&h);
      dc.DrawText(tmp,WXROUND (GetX()-w*0.5),WXROUND (GetY()-h*0.5));
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
  else if ( type == wxT("scheduled_gain") )
    {
      wxPoint c[3];
      
      wxCoord  h,w, x0, y0;
      y0 = (wxCoord )m_height/2 - 2*arr_size;
      x0 = -(wxCoord )m_width/2 + 2*arr_size;
      h = (wxCoord )m_height - 4*arr_size;
      w = (wxCoord )m_width - 4*arr_size;
      
      c[0].x = -2;
      c[0].y = -h + 6;
      c[1].x = 0;
      c[1].y = -h + 2;
      c[2].x = 2;
      c[2].y = c[0].y;
      dc.DrawLines (3, c, WXROUND (m_xpos + x0), WXROUND (m_ypos + y0));
      c[0].x = 0;
      c[0].y = -h + 2;
      c[1].x = c[0].x;
      c[1].y = 0;
      dc.DrawLines (2, c, WXROUND (m_xpos + x0), WXROUND (m_ypos + y0));
      
      c[0].y = -2;
      c[0].x = w - 6;
      c[1].y = 0;
      c[1].x = w - 2;
      c[2].y = 2;
      c[2].x = c[0].x;
      dc.DrawLines (3, c, WXROUND (m_xpos + x0), WXROUND (m_ypos + y0));
      c[0].y = 0;
      c[0].x = w - 2;
      c[1].y = c[0].y;
      c[1].x = 0;
      dc.DrawLines (2, c, WXROUND (m_xpos + x0), WXROUND (m_ypos + y0));
    
      c[0].x = w/6;
      c[0].y = -h*5/6;
      c[1].x = w/3;
      c[1].y = -h/3;
      c[2].x = 5*w/6;
      c[2].y = -h/6;
      dc.DrawLines (3, c, WXROUND (m_xpos + x0), WXROUND (m_ypos + y0));
    }
  else
    {
      ShowType(dc);
    }
}

#if wxUSE_PROLOGIO
void 
Gain::WriteAttributes (wxExpr * clause)
{
  ComponentShape::WriteAttributes (clause);

  clause->AddAttributeValue (wxT ("gain"), gain);

  if ( type == wxT("aerosurface_scale") )
    {
      clause->AddAttributeValue (wxT ("asMax"), asMax);
      clause->AddAttributeValue (wxT ("asMin"), asMin);
      clause->AddAttributeValue (wxT ("asDomainMax"), asDomainMax);
      clause->AddAttributeValue (wxT ("asDomainMin"), asDomainMin);
      long tmp = (long)zero_centered;
      clause->AddAttributeValue (wxT ("zero_centered"), tmp);
    }
  else if ( type == wxT("scheduled_gain") )
    {
      clause->AddAttributeValueString (wxT ("indep"), indep);
      wxExpr *list = new wxExpr(wxExprList);
      wxMyPairListNode * node = Table.GetFirst();
      while (node)
	{
	  MyPair * value = node->GetData();
	  list->Append(new wxExpr(value->x));
	  list->Append(new wxExpr(value->y));
	  node = node->GetNext();
	}
      clause->AddAttributeValue(wxT("Table"), list);
    }
}


void 
Gain::ReadAttributes (wxExpr * clause)
{
  ComponentShape::ReadAttributes (clause);

  clause->GetAttributeValue(wxT ("gain"), gain);

  if ( type == wxT("aerosurface_scale") )
    {
      clause->GetAttributeValue (wxT ("asMax"), asMax);
      clause->GetAttributeValue (wxT ("asMin"), asMin);
      clause->GetAttributeValue (wxT ("asDomainMax"), asDomainMax);
      clause->GetAttributeValue (wxT ("asDomainMin"), asDomainMin);
      long tmp;
      clause->GetAttributeValue (wxT ("zero_centered"), tmp);
      zero_centered = (bool)tmp;
    }
  else if ( type == wxT("scheduled_gain") )
    {
      clause->GetAttributeValue (wxT ("indep"), indep);
      Table.Clear();
      wxExpr *list = clause->AttributeValue(wxT("Table"));
      if (list)
      {
	wxExpr *expr = list->GetFirst();
	while (expr)
	{
	  MyPair * p = new MyPair;
	  p->x = expr->RealValue();
	  expr = expr->GetNext();
	  p->y = expr->RealValue();
	  expr = expr->GetNext();
	  Table.Append(p);
	}  
      }
    }
}
#endif


void 
Gain::Copy (wxShape & copy)
{
  ComponentShape::Copy (copy);

  wxASSERT (copy.IsKindOf (CLASSINFO (Gain)));

  Gain & GainCopy = (Gain &) copy;
  GainCopy.gain = gain;
  GainCopy.asDomainMin = asDomainMin;
  GainCopy.asDomainMax = asDomainMax;
  GainCopy.zero_centered = zero_centered;
  GainCopy.asMin = asMin;
  GainCopy.asMax = asMax;
  GainCopy.indep = indep;

  GainCopy.Table.Clear();
  wxMyPairListNode * node = Table.GetFirst();
  while (node)
    {
      MyPair * p = node->GetData();
      GainCopy.Table.Append(new MyPair(*p));
      node = node->GetNext();
    }
}

void 
Gain::ExportXML(wxTextOutputStream & stream, const wxString & prefix)
{
  ExportHead(stream, prefix);

  wxString Pre = prefix + wxT("  ");
  
  ExportInputs(stream,Pre);

  if (gain != 1 || type == wxT("pure_gain"))
    stream << Pre << wxT("<gain>")<< wxString::Format(wxT("%g"), gain) << wxT("</gain>") << endl;

  if (type == wxT("aerosurface_scale"))
  {
    if (asDomainMin != -1 || asDomainMax != 1)
   {
    stream << Pre << wxT("<domain>")<< endl;
    stream << Pre << wxT("  <min>")<< wxString::Format(wxT("%g"), asDomainMin) << wxT("</min>") << endl;
    stream << Pre << wxT("  <max>")<< wxString::Format(wxT("%g"), asDomainMax) << wxT("</max>") << endl;
    stream << Pre << wxT("</domain>")<< endl;
   }
    stream << Pre << wxT("<range>")<< endl;
    stream << Pre << wxT("  <min>")<< wxString::Format(wxT("%g"), asMin) << wxT("</min>") << endl;
    stream << Pre << wxT("  <max>")<< wxString::Format(wxT("%g"), asMax) << wxT("</max>") << endl;
    stream << Pre << wxT("</range>")<< endl;
    if (!zero_centered)
    {
      stream << Pre << wxT("<zero_centered>false</zero_centered>")<< endl;
    }
  }
  else if ((type == wxT("scheduled_gain")))
  {
    stream << Pre << wxT("<table>")<< endl;
    stream << Pre << wxT("  <independentVar> ") << indep << wxT(" </independentVar>") << endl;
    stream << Pre << wxT("  <tableData>")<< endl;
    wxMyPairListNode * node = Table.GetFirst();
    while (node)
    {
      MyPair * p = node->GetData();
      stream << Pre << wxT("    ") << wxString::Format(wxT("%g"), p->x) << wxT("\t\t") << wxString::Format(wxT("%g"), p->y) << endl;
      node = node->GetNext();
    }
    
    stream << Pre << wxT("  </tableData>")<< endl;
    stream << Pre << wxT("</table>")<< endl;  
  }
  
  ExportCliper(stream,Pre);
  ExportOutput(stream,Pre);
  
  ExportTail(stream, prefix);
}


wxArrayString
Gain::ImportXML(JSBSim::Element * el)
{
  wxArrayString strings = ComponentShape::ImportXML(el);

  if ( el->FindElement("gain") ) {
    gain = el->FindElementValueAsNumber("gain");
  }

  if (type == wxT("pure_gain")) {
    if ( !el->FindElement("gain") ) {
      cerr << "No GAIN supplied for PURE_GAIN component: " << name << endl;
      exit(-1);
    }
  }

  if (type == wxT("aerosurface_scale")) 
  {
    JSBSim::Element * scale_el = el->FindElement("range");
    if (scale_el && scale_el->FindElement("max") && scale_el->FindElement("min") )
    {
      asMax = scale_el->FindElementValueAsNumber("max");
      asMin = scale_el->FindElementValueAsNumber("min");
    } else {
      cerr << "A maximum and minimum scale value must be supplied for the "
              "aerosurface scale component" << endl;
      exit(-1);
    }
    scale_el = el->FindElement("domain");
    if (scale_el && scale_el->FindElement("max") && scale_el->FindElement("min") )
    {
      asDomainMax = scale_el->FindElementValueAsNumber("max");
      asDomainMin = scale_el->FindElementValueAsNumber("min");
    }
    wxString zc = std2wxstr(el->FindElementValue("zero_centered"));
    if (zc == wxT("false"))
    {
      zero_centered = false;
    }
  }

  if (type == wxT("scheduled_gain")) 
  {
    JSBSim::Element * table_el;
    if (table_el = el->FindElement("table")) 
    {
      indep = std2wxstr(table_el->FindElementValue("independentVar"));
      JSBSim::Element * tablevalue_el = table_el->FindElement("tableData");
      Table.Clear();
      for ( int i = 0; i < tablevalue_el->GetNumDataLines();++i)
      {
	wxString tmp = std2wxstr(tablevalue_el->GetDataLine(i));
	MyPair * pair = new MyPair;
	size_t head1,tail1,head2;
	size_t j = 0;
	while ( (tmp[j] < '0' || tmp[j] > '9') && tmp[j] != '.' && tmp[j] != '-')
	  ++j;
	head1 = j++;
	while ( (tmp[j] >= '0' && tmp[j] <= '9') || tmp[j] == '.')
	  ++j;
	tail1 = j;
	while ( (tmp[j] < '0' || tmp[j] > '9') && tmp[j] != '.' && tmp[j] != '-')
	  ++j;
	head2 = j;

	tmp.Mid(head1, tail1-head1).ToDouble(&pair->x);
	tmp.Mid(head2).ToDouble(&pair->y);
	Table.Append(pair);
      }
    } else {
      cerr << "A table must be provided for the scheduled gain component" << endl;
      exit(-1);
    }
  }

  return strings;
}


