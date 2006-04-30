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

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(MyPairList);

IMPLEMENT_DYNAMIC_CLASS (Gain, ComponentShape)

Gain::Gain (double w, double h, const wxString & Name )
  :ComponentShape(w, h, wxT("PURE_GAIN"), Name),
   gain(1),
   asMin(0),
   asMax(0)
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
  if ( type == wxT("PURE_GAIN") )
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
  else if ( type == wxT("SCHEDULED_GAIN") )
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

  if ( type == wxT("AEROSURFACE_SCALE") )
    {
      clause->AddAttributeValue (wxT ("asMax"), asMax);
      clause->AddAttributeValue (wxT ("asMin"), asMin);
    }
  else if ( type == wxT("SCHEDULED_GAIN") )
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

  if ( type == wxT("AEROSURFACE_SCALE") )
    {
      clause->GetAttributeValue (wxT ("asMax"), asMax);
      clause->GetAttributeValue (wxT ("asMin"), asMin);
    }
  else if ( type == wxT("SCHEDULED_GAIN") )
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

  if (gain != 1 || type == wxT("PURE_GAIN"))
    stream << Pre << wxT("<gain> ")<< wxString::Format(wxT("%g"), gain) << wxT(" </gain>") << endl;

  if (type == wxT("AEROSURFACE_SCALE"))
  {
    stream << Pre << wxT("<limit>")<< endl;
    stream << Pre << wxT("  <min> ")<< wxString::Format(wxT("%g"), asMin) << wxT(" </min>") << endl;
    stream << Pre << wxT("  <max> ")<< wxString::Format(wxT("%g"), asMax) << wxT(" </max>") << endl;
    stream << Pre << wxT("</limit>")<< endl;
  }
  else if ((type == wxT("SCHEDULED_GAIN")))
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

  if (type == wxT("PURE_GAIN")) {
    if ( !el->FindElement("gain") ) {
      cerr << "No GAIN supplied for PURE_GAIN component: " << name << endl;
      exit(-1);
    }
  }

  if (type == wxT("AEROSURFACE_SCALE")) 
  {
    JSBSim::Element * scale_el = el->FindElement("limit");
    if (scale_el->FindElement("max") && scale_el->FindElement("min") )
    {
      asMax = scale_el->FindElementValueAsNumber("max");
      asMin = scale_el->FindElementValueAsNumber("min");
    } else {
      cerr << "A maximum and minimum scale value must be supplied for the "
              "aerosurface scale component" << endl;
      exit(-1);
    }
  }

  if (type == wxT("SCHEDULED_GAIN")) 
  {
    JSBSim::Element * table_el;
    if (table_el = el->FindElement("table")) 
    {
      indep = table_el->FindElementValue("independentVar").c_str();
      JSBSim::Element * tablevalue_el = table_el->FindElement("tableData");
      Table.Clear();
      for ( int i = 0; i < tablevalue_el->GetNumDataLines();++i)
      {
	wxString tmp = tablevalue_el->GetDataLine(i).c_str();
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


