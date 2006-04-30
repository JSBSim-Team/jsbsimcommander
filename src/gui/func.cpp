/////////////////////////////////////////////////////////////////////////////
// Name:        func.cpp
// Purpose:     support FGFCSFunction in FCSBuilder
// Author:      Matthew Gong
// Created:     07/24/2005
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
#include <wx/sstream.h>
#include <wx/datetime.h>
#include <wx/treectrl.h>

#include "wx/deprecated/setup.h"

#if wxUSE_PROLOGIO
#include "wx/deprecated/wxexpr.h"
#endif

#include "wx/ogl/ogl.h"
#include "FGXMLElement.h"
#include "TreeItem.h"

#include "shape.h"
#include "func.h"


IMPLEMENT_DYNAMIC_CLASS (FCSFunction, ComponentShape)

FCSFunction::FCSFunction (double w, double h, const wxString & Name )
  :ComponentShape(w, h, wxT("FUNCTION"), Name)
{
  SetAttachmentMode (ATTACHMENT_MODE_EDGE);

  ClearAttachments ();
  GetAttachments ().Append (new wxAttachmentPoint (0, w * 0.5, 0.0));
  GetAttachments ().Append (new wxAttachmentPoint (1, -w * 0.5, 0.0));

  input_sign_list.Append(new bool(false));

  function.Clear();
  wxStringOutputStream os(&function);
  wxTextOutputStream out(os,wxEOL_UNIX);
  out << wxT("<?xml version=\"1.0\"?>\n");
  wxString name = wxString::Format("fcs/function/func%lu", wxDateTime::Now().GetTicks());
  out << wxT("<function name=\"") << name << wxT("\">\n");
  out << wxT(" <value>1.000000</value>\n");
  out << wxT("</function>\n");
}

FCSFunction::~FCSFunction()
{
}

void
FCSFunction::OnDraw (wxDC & dc)
{
  ComponentShape::OnDraw (dc);
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

#if wxUSE_PROLOGIO
void 
FCSFunction::WriteAttributes (wxExpr * clause)
{
  ComponentShape::WriteAttributes (clause);
  clause->AddAttributeValueString (wxT ("function"), function);
}


void 
FCSFunction::ReadAttributes (wxExpr * clause)
{
  ComponentShape::ReadAttributes (clause);
  clause->GetAttributeValue(wxT ("function"), function);
}
#endif

/******************************************************************************
void FCSFunction::CopyTree(wxTreeCtrl * fromTree, wxTreeItemId & from, wxTreeCtrl * toTree, wxTreeItemId & to)
{
  TreeItemData_ebase * ddata = NULL;
  if (TreeItemData_function * sdata = dynamic_cast<TreeItemData_function *>(fromTree->GetItemData(from)))
  {
    ddata = new TreeItemData_function(*sdata);
  }
  else if (TreeItemData_Property * sdata = dynamic_cast<TreeItemData_Property *>(fromTree->GetItemData(from)))
  {
    ddata = new TreeItemData_Property(*sdata);
  }
  else if (TreeItemData_Value * sdata = dynamic_cast<TreeItemData_Value *>(fromTree->GetItemData(from)))
  {
    ddata = new TreeItemData_Value(*sdata);
  }
  else if (TreeItemData_Table * sdata = dynamic_cast<TreeItemData_Table *>(fromTree->GetItemData(from)))
  {
    ddata = new TreeItemData_Table(*sdata);
  }
  
  if (ddata)
  {
    ddata->tree = toTree;
    wxTreeItemId id = toTree->AppendItem(to, fromTree->GetItemText(from), fromTree->GetItemImage(from), fromTree->GetItemImage(from, wxTreeItemIcon_Selected), ddata );

    wxTreeItemIdValue cookie = (wxTreeItemIdValue)0503217LU;
    for (wxTreeItemId node = fromTree->GetFirstChild(from, cookie); node.IsOk(); node = fromTree->GetNextChild(from, cookie) )
    {
      CopyTree(fromTree, node, toTree, id);
    }
  }
}
******************************************************************************/

void 
FCSFunction::Copy (wxShape & copy)
{
  ComponentShape::Copy (copy);

  wxASSERT (copy.IsKindOf (CLASSINFO (FCSFunction)));

  FCSFunction & FCSFunctionCopy = (FCSFunction &) copy;

  FCSFunctionCopy.function = function;
}

void 
FCSFunction::ExportXML(wxTextOutputStream & stream, const wxString & prefix)
{
    
  ExportHead(stream, prefix);

  wxString Pre = prefix + wxT("  ");
   
  ExportInputs(stream,Pre);
  
  wxStringInputStream istr(function);
  wxTextInputStream tistr(istr);
  wxString tmp;
  do
  {
    tmp = tistr.ReadLine();
  }
  while (tmp.Find(wxT("<?"))!= -1 && !istr.Eof());
  if (!istr.Eof())
    stream << Pre << tmp << '\n';
  while (!istr.Eof())
  {
    stream << Pre << tistr.ReadLine() << '\n';
  }
  
  ExportCliper(stream,Pre);
  ExportOutput(stream,Pre);
  
  ExportTail(stream, prefix);
}

void CopyTo(wxTextOutputStream & out, JSBSim::Element * el, const wxString & prefix)
{
  int attribute_num = el->GetNumAttributeKeys();
  wxString attr = wxEmptyString;
  for (int i=0; i<attribute_num ; ++i)
  {
    string key = el->GetAttributeKey(i);
    attr += wxT(" ") + key + wxT("=\"") + el->GetAttributeValue(key) + wxT("\"");
  }
  int children_num = el->GetNumElements();
  if (children_num ==0)
  {
    out << prefix << wxT("<") << el->GetName() << attr << wxT(">");
    int line_num = el->GetNumDataLines();
    if (line_num>1)
      out << wxT("\n");
    for (int i=0; i< line_num; ++i)
    {
      out << el->GetDataLine(i);
      if (i<line_num-1) out << wxT("\n");
    }
    out << wxT("</") << el->GetName() << wxT(">\n");
  }
  else
  {
    out << prefix << wxT("<") << el->GetName() << attr << wxT(">\n");
    JSBSim::Element * tmp = el->GetElement();
    while (tmp)
    {
      CopyTo(out, tmp, prefix+wxT("    "));
      tmp = el->GetNextElement();
    }
    out << prefix << wxT("</") << el->GetName() << wxT(">\n");
  }
}


void mv2str(JSBSim::Element * el, wxTextOutputStream & out)
{
  out << wxT("<?xml version=\"1.0\"?>\n");

  CopyTo(out, el, wxEmptyString);
}


wxArrayString
FCSFunction::ImportXML(JSBSim::Element * el)
{
  wxArrayString strings = ComponentShape::ImportXML(el);

  JSBSim::Element *func_element;

  func_element = el->FindElement("function");
  if ( func_element) {
    function.Clear();
    wxStringOutputStream os(&function);
    wxTextOutputStream out(os,wxEOL_UNIX);
    mv2str(func_element, out);
  }

  return strings;
}

