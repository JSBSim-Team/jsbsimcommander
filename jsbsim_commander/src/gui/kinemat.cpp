/////////////////////////////////////////////////////////////////////////////
// Name:        kinemat.cpp
// Purpose:     support FGKinemat in FCSBuilder
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
#include "kinemat.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(MySettingPairList);

IMPLEMENT_DYNAMIC_CLASS (Kinemat, ComponentShape)

Kinemat::Kinemat (double w, double h, const wxString & Name )
  :ComponentShape(w, h, wxT("KINEMAT"), Name),
   isscale(true)
{
  pairlist.DeleteContents(true);

  SetAttachmentMode (ATTACHMENT_MODE_EDGE);

  ClearAttachments ();
  GetAttachments ().Append (new wxAttachmentPoint (0, w * 0.5, 0.0));
  GetAttachments ().Append (new wxAttachmentPoint (1, -w * 0.5, 0.0));

  input_sign_list.Append(new bool(false));

}

Kinemat::~Kinemat()
{
  pairlist.Clear();
}

void
Kinemat::OnDraw (wxDC & dc)
{
  ComponentShape::OnDraw (dc);
  ShowType(dc);
}

#if wxUSE_PROLOGIO
void 
Kinemat::WriteAttributes (wxExpr * clause)
{
  ComponentShape::WriteAttributes (clause);

  if (isscale)
    clause->AddAttributeValue (wxT ("isscale"), 1L);
  else
    clause->AddAttributeValue (wxT ("isscale"), 0L);

  wxExpr *list = new wxExpr(wxExprList);
  wxMySettingPairListNode * node = pairlist.GetFirst();
  while (node)
    {
      MySettingPair * value = node->GetData();
      list->Append(new wxExpr(value->pos));
      list->Append(new wxExpr(value->time));
      node = node->GetNext();
    }
  clause->AddAttributeValue(wxT("SettingPairList"), list);
}


void 
Kinemat::ReadAttributes (wxExpr * clause)
{
  ComponentShape::ReadAttributes (clause);

  long int tmp;

  clause->GetAttributeValue(wxT ("isscale"), tmp);
  if (tmp == 0L)
    isscale = false;
  else
    isscale = true;

  pairlist.Clear();
  wxExpr *list = clause->AttributeValue(wxT("SettingPairList"));
  if (list)
  {
    wxExpr *expr = list->GetFirst();
    while (expr)
    {
      MySettingPair * p = new MySettingPair;
      p->pos = expr->RealValue();
      expr = expr->GetNext();
      p->time = expr->RealValue();
      expr = expr->GetNext();
      pairlist.Append(p);
    }  
  }
}
#endif


void 
Kinemat::Copy (wxShape & copy)
{
  ComponentShape::Copy (copy);

  wxASSERT (copy.IsKindOf (CLASSINFO (Kinemat)));

  Kinemat & KinematCopy = (Kinemat &) copy;
  KinematCopy.isscale = isscale;

  KinematCopy.pairlist.Clear();
  wxMySettingPairListNode * node = pairlist.GetFirst();
  while (node)
    {
      MySettingPair * p = node->GetData();
      KinematCopy.pairlist.Append(new MySettingPair(*p));
      node = node->GetNext();
    }
}


void 
Kinemat::ExportXML(wxTextOutputStream & stream, const wxString & prefix)
{
  ExportHead(stream, prefix);

  wxString Pre = prefix + wxT("  ");
  
  ExportInputs(stream,Pre);

  stream << Pre << wxT("<traverse>") << endl;
  wxMySettingPairListNode * node = pairlist.GetFirst();
  while (node)
    {
      MySettingPair * p = node->GetData();
      stream << Pre << wxT("  <setting>") << endl;
      stream << Pre << wxT("    <position> ") << wxString::Format(wxT("%g"), p->pos) << wxT(" </position>") << endl;
      stream << Pre << wxT("    <time> ") << wxString::Format(wxT("%g"), p->time) << wxT(" </time>") << endl;      
      stream << Pre << wxT("  </setting>") << endl;
      node = node->GetNext();
    }
  stream << Pre << wxT("</traverse>") << endl;

  ExportCliper(stream,Pre);
  ExportOutput(stream,Pre);
  
  ExportTail(stream, prefix);
}


wxArrayString
Kinemat::ImportXML(JSBSim::Element * el)
{
  wxArrayString strings = ComponentShape::ImportXML(el);

  JSBSim::Element *traverse_el, *setting_el;

  isscale = true;
  pairlist.Clear();

  if (el->FindElement("noscale")) isscale = false;

  traverse_el = el->FindElement("traverse");
  setting_el = traverse_el->FindElement("setting");
  while (setting_el) {
    MySettingPair * pair = new MySettingPair;
    pair->pos  = setting_el->FindElementValueAsNumber("position");
    pair->time = setting_el->FindElementValueAsNumber("time");
    pairlist.Append(pair);
    setting_el = traverse_el->FindNextElement("setting");
  }

  if (pairlist.GetCount() <= 1) {
    cerr << "Kinematic component " << name
         << " must have more than 1 setting element" << endl;
    exit(-1);
  }
  
  return strings;
}

