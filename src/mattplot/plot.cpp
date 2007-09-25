/////////////////////////////////////////////////////////////////////////////
// Name:        plot.cpp
// Purpose:     Plot demo for wxWidgets
// Author:      Matthew Gong
// Modified by:
// Created:     2005
// Copyright:   (c) Matthew Gong
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_PRINTING_ARCHITECTURE
#error "You must set wxUSE_PRINTING_ARCHITECTURE to 1 in setup.h, and recompile the library."
#endif

#include <ctype.h>
#include <cmath>

#include <wx/fontdlg.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include <wx/sstream.h>
#include <wx/dcbuffer.h>
#include <wx/metafile.h>
#include <wx/print.h>
#include <wx/printdlg.h>
#include <wx/image.h>
#include <wx/accel.h>
#include <wx/xrc/xmlres.h>
#include <wx/clipbrd.h>
#include <wx/numdlg.h>
#include <wx/settings.h>
#include <wx/colordlg.h>

#if wxUSE_LIBGNOMEPRINT
#include "wx/html/forcelnk.h"
FORCE_LINK(gnome_print)
#endif

#include "MyApp.h"
#include "plot.h"
#include "Table.h"
#include "Condition.h"
#include "input_output/FGXMLParse.h"
#include "math/FGFunction.h"
#include "math/FGRealValue.h"
#include "math/FGPropertyValue.h"
#include "math/FGTable.h"
using JSBSim::FGXMLParse;
using JSBSim::FGFunction;
using JSBSim::FGRealValue;
using JSBSim::FGPropertyValue;
using JSBSim::FGTable;

static double ppi_x = 96.0;
static double ppi_y = 96.0;
static wxSize ppi(96, 96);

/** @file plot.cpp
  * Implement the plot handler, canvas, printer, etc.
  */


void make_order(vector<double> &x, vector<double> &y, size_t num)
{
    VPRBTree rbtree;
    vector<double>::iterator i=x.begin();
    vector<double>::iterator j=y.begin();
    for (;i != x.end(); ++i, ++j)
    {
      rbtree.insert(VPRBTree::value_type(*i, VP(*j, 1)));
    }
    while (rbtree.size() > num)
    {
      VPRBTree_Ptr no = rbtree.end();
      double len = 99e99;
      VPRBTree_Ptr k = rbtree.begin();
      VPRBTree_Ptr e = rbtree.end();
      ++k;
      --e;
      double last = k->first;
      for (++k; k != e; ++k)
      {
        double tmp = k->first - last;
        if (tmp < len)
        {
          no = k;
          len = tmp;
        }
        last = k->first;
      }
      VPRBTree_Ptr no2 = no;
      VPRBTree_Ptr end = rbtree.end();
      --end;
      --no2;
      if (no2 == rbtree.begin() || no == end)
      {
        break;
      }
      double p0 = no2->second.power;
      double p1 = no->second.power;
      double ps = p0+p1;
      rbtree.insert(
              VPRBTree::value_type(
                  (no2->first*p0+no->first*p1)/ps, 
                  VP((no2->second.value*p0+no->second.value*p1)/ps,
                     ps
                     )
                  )
              );
      rbtree.erase(no);
      rbtree.erase(no2);
    }
    x.clear();
    y.clear();
    for (VPRBTree_Ptr k = rbtree.begin();
            k != rbtree.end(); ++k)
    {
      x.push_back((*k).first);
      y.push_back((*k).second.value);
    }
}

void LoadColour(Element * element, wxColour &colour)
{
    wxString c = std2wxstr(element->GetDataLine());
    colour.Set(c);
    /*
    if (c.Length() == 7 && c[0] == wxChar('#'))
    {
      unsigned long r,g,b;
      r = g = b = 0xFF;
      c.Mid(1, 2).ToULong(&r, 16);
      c.Mid(3, 2).ToULong(&g, 16);
      c.Mid(5, 2).ToULong(&b, 16);
      colour.Set((unsigned char)r, (unsigned char)g, (unsigned char)b);
    }
    */
}

void LoadPen(Element * element, wxPen &pen)
{
    Element *el;
    if ((el = element->FindElement("colour")))
    {
      LoadColour(el, pen.GetColour());
    }
    
   double size = element->FindElementValueAsNumber("width");
   if (size < 9999 && size > 0)
   {
     pen.SetWidth((int)size);
   }
   else
   {
     pen.SetWidth(1);
   }

   wxString styles = std2wxstr(element->FindElementValue("style"));
   int style_tag = 0;
   while (!styles.IsEmpty())
   {
     wxString style = styles.BeforeFirst('|');
     if (style.Upper().Contains(wxT("SOLID")))
     {
       style_tag |= wxSOLID;
     }
     else if (style.Upper().Contains(wxT("TRANSPARENT")))
     {
       style_tag |= wxTRANSPARENT;
     }
     else if (style.Upper().Contains(wxT("DOT")))
     {
       style_tag |= wxDOT;
     }
     else if (style.Upper().Contains(wxT("LONG_DASH")))
     {
       style_tag |= wxLONG_DASH;
     }
     else if (style.Upper().Contains(wxT("SHORT_DASH")))
     {
       style_tag |= wxSHORT_DASH;
     }
     else if (style.Upper().Contains(wxT("DOT_DASH")))
     {
       style_tag |= wxDOT_DASH;
     }
     else if (style.Upper().Contains(wxT("BDIAGONAL_HATCH")))
     {
       style_tag |= wxBDIAGONAL_HATCH;
     }
     else if (style.Upper().Contains(wxT("CROSSDIAG_HATCH ")))
     {
       style_tag |= wxCROSSDIAG_HATCH ;
     }
     else if (style.Upper().Contains(wxT("FDIAGONAL_HATCH")))
     {
       style_tag |= wxFDIAGONAL_HATCH;
     }
     else if (style.Upper().Contains(wxT("HORIZONTAL_HATCH")))
     {
       style_tag |= wxHORIZONTAL_HATCH;
     }
     else if (style.Upper().Contains(wxT("VERTICAL_HATCH")))
     {
       style_tag |= wxVERTICAL_HATCH;
     }
     else if (style.Upper().Contains(wxT("CROSS_HATCH")))
     {
       style_tag |= wxCROSS_HATCH;
     }

     styles = styles.AfterFirst('|');
   }
   if (style_tag == 0)
   {
     style_tag = wxSOLID;
   }
   pen.SetStyle(style_tag);
}

void LoadFont(Element * element, wxFont &fnt)
{
  {
   wxString info = std2wxstr(element->FindElementValue("NativeFontInfo"));
   if (!info.IsEmpty() && fnt.SetNativeFontInfo(info))
   {
     return;
   }
  }
  {
   wxString info = std2wxstr(element->FindElementValue("NativeFontInfoUserDesc"));
   if (!info.IsEmpty() && fnt.SetNativeFontInfoUserDesc(info))
   {
     return;
   }
  }

   double size = element->FindElementValueAsNumber("size");
   if (size < 9999 && size > 0 )
   {
     fnt.SetPointSize((int)size);
   }

   wxString style_str = std2wxstr(element->FindElementValue("style"));
   if (style_str.Find(wxT("italic")) != -1)
   {
     fnt.SetStyle( wxFONTSTYLE_ITALIC );
   }
   else if (style_str.Find(wxT("slant")) != -1)
   {
     fnt.SetStyle( wxFONTSTYLE_SLANT );
   }
   else
   {
     fnt.SetStyle( wxFONTSTYLE_NORMAL );
   }

   wxString weight_str = std2wxstr(element->FindElementValue("weight"));
   weight_str.LowerCase();
   if (weight_str.Find(wxT("bold")) != -1)
   {
     fnt.SetWeight( wxFONTWEIGHT_BOLD);
   }
   else if (weight_str.Find(wxT("light")) != -1)
   {
     fnt.SetWeight( wxFONTWEIGHT_LIGHT);
   }
   else
   {
     fnt.SetWeight( wxFONTWEIGHT_NORMAL);
   }
   
   wxString family_str = std2wxstr(element->FindElementValue("family"));
   family_str.LowerCase();
   if (family_str.Find(wxT("decorative")) != -1)
   {
     fnt.SetFamily( wxFONTFAMILY_DECORATIVE );
   }
   else if (family_str.Find(wxT("roman")) != -1)
   {
     fnt.SetFamily( wxFONTFAMILY_ROMAN );
   }
   else if (family_str.Find(wxT("script")) != -1)
   {
     fnt.SetFamily( wxFONTFAMILY_SCRIPT );
   }
   else if (family_str.Find(wxT("swiss")) != -1)
   {
     fnt.SetFamily( wxFONTFAMILY_SWISS );
   }
   else if (family_str.Find(wxT("modern")) != -1)
   {
     fnt.SetFamily( wxFONTFAMILY_MODERN );
   }
   else if (family_str.Find(wxT("teletype")) != -1)
   {
     fnt.SetFamily( wxFONTFAMILY_TELETYPE );
   }
   else
   {
     fnt.SetFamily( wxFONTFAMILY_DEFAULT );
   }

   wxString underlined = std2wxstr(element->FindElementValue("underlined"));
   underlined.LowerCase();
   if (underlined == wxT("true") )
   {
     fnt.SetUnderlined(true);
   }
   else
   {
     fnt.SetUnderlined(false);
   }
   
   wxString face_name_str = std2wxstr(element->FindElementValue("face"));
   if (!face_name_str.IsEmpty())
   {
     fnt.SetFaceName(face_name_str);
   }
}


void LoadPainter(Element * element, wxPen *pen, wxFont *fnt)
{
  if (element)
  {
    Element * tmp;
    if (pen && (tmp = element->FindElement("pen")))
    {
        LoadPen(tmp, *pen);
    }
    if (fnt && (tmp = element->FindElement("font")))
    {
        LoadFont(tmp, *fnt);
    }    
  }
}

void ExportColour(wxTextOutputStream & tstream, const wxString & prefix, const wxColour &color, const wxString &tag=wxT("colour"))
{
  tstream << prefix << _("<!-- Colour Setting. Initial with '#', followd with RGB Hex number. -->\n");
  tstream << prefix << wxT("<") << tag << wxT(">") << color.GetAsString(wxC2S_HTML_SYNTAX) << wxT("</") << tag << wxT(">\n");
}

void ExportPen(wxTextOutputStream & tstream, const wxString & prefix, const wxPen &pen, const wxString &tag=wxT("pen"))
{
  tstream << prefix << _("<!-- Pen Setting -->\n");
  tstream << prefix << wxT("<") << tag << wxT(">\n");
  {
    wxString pre = prefix + wxT("    ");
    ExportColour(tstream, pre, pen.GetColour());
    int w = pen.GetWidth();
    if (w < 1)
      w = 1;
    tstream << pre << _("<!-- The width of the Pen -->\n");
    tstream << pre << wxT("<width> ") << w << wxT(" </width>\n");
    wxString sty= wxEmptyString;
    int s = pen.GetStyle();
    bool first = true;
    switch (s)
    {
    case wxTRANSPARENT :
    {
        sty = wxT("TRANSPARENT");
    }
    break;
    case wxDOT :
    {
        sty = wxT("DOT");
    }
    break;
    case wxLONG_DASH :
    {
        sty = wxT("LONG_DASH");
    }
    break;
    case wxSHORT_DASH :
    {
        sty = wxT("SHORT_DASH");
    }
    break;
    case wxDOT_DASH :
    {
        sty = wxT("DOT_DASH");
    }
    break;
    case wxBDIAGONAL_HATCH :
    {
        sty = wxT("BDIAGONAL_HATCH");
    }
    break;
    case wxCROSSDIAG_HATCH :
    {
        sty = wxT("CROSSDIAG_HATCH");
    }
    break;
    case wxFDIAGONAL_HATCH :
    {
        sty = wxT("FDIAGONAL_HATCH");
    }
    break;
    case wxHORIZONTAL_HATCH :
    {
        sty = wxT("HORIZONTAL_HATCH");
    }
    break;
    case wxVERTICAL_HATCH :
    {
        sty = wxT("VERTICAL_HATCH");
    }
    break;
    case wxCROSS_HATCH :
    {
        sty = wxT("CROSS_HATCH");
    }
    break;
    default :
    {
        sty = wxT("SOLID");
    }
    break;
    }
    tstream << pre << _("<!-- The style of the pen -->\n");
    tstream << pre << _("<!-- one of these tags: -->\n");
    tstream << pre << _("<!-- one of these tags: \n");
    tstream << pre << _("    SOLID             Solid style. \n");
    tstream << pre << _("    TRANSPARENT       No pen is used. \n");
    tstream << pre << _("    DOT Dotted        style. \n");
    tstream << pre << _("    LONG_DASH         Long dashed style. \n");
    tstream << pre << _("    SHORT_DASH        Short dashed style. \n");
    tstream << pre << _("    DOT_DASH          Dot and dash style. \n");
    tstream << pre << _("    BDIAGONAL_HATCH   Backward diagonal hatch. \n");
    tstream << pre << _("    CROSSDIAG_HATCH   Cross-diagonal hatch. \n");
    tstream << pre << _("    FDIAGONAL_HATCH   Forward diagonal hatch. \n");
    tstream << pre << _("    CROSS_HATCH       Cross hatch. \n");
    tstream << pre << _("    HORIZONTAL_HATCH  Horizontal hatch. \n");
    tstream << pre << _("    VERTICAL_HATCH    Vertical hatch. -->\n");
    tstream << pre << wxT("<style> ") << sty << wxT(" </style>\n");
  }
  tstream << prefix << wxT("</") << tag << wxT(">\n");
}

void ExportFont(wxTextOutputStream & tstream, const wxString & prefix, const wxFont &fnt, const wxString &tag=wxT("font"))
{
  tstream << prefix << _("<!-- Font Setting -->\n");
  tstream << prefix << wxT("<") << tag << wxT(">\n");
  {
    wxString pre = prefix + wxT("    ");
    tstream << pre << wxT("<NativeFontInfo>") << fnt.GetNativeFontInfoDesc() << wxT("</NativeFontInfo>\n") ;
    tstream << pre << wxT("<NativeFontInfoUserDesc>") << fnt.GetNativeFontInfoUserDesc() << wxT("</NativeFontInfoUserDesc>\n") ;

    tstream << pre << _("<!-- the point size of the font -->\n");
    tstream << pre << wxT("<size> ") << fnt.GetPointSize() << wxT(" </size>\n");
    wxString style_str;
    switch (fnt.GetStyle())
    {
      case wxFONTSTYLE_ITALIC :
        style_str = wxT("italic");
        break;
      case wxFONTSTYLE_SLANT :
        style_str = wxT("slant");
        break;
      default :
        style_str = wxT("normal");
        break;
    }
    tstream << pre << _("<!-- the style of the font -->\n");
    tstream << pre << _("<!-- one of these styles\n");
    tstream << pre << wxT("     italic \n");
    tstream << pre << wxT("     slant\n");
    tstream << pre << wxT("     normal -->\n");
    tstream << pre << wxT("<style> ") << style_str << wxT(" </style>\n");
    wxString weight_str;
    switch (fnt.GetWeight())
    {
      case wxFONTWEIGHT_BOLD :
        weight_str = wxT("bold");
        break;
      case wxFONTWEIGHT_LIGHT :
        weight_str = wxT("light");
        break;
      default :
        weight_str = wxT("normal");
        break;
    }
    tstream << pre << _("<!-- the weight of the font -->\n");
    tstream << pre << _("<!-- one of these styles\n");
    tstream << pre << wxT("     bold \n");
    tstream << pre << wxT("     light\n");
    tstream << pre << wxT("     normal -->\n");
    tstream << pre << wxT("<weight> ") << weight_str << wxT(" </weight>\n");
    wxString family_str;
    switch (fnt.GetFamily())
    {
      case wxFONTFAMILY_DECORATIVE :
        family_str = wxT("decorative");
        break;
      case wxFONTFAMILY_ROMAN :
        family_str = wxT("roman");
        break;
      case wxFONTFAMILY_SCRIPT :
        family_str = wxT("script");
        break;
      case wxFONTFAMILY_MODERN :
        family_str = wxT("modern");
        break;
      case wxFONTFAMILY_SWISS :
        family_str = wxT("swiss");
        break;
      case wxFONTFAMILY_TELETYPE :
        family_str = wxT("teletype");
        break;
      default :
        family_str = wxT("default");
    }
    tstream << pre << _("<!-- the family of the font -->\n");
    tstream << pre << _("<!-- one of these families\n");
    tstream << pre << wxT("     decorative roman \n");
    tstream << pre << wxT("     script     swiss\n");
    tstream << pre << wxT("     modern     teletype\n");
    tstream << pre << wxT("     default -->\n");
    tstream << pre << wxT("<family> ") << family_str << wxT(" </family>\n");
    tstream << pre << _("<!-- the face name of the font -->\n");
    tstream << pre << wxT("<face> ") << fnt.GetFaceName() << wxT(" </face>\n");
    tstream << pre << _("<!-- whether underlined font (true or false) -->\n");
    if (fnt.GetUnderlined())
    {
      tstream << pre << wxT("<underlined> true </underlined>\n");
    }
    else
    {
      tstream << pre << wxT("<underlined> false </underlined>\n");
    }
  }
  tstream << prefix << wxT("</") << tag << wxT(">\n");
}

void ExportPainter(wxTextOutputStream & tstream, const wxString & prefix, const wxPen &pen, const wxFont &fnt, const wxString &tag=wxT("painter"))
{
  tstream << prefix << wxT("<") << tag << wxT(">\n");
  {
    wxString pre = prefix + wxT("    ");
    ExportPen(tstream, pre, pen);
    ExportFont(tstream, pre, fnt);
  }
  tstream << prefix << wxT("</") << tag << wxT(">\n");
}


typedef vector<const Table *> TableCPList;
typedef TableCPList::const_iterator TableCPListCIter;

void Export1DTable(wxTextOutputStream & tstream, const wxString & prefix, const TableCPList & list)
{
  tstream << prefix << std2wxstr(list[0]->GetIndepVar()) << wxT("(") << std2wxstr(list[0]->GetIndepVarLabel()) << wxT(") ") ;
  for (TableCPListCIter i = list.begin(); i != list.end(); ++i)
  {
    tstream << std2wxstr((*i)->GetName()) << wxT('(') << std2wxstr((*i)->GetNameLabel()) << wxT(") ");
  }
  tstream << wxT("\n");
  for (unsigned i=0u; i<list[0]->GetSize(); ++i)
  {
    tstream << prefix;
    TableCPListCIter j = list.begin();
    tstream << (*j)->GetData()[i].key << wxT(' ') << (*j)->GetData()[i].value << wxT(' ');
    ++j;
    for (; j != list.end(); ++j)
    {
      tstream << (*j)->GetData()[i].value << wxT(' ');
    }
    tstream << wxT("\n");
  }
}

void ExportMDTable(wxTextOutputStream & tstream, const wxString & prefix, const TableCPList & list)
{
  tstream << prefix << wxT("<independentVar>") << std2wxstr(list[0]->GetIndepVar()) << wxT("(") << std2wxstr(list[0]->GetIndepVarLabel()) << wxT(")</independentVar>\n");
  const Matt::TablePairList& l = list[0]->GetTables();
  for ( unsigned i = 0u; i < l.size(); ++i)
  {
    tstream << prefix << wxT("  <table key=\"") << l[i].key << wxT("\">\n");
    TableCPList tmp;
    for (TableCPList::const_iterator j=list.begin(); j != list.end(); ++j)
    {
      tmp.push_back(&((*j)->GetTables()[i].table));
    }
    if (tmp[0]->GetDimension() == 1u)
    {
      Export1DTable(tstream, wxEmptyString, tmp);
    }
    else
    {
      ExportMDTable(tstream, prefix+wxT("  "), tmp);
    }
    tstream << prefix << wxT("  </table>\n");
  }
}

void ExportTable(wxTextOutputStream & tstream, const wxString & prefix, const TableList & list, const wxString &tag=wxT("table"))
{
  tstream << prefix << wxT("<") << tag << wxT(">\n") ;
  wxString pre = prefix+wxT("  ");
  const Table & t1 = list[0];
  TableCPList plist;
  for (Matt::TableListCIter i= list.begin(); i!= list.end(); ++i)
  {
    plist.push_back(&(*i));
  }
  if (t1.GetDimension() == 1u)
  {
    Export1DTable(tstream, wxEmptyString, plist);
  }
  else
  {
    ExportMDTable(tstream, pre, plist);
  }
  tstream << prefix << wxT("</") << tag << wxT(">\n");
}

void ExportFGTable(wxTextOutputStream & tstream, const wxString & pre, const FGTable *tab)
{
  //TODO export FGTable
  //tstream << prefix << wxT("<table>\n");
  //tstream << prefix << wxT("</table>\n");
}

void ExportFunction(wxTextOutputStream & tstream, const wxString & prefix, const FGFunction * func)
{
  switch (func->GetType())
  {
    case FGFunction::eTopLevel :
      tstream << prefix << wxT("<function name=\"") << std2wxstr(func->GetName()) << wxT("\">\n");
      break;
    case FGFunction::eProduct :
      tstream << prefix << wxT("<product>\n");
      break;
    case FGFunction::eDifference :
      tstream << prefix << wxT("<difference>\n");
      break;
    case FGFunction::eSum :
      tstream << prefix << wxT("<sum>\n");
      break;
    case FGFunction::eQuotient :
      tstream << prefix << wxT("<quotient>\n");
      break;
    case FGFunction::ePow :
      tstream << prefix << wxT("<pow>\n");
      break;
    case FGFunction::eAbs :
      tstream << prefix << wxT("<abs>\n");
      break;
    case FGFunction::eSin :
      tstream << prefix << wxT("<sin>\n");
      break;
    case FGFunction::eExp :
      tstream << prefix << wxT("<exp>\n");
      break;
    case FGFunction::eCos :
      tstream << prefix << wxT("<cos>\n");
      break;
    case FGFunction::eTan :
      tstream << prefix << wxT("<tan>\n");
      break;
    case FGFunction::eASin :
      tstream << prefix << wxT("<asin>\n");
      break;
    case FGFunction::eACos :
      tstream << prefix << wxT("<acos>\n");
      break;
    case FGFunction::eATan :
      tstream << prefix << wxT("<atan>\n");
      break;
    case FGFunction::eATan2 :
      tstream << prefix << wxT("<atan2>\n");
      break;
    case FGFunction::eDerivative :
      tstream << prefix << wxT("<derivative>\n");
      break;
    case FGFunction::eIntegral :
      tstream << prefix << wxT("<integral>\n");
      break;
    case FGFunction::eLT :
      tstream << prefix << wxT("<lt>\n");
      break;
    case FGFunction::eLE :
      tstream << prefix << wxT("<le>\n");
      break;
    case FGFunction::eGT :
      tstream << prefix << wxT("<gt>\n");
      break;
    case FGFunction::eGE :
      tstream << prefix << wxT("<ge>\n");
      break;
    case FGFunction::eEQ :
      tstream << prefix << wxT("<eq>\n");
      break;
    case FGFunction::eNE :
      tstream << prefix << wxT("<ne>\n");
      break;
    case FGFunction::eAND :
      tstream << prefix << wxT("<and>\n");
      break;
    case FGFunction::eOR :
      tstream << prefix << wxT("<or>\n");
      break;
    case FGFunction::eNOT :
      tstream << prefix << wxT("<not>\n");
      break;
    case FGFunction::eIFTHEN :
      tstream << prefix << wxT("<ifthen>\n");
      break;
  }
  {
    wxString pre = prefix + wxT("    ");
    wxString desc = std2wxstr(func->GetDescription());
    if (!desc.IsEmpty())
    {
      tstream << pre << wxT("<description>") << desc << wxT("</description>\n");
    }
    for (unsigned i=0u; i < func->GetParaCount(); ++i)
    {
      const FGFunction * f = dynamic_cast<const FGFunction *>(func->GetParameter(i));
      if (f)
      {
        ExportFunction(tstream, pre, f);
      }
      else
      {
        const FGPropertyValue * p = dynamic_cast<const FGPropertyValue *>(func->GetParameter(i));
        if (p)
        {
          tstream << pre << wxT("<property>") << std2wxstr(p->GetName()) << wxT("</property>\n");
        }
        else
        {
          const FGRealValue * r = dynamic_cast<const FGRealValue *>(func->GetParameter(i));
          if (r)
          {
            tstream << pre << wxT("<value>") << r->GetValue() << wxT("</value>\n");
          }
          else
          {
            const FGTable * t = dynamic_cast<const FGTable *>(func->GetParameter(i));
            if (t)
            {
              ExportFGTable(tstream, pre, t);
            }
            else
            {
              throw "Bad operation  detected";
            }
          }
        }
      }
    }
  }
  switch (func->GetType())
  {
    case FGFunction::eTopLevel :
      tstream << prefix << wxT("</function>\n");
      break;
    case FGFunction::eProduct :
      tstream << prefix << wxT("</product>\n");
      break;
    case FGFunction::eDifference :
      tstream << prefix << wxT("</difference>\n");
      break;
    case FGFunction::eSum :
      tstream << prefix << wxT("</sum>\n");
      break;
    case FGFunction::eQuotient :
      tstream << prefix << wxT("</quotient>\n");
      break;
    case FGFunction::ePow :
      tstream << prefix << wxT("</pow>\n");
      break;
    case FGFunction::eAbs :
      tstream << prefix << wxT("</abs>\n");
      break;
    case FGFunction::eSin :
      tstream << prefix << wxT("</sin>\n");
      break;
    case FGFunction::eExp :
      tstream << prefix << wxT("</exp>\n");
      break;
    case FGFunction::eCos :
      tstream << prefix << wxT("</cos>\n");
      break;
    case FGFunction::eTan :
      tstream << prefix << wxT("</tan>\n");
      break;
    case FGFunction::eASin :
      tstream << prefix << wxT("</asin>\n");
      break;
    case FGFunction::eACos :
      tstream << prefix << wxT("</acos>\n");
      break;
    case FGFunction::eATan :
      tstream << prefix << wxT("</atan>\n");
      break;
    case FGFunction::eATan2 :
      tstream << prefix << wxT("</atan2>\n");
      break;
    case FGFunction::eDerivative :
      tstream << prefix << wxT("</derivative>\n");
      break;
    case FGFunction::eIntegral :
      tstream << prefix << wxT("</integral>\n");
      break;
    case FGFunction::eLT :
      tstream << prefix << wxT("</lt>\n");
      break;
    case FGFunction::eLE :
      tstream << prefix << wxT("</le>\n");
      break;
    case FGFunction::eGT :
      tstream << prefix << wxT("</gt>\n");
      break;
    case FGFunction::eGE :
      tstream << prefix << wxT("</ge>\n");
      break;
    case FGFunction::eEQ :
      tstream << prefix << wxT("</eq>\n");
      break;
    case FGFunction::eNE :
      tstream << prefix << wxT("</ne>\n");
      break;
    case FGFunction::eAND :
      tstream << prefix << wxT("</and>\n");
      break;
    case FGFunction::eOR :
      tstream << prefix << wxT("</or>\n");
      break;
    case FGFunction::eNOT :
      tstream << prefix << wxT("</not>\n");
      break;
    case FGFunction::eIFTHEN :
      tstream << prefix << wxT("</ifthen>\n");
      break;
  }
}

void ExportCondition(wxTextOutputStream & tstream, const wxString & prefix, const Condition * cond)
{
  if (cond->isGroup)
  {
    tstream << prefix << wxT("<condition logic=\""); 
    switch (cond->Logic)
    {
      case Condition::eOR :
        tstream << wxT("OR");
        break;
      default :
        tstream << wxT("AND");
    }
    tstream << wxT("\">\n");
    wxString pre = prefix + wxT("    ");
    for (vector <Condition>::const_iterator i=cond->conditions.begin(); i != cond->conditions.end(); ++i)
    {
      ExportCondition(tstream, pre, &(*i));
    }
    tstream << prefix << wxT("</condition>\n"); 
  }
  else
  {
    tstream << prefix;
    if (cond->TestParam1)
    {
      if (cond->sign1 < 0)
      {
        tstream << wxT('-');
      }
      tstream << std2wxstr(cond->property1) << wxT(' ');
    }
    else
    {
      tstream << cond->TestValue1 << wxT(' ');
    }
    tstream << std2wxstr(cond->conditional) << wxT(' ');
    if (cond->TestParam2)
    {
      if (cond->sign2 < 0)
      {
        tstream << wxT('-');
      }
      tstream << std2wxstr(cond->property2) << '\n';
    }
    else
    {
      tstream << cond->TestValue2 << '\n';
    }
  }
}

template <class T>
void readNumsfromString(vector<T> &list, const wxString & str)
{
  wxStringInputStream mis(str);
  wxTextInputStream tis(mis, wxT("\t, "));

  list.clear();

  while (!mis.Eof())
  {
    wxString tmp = tis.ReadWord();
    double tmpd;
    if (tmp.ToDouble(&tmpd))
    {
      list.push_back((T)tmpd);
    }
    else
    {
      list.push_back(0);
    }
  }
}

void DrawText(wxDC& dc, const wxString & text, wxCoord & x, const wxCoord & y, const wxFont & font, const bool & isDraw=true)
{
  dc.SetFont(font);
  wxCoord w,h;
  dc.GetTextExtent(text, &w, &h);
  if (isDraw)
    dc.DrawText(text, x, y);
  x += w;
}

void DrawTexText(wxDC& dc, wxString text, wxCoord x, const wxCoord &y, wxCoord & tw, wxCoord & mh, const bool & isDraw=true)
{
  dc.GetTextExtent(text, &tw, &mh);
  wxFont old_font= dc.GetFont();
  wxFont small_font = old_font;
  small_font.SetPointSize( int(old_font.GetPointSize() *0.6667+0.5) + 1);
  dc.SetFont(small_font);
  wxCoord x0, y1;
  dc.GetTextExtent(text, &x0, &y1);
  y1 = y + mh - y1;
  x0 = x;

  while (!text.IsEmpty())
  {
    int i, j;
    if ( (i = text.Find(wxT('^'))) != -1 || (j = text.Find(wxT('_'))) != -1)
    {
      bool down = false;
      if (j < i && j != -1 || i == -1)
      {
        i = j;
	down = true;
      }
      if (text.Length() < 2)
	break;
      if (i == 0)
      {
	wxString tmp;
        if (text[1] == wxT('{'))
	{
	  j = text.Find(wxT('}'));
	  if (j == -1)
	  {
	    tmp = text.Mid(2);
	    text.Empty(); 
	  }
	  else
	  {
	    tmp = text.Mid(2, j-2);
	    if (j == text.Length()-1)
	      text.Empty();
	    else
	      text = text.Mid(j+1);
	  }
	}
	else
	{
	  tmp = text.Mid(1u, 1u);
	  text = text.Mid(2);
	}
        if (down)
	{
          DrawText(dc, tmp, x, y1+2, small_font, isDraw);
	}
	else
	{
          DrawText(dc, tmp, x, y-3, small_font, isDraw);
	}
      }
      else
      {
	wxString tmp = text.Mid(0u, i);
	text = text.Mid(i);
        DrawText(dc, tmp, x, y, old_font, isDraw);
      }
    }
    else
    {
      DrawText(dc, text, x, y, old_font, isDraw);
      text.Empty();
    }
  }
  dc.SetFont(old_font);
  tw = x - x0;
}


wxPoint Curve::points[8][7];
bool Curve::inited = false;

void Curve::init_points(wxCoord size)
{
  points[0][0].x=size;
  points[0][0].y=size;
  points[0][1].x=-size;
  points[0][1].y=size;
  points[0][2].x=-size;
  points[0][2].y=-size;
  points[0][3].x=size;
  points[0][3].y=-size;
  points[0][4].x=size;
  points[0][4].y=size;

  points[1][0].x=0;
  points[1][0].y=-size;
  points[1][1].x=size;
  points[1][1].y=size;
  points[1][2].x=-size;
  points[1][2].y=size;
  points[1][3].x=0;
  points[1][3].y=-size;

  points[2][0].x=-size;
  points[2][0].y=-size;
  points[2][1].x=size;
  points[2][1].y=-size;
  points[2][2].x=0;
  points[2][2].y=size;
  points[2][3].x=-size;
  points[2][3].y=-size;

  points[3][0].x=-size;
  points[3][0].y=-size;
  points[3][1].x=size;
  points[3][1].y=0;
  points[3][2].x=-size;
  points[3][2].y=size;
  points[3][3].x=-size;
  points[3][3].y=-size;

  points[4][0].x=-size;
  points[4][0].y=0;
  points[4][1].x=size;
  points[4][1].y=-size;
  points[4][2].x=size;
  points[4][2].y=size;
  points[4][3].x=-size;
  points[4][3].y=0;

  points[5][0].x=0;
  points[5][0].y=-size;
  points[5][1].x=size;
  points[5][1].y=0;
  points[5][2].x=0;
  points[5][2].y=size;
  points[5][3].x=-size;
  points[5][3].y=0;
  points[5][4].x=0;
  points[5][4].y=-size;

  points[6][0].x=0;
  points[6][0].y=-size;
  points[6][1].x=size;
  points[6][1].y=-size/2;
  points[6][2].x=size;
  points[6][2].y=size;
  points[6][3].x=-size;
  points[6][3].y=size;
  points[6][4].x=-size;
  points[6][4].y=-size/2;
  points[6][5].x=0;
  points[6][5].y=-size;

  points[7][0].x=0;
  points[7][0].y=-size;
  points[7][1].x=size;
  points[7][1].y=-size/2;
  points[7][2].x=size;
  points[7][2].y=size/2;
  points[7][3].x=0;
  points[7][3].y=size;
  points[7][4].x=-size;
  points[7][4].y=size/2;
  points[7][5].x=-size;
  points[7][5].y=-size/2;
  points[7][6].x=0;
  points[7][6].y=-size;
}

void Curve::draw_symbol(wxDC &dc, wxCoord x, wxCoord y, CURVE_SYMBOL symbol, bool filled)
{
 if (filled)
 {
  switch (symbol)
  {
  case CURVE_REC: 
    dc.DrawPolygon(5, points[0], x, y);
    break;
  case CURVE_TRI_UP :
    dc.DrawPolygon(4, points[1], x, y);
    break;
  case CURVE_TRI_DOWN :
    dc.DrawPolygon(4, points[2], x, y);
    break;
  case CURVE_TRI_LEFT :
    dc.DrawPolygon(4, points[3], x, y);
    break;
  case CURVE_TRI_RIGHT :
    dc.DrawPolygon(4, points[4], x, y);
    break;
  case CURVE_DIAMOND :
    dc.DrawPolygon(5, points[5], x, y);
    break;
  case CURVE_PENTAGON :
    dc.DrawPolygon(6, points[6], x, y);
    break;
  case CURVE_HEXAGON :
    dc.DrawPolygon(7, points[7], x, y);
    break;
  default :
    break;
  }
 }
 else
 {
  switch (symbol)
  {
  case CURVE_REC: 
    dc.DrawLines(5, points[0], x, y);
    break;
  case CURVE_TRI_UP :
    dc.DrawLines(4, points[1], x, y);
    break;
  case CURVE_TRI_DOWN :
    dc.DrawLines(4, points[2], x, y);
    break;
  case CURVE_TRI_LEFT :
    dc.DrawLines(4, points[3], x, y);
    break;
  case CURVE_TRI_RIGHT :
    dc.DrawLines(4, points[4], x, y);
    break;
  case CURVE_DIAMOND :
    dc.DrawLines(5, points[5], x, y);
    break;
  case CURVE_PENTAGON :
    dc.DrawLines(6, points[6], x, y);
    break;
  case CURVE_HEXAGON :
    dc.DrawLines(7, points[7], x, y);
    break;
  default :
    break;
  }
 }
}

Curve::Curve() :
  fnt(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL),
  pen(wxT("black"), 1, wxSOLID), 
  type(CURVE_BSPLINE), 
  symbol(CURVE_NONE), 
  text_pos(CURVE_NO_TEXT), 
  text_value(0.2), 
  text(wxEmptyString),
  is_symbol_filled(false),
  isHide(false)
{
  if (!inited)
  {
    init_points();
  }
}

Curve::Curve(const double * x, size_t nx, const double * y) :
  Matt::BSplineD(x, nx, y),
  fnt(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL),
  pen(wxT("black"), 1, wxSOLID), 
  type(CURVE_BSPLINE), 
  symbol(CURVE_NONE), 
  text_pos(CURVE_NO_TEXT), 
  text_value(0.2), 
  legend_pos(0),
  text(wxEmptyString),
  is_symbol_filled(false),
  isHide(false)
{
  if (!inited)
  {
    init_points();
  }
}

Curve::Curve(vector<double> x, vector<double> y) :
  Matt::BSplineD(x, y),
  fnt(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL),
  pen(wxT("black"), 1, wxSOLID), 
  type(CURVE_BSPLINE), 
  symbol(CURVE_NONE), 
  text_pos(CURVE_NO_TEXT), 
  text_value(0.2), 
  legend_pos(0),
  text(wxEmptyString),
  is_symbol_filled(false),
  isHide(false)
{
  if (!inited)
  {
    init_points();
  }
}

void Curve::DrawText(wxDC& dc, const wxPoint * list, int s)
{
  wxString ttext = GetWholeText();
  if (ttext.IsEmpty() || text_pos == CURVE_NO_TEXT)
  {
    return;
  }
  else
  {
    // Calculate a suitable scaling factor
    double scaleX=(double)(ppi.GetWidth()/ppi_x);
    double scaleY=(double)(ppi.GetHeight()/ppi_y);
    // Use x or y scaling factor, whichever fits on the DC
    double actualScale = wxMin(scaleX,scaleY);
    // Set the scale
    wxFont tmp_font = fnt;
    tmp_font.SetPointSize(int(tmp_font.GetPointSize()*actualScale+0.5));

    dc.SetFont(tmp_font);
    dc.SetTextForeground(pen.GetColour());
    wxCoord tw, th;
    dc.GetTextExtent(ttext, &tw, &th);
    wxCoord tw2=tw*tw;
    int j = (int)((s-1) * text_value);
    if (text_pos == CURVE_LEFT_UP)
    {
      if (j == s-1)
          j = s-2;
      int i = j+1;
      while (i<s-1&&(list[i].y-list[j].y)*(list[i].y-list[j].y)+(list[i].x-list[j].x)*(list[i].x-list[j].x) < tw2) ++i;
      double a = atan(-(list[i].y-list[j].y)/double(list[i].x-list[j].x));
      tw = wxCoord(th * sin(a));
      th = wxCoord(th * cos(a));
      a *= 57.3;
      dc.DrawRotatedText( ttext, list[j].x-tw, list[j].y-th-3, a);
    }
    else if (text_pos == CURVE_LEFT_DOWN)
    {
      if (j == s-1)
          j = s-2;
      int i = j+1;
      while (i<s-1&&(list[i].y-list[j].y)*(list[i].y-list[j].y)+(list[i].x-list[j].x)*(list[i].x-list[j].x) < tw2) ++i;
      double a = atan(-(list[i].y-list[j].y)/double(list[i].x-list[j].x));
      a *= 57.3;
      dc.DrawRotatedText( ttext, list[j].x, list[j].y+3, a);
    }
    else if (text_pos == CURVE_RIGHT_UP)
    {
      if (j == 0)
          j = 1;
      int i = j-1;
      while (i>0&&(list[i].y-list[j].y)*(list[i].y-list[j].y)+(list[i].x-list[j].x)*(list[i].x-list[j].x) < tw2) --i;
      double a = atan(-(list[i].y-list[j].y)/double(list[i].x-list[j].x));
      tw = wxCoord(th * sin(a));
      th = wxCoord(th * cos(a));
      a *= 57.3;
      dc.DrawRotatedText( ttext, list[i].x-tw, list[i].y-th-3, a);
    }
    else if (text_pos == CURVE_RIGHT_DOWN)
    {
      if (j == 0)
          j = 1;
      int i = j-1;
      while (i>0&&(list[i].y-list[j].y)*(list[i].y-list[j].y)+(list[i].x-list[j].x)*(list[i].x-list[j].x) < tw2) --i;
      double a = atan(-(list[i].y-list[j].y)/double(list[i].x-list[j].x));
      a *= 57.3;
      dc.DrawRotatedText( ttext, list[i].x, list[i].y+3, a);
    }
  }
}

void AxisDirMap(AXISDIR axis_dir, int &dirx, int &diry, int &posx, int &posy, int &rangex, int &rangey, bool & exchange, const wxCoord &x, const wxCoord &y, const wxCoord &w, const wxCoord &h)
{
  switch (axis_dir)
  {
      case X_RIGHT_Y_UP :
          exchange = false;
          dirx = 1;
          rangex = w;
          posx = x;
          diry = -1;
          rangey = h;
          posy = y+h;
          break;
      case X_RIGHT_Y_DOWN :
          exchange = false;
          dirx = 1;
          rangex = w;
          posx = x;
          diry = 1;
          rangey = h;
          posy = y;
          break;
      case X_LEFT_Y_UP :
          exchange = false;
          dirx = -1;
          rangex = w;
          posx = x+w;
          diry = -1;
          rangey = h;
          posy = y+h;
          break;
      case X_LEFT_Y_DOWN :
          exchange = false;
          dirx = -1;
          rangex = w;
          posx = x+w;
          diry = 1;
          rangey = h;
          posy = y;
          break;
      case X_UP_Y_LEFT :
          exchange = true;
          dirx = -1;
          rangex = h;
          posx = y+h;
          diry = -1;
          rangey = w;
          posy = x+w;
          break;
      case X_UP_Y_RIGHT :
          exchange = true;
          dirx = -1;
          rangex = h;
          posx = y+h;
          diry = 1;
          rangey = w;
          posy = x;
          break;
      case X_DOWN_Y_LEFT :
          exchange = true;
          dirx = 1;
          rangex = h;
          posx = y;
          diry = -1;
          rangey = w;
          posy = x+w;
          break;
      case X_DOWN_Y_RIGHT :
          exchange = true;
          dirx = 1;
          rangex = h;
          posx = y;
          diry = 1;
          rangey = w;
          posy = x;
          break;          
  }
}

void Curve::DrawBSpline(wxDC& dc, wxCoord x, wxCoord y, wxCoord w, wxCoord h, double minx, double maxx, double miny, double maxy, AXISDIR axis_dir)
{
  //init
  // Calculate a suitable scaling factor
  double scaleX=(double)(ppi.GetWidth()/ppi_x);
  double scaleY=(double)(ppi.GetHeight()/ppi_y);
  // Use x or y scaling factor, whichever fits on the DC
  double actualScale = wxMin(scaleX,scaleY);
  // Set the scale
  wxPen tmp_pen = pen;
  tmp_pen.SetWidth(int(pen.GetWidth()*actualScale+0.5));
  dc.SetPen(tmp_pen);
  dc.SetBrush(wxBrush(tmp_pen.GetColour()));

  int dirx, diry, posx, posy, rangex, rangey;
  bool exchange;
  AxisDirMap(axis_dir, dirx, diry, posx, posy, rangex, rangey, exchange, x, y, w, h);

  //make point list
  double sx = maxx-minx;
  double sy = maxy-miny;
  wxCoord tmpx, tmpy, tmpx_old=-9999, tmpy_old=-9999;
  int s = nX();
  wxPoint * list = new wxPoint[s];
  int j = 0;
  bool flag = true;
  wxCoord cache_x, cache_y;
  for (int i = 0; i<s; ++i)
  {
      if (X(i)>maxx)
      {
          s = i;
          break;
      }
      tmpx = int((X(i)-minx)/sx*rangex)*dirx+posx;
      tmpy = int((Y(i)-miny)/sy*rangey)*diry+posy;
      if (tmpx_old != -9999)
      {
        if ((tmpx-tmpx_old)*(tmpx-tmpx_old)+(tmpy-tmpy_old)*(tmpy-tmpy_old) <= 4)
	  continue;
      }
      tmpx_old = tmpx;
      tmpy_old = tmpy;
      if (exchange)
      {
        list[j].x = tmpy;
        list[j].y = tmpx;
      }
      else
      {
        list[j].x = tmpx;
        list[j].y = tmpy;      
      }
      if (list[j].x >= x && list[j].x <= x+w && list[j].y >= y && list[j].y <= y+h )
      {
        ++j;
	flag = true;
      }
      else
      {
        if (flag == true)
	{
	  cache_x = list[j].x;
	  cache_y = list[j].y;
	}
        flag = false;
      }
  }

  if (flag == false)
  {
      list[j].x = cache_x;
      list[j].y = cache_y;
      if (list[j].x < x)
      {
	double tmp = double(list[j-1].x-x)/double(x-list[j].x);
        list[j].y = int((tmp * list[j].y + list[j-1].y)/(1+tmp));
        list[j].x = x;
      }
      else if (list[j].x > x+w)
      {
	double tmp = double(x+w-list[j-1].x)/double(list[j].x-x-w);
        list[j].y = int( (tmp * list[j].y + list[j-1].y)/(tmp+1) );
        list[j].x = x+w;
      }

      if (list[j].y < y)
      {
	double tmp = double(list[j-1].y-y)/double(y-list[j].y);
        list[j].x = int( (tmp * list[j].x + list[j-1].x)/(1+tmp) );
        list[j].y = y;
      }
      else if (list[j].y > y+h)
      {
	double tmp = double(y+h-list[j-1].y) / double(list[j].y-y-h);
        list[j].x =  int( (tmp * list[j].x + list[j-1].x)/(1+tmp) );
        list[j].y = y+h;
      }
      /*
      if (list[j].x >= x && list[j].x <= x+w && list[j].y >= y && list[j].y <= y+h )
      {
      }
      */
      ++j;
  }

  //draw curve text.
  DrawText(dc, list, j);

  //draw it.
  dc.DrawLines(j, list);
  delete []list;

  //draw symbol points
  wxPen tmp2_pen = *wxBLACK_PEN;
  tmp2_pen.SetWidth(int(tmp2_pen.GetWidth()*actualScale+0.5));
  dc.SetPen(tmp2_pen);
  s = nNode();
  for (int i = 0; i<s; ++i)
  {
      tmpx = int((xNode(i)-minx)/sx*rangex)*dirx+posx;
      tmpy = int((yNode(i)-miny)/sy*rangey)*diry+posy;
      if (exchange && tmpy >= x && tmpy <= x+w && tmpx >= y && tmpx <= y+h)
      {
        draw_symbol(dc, tmpy, tmpx, symbol, is_symbol_filled);
      }
      else if ( tmpx >= x && tmpx <= x+w && tmpy >= y && tmpy <= y+h)
      {
        draw_symbol(dc, tmpx, tmpy, symbol, is_symbol_filled);
      }
  }
}

void Curve::DrawPolyline(wxDC& dc, wxCoord x, wxCoord y, wxCoord w, wxCoord h, double minx, double maxx, double miny, double maxy, AXISDIR axis_dir)
{
  //init
  // Calculate a suitable scaling factor
  double scaleX=(double)(ppi.GetWidth()/ppi_x);
  double scaleY=(double)(ppi.GetHeight()/ppi_y);
  // Use x or y scaling factor, whichever fits on the DC
  double actualScale = wxMin(scaleX,scaleY);
  // Set the scale
  wxPen tmp_pen = pen;
  tmp_pen.SetWidth(int(pen.GetWidth()*actualScale+0.5));
  dc.SetPen(tmp_pen);
  dc.SetBrush(wxBrush(tmp_pen.GetColour()));

  int dirx, diry, posx, posy, rangex, rangey;
  bool exchange;
  AxisDirMap(axis_dir, dirx, diry, posx, posy, rangex, rangey, exchange, x, y, w, h);

  //make point list
  double sx = maxx-minx;
  double sy = maxy-miny;
  wxCoord tmpx, tmpy;
  int s = nNode();
  wxPoint * list = new wxPoint[s];
  int j = 0;
  bool flag = true;
  wxCoord cache_x, cache_y;
  for (int i = 0; i<s; ++i)
  {
      tmpx = int((xNode(i)-minx)/sx*rangex)*dirx+posx;
      tmpy = int((yNode(i)-miny)/sy*rangey)*diry+posy;
      if (exchange)
      {
        list[j].x = tmpy;
        list[j].y = tmpx;
      }
      else
      {
        list[j].x = tmpx;
        list[j].y = tmpy;      
      }
      if (list[j].x >= x && list[j].x <= x+w && list[j].y >= y && list[j].y <= y+h )
      {
        ++j;
	flag = true;
      }
      else
      {
        if (flag == true)
	{
	  cache_x = list[j].x;
	  cache_y = list[j].y;
	}
        flag = false;
      }
  }

  if (flag == false)
  {
      list[j].x = cache_x;
      list[j].y = cache_y;
      if (list[j].x < x)
      {
	double tmp = double(list[j-1].x-x)/double(x-list[j].x);
        list[j].y = int( (tmp * list[j].y + list[j-1].y)/(1+tmp) );
        list[j].x = x;
      }
      else if (list[j].x > x+w)
      {
	double tmp = double(x+w-list[j-1].x)/double(list[j].x-x-w);
        list[j].y =  int( (tmp * list[j].y + list[j-1].y)/(tmp+1) );
        list[j].x = x+w;
      }

      if (list[j].y < y)
      {
	double tmp = double(list[j-1].y-y)/double(y-list[j].y);
        list[j].x = int( (tmp * list[j].x + list[j-1].x)/(1+tmp) );
        list[j].y = y;
      }
      else if (list[j].y > y+h)
      {
	double tmp = double(y+h-list[j-1].y) / double(list[j].y-y-h);
        list[j].x = int( (tmp * list[j].x + list[j-1].x)/(1+tmp) );
        list[j].y = y+h;
      }
      /*
      if (list[j].x >= x && list[j].x <= x+w && list[j].y >= y && list[j].y <= y+h )
      {
      }
      */
      ++j;
  }


  //draw curve text.
  DrawText(dc, list, j);

  //draw it.
  dc.DrawLines(j, list);

  //draw symbol points
  wxPen tmp2_pen = *wxBLACK_PEN;
  tmp2_pen.SetWidth(int(tmp2_pen.GetWidth()*actualScale+0.5));
  dc.SetPen(tmp2_pen);
  for (int i = 0; i<j; ++i)
  {
      draw_symbol(dc, list[i].x, list[i].y, symbol, is_symbol_filled);
  }
  
  delete []list;
}

void Curve::DrawPoints(wxDC& dc, wxCoord x, wxCoord y, wxCoord w, wxCoord h, double minx, double maxx, double miny, double maxy, AXISDIR axis_dir)
{
  //init
  // Calculate a suitable scaling factor
  double scaleX=(double)(ppi.GetWidth()/ppi_x);
  double scaleY=(double)(ppi.GetHeight()/ppi_y);
  // Use x or y scaling factor, whichever fits on the DC
  double actualScale = wxMin(scaleX,scaleY);
  wxPen tmp_pen = *wxBLACK_PEN;
  tmp_pen.SetWidth(int(tmp_pen.GetWidth()*actualScale+0.5));
  dc.SetPen(tmp_pen);
  dc.SetBrush(wxBrush(pen.GetColour()));

  int dirx, diry, posx, posy, rangex, rangey;
  bool exchange;
  AxisDirMap(axis_dir, dirx, diry, posx, posy, rangex, rangey, exchange, x, y, w, h);

  //make point list
  double sx = maxx-minx;
  double sy = maxy-miny;
  wxCoord tmpx, tmpy;
  int s = nNode();
  for (int i = 0; i<s; ++i)
  {
      tmpx = int((xNode(i)-minx)/sx*rangex)*dirx+posx;
      tmpy = int((yNode(i)-miny)/sy*rangey)*diry+posy;
      if (exchange && tmpy >= x && tmpy <= x+w && tmpx >= y && tmpx <= y+h)
      {
        draw_symbol(dc, tmpy, tmpx, symbol, is_symbol_filled);
      }
      else  if ( tmpx >= x && tmpx <= x+w && tmpy >= y && tmpy <= y+h)
      {
        draw_symbol(dc, tmpx, tmpy, symbol, is_symbol_filled);
      }
  }
}

void Curve::DrawCurve(wxDC& dc, wxCoord x, wxCoord y, wxCoord w, wxCoord h, double minx, double maxx, double miny, double maxy, AXISDIR axis_dir)
{
  if (!isHide)
  switch (type)
  {
    case CURVE_BSPLINE:
      DrawBSpline(dc, x, y, w, h, minx, maxx, miny, maxy, axis_dir);
      break;
    case CURVE_POLYLINE:
      DrawPolyline(dc, x, y, w, h, minx, maxx, miny, maxy, axis_dir);
      break;
  }
}

void Curve::Load(Element * element)
{
    Element * tmp = element->FindElement("painter");
    LoadPainter(tmp, &pen, &fnt);

    wxString t = std2wxstr(element->FindElementValue("type"));
    t.UpperCase();
    if (t.Find(wxT("BSPLINE")) != -1)
    {
        type = CURVE_BSPLINE;
    }
    else
    {
        type = CURVE_POLYLINE;
    }

    wxString h = std2wxstr(element->FindElementValue("hide"));
    h.UpperCase();
    if (h.Find(wxT("TRUE")) != -1)
    {
        isHide = true;
    }
    else
    {
        isHide = false;
    }

    if ((tmp = element->FindElement("symbol")))
    {
        wxString f = std2wxstr(tmp->GetAttributeValue("filled"));
        f.LowerCase();
        if (f.Find(wxT("true")) != -1 )
        {
          is_symbol_filled = true;
        }
        else
        {
          is_symbol_filled = false;
        }
        wxString s = std2wxstr(tmp->GetDataLine());
        s.UpperCase();
        if (s.Find(wxT("REC")) != -1)
        {
          symbol = CURVE_REC;
        }
        else if (s.Find(wxT("TRI_DOWN")) != -1)
        {
          symbol = CURVE_TRI_DOWN;
        }
        else if (s.Find(wxT("TRI_UP")) != -1)
        {
          symbol = CURVE_TRI_UP;
        }
        else if (s.Find(wxT("TRI_LEFT")) != -1)
        {
          symbol = CURVE_TRI_LEFT;
        }
        else if (s.Find(wxT("TRI_RIGHT")) != -1)
        {
          symbol = CURVE_TRI_RIGHT;
        }
        else if (s.Find(wxT("DIAMOND")) != -1)
        {
          symbol = CURVE_DIAMOND;
        }
        else if (s.Find(wxT("PENTAGON")) != -1)
        {
          symbol = CURVE_PENTAGON;
        }
        else if (s.Find(wxT("HEXAGON")) != -1)
        {
          symbol = CURVE_HEXAGON;
        }
        else
        {
          symbol = CURVE_NONE;
        }
    }
    if ((tmp = element->FindElement("text")))
    {
        wxString p = std2wxstr(tmp->GetAttributeValue("pos"));
        p.LowerCase();
        if (p.Find(wxT("left_up")) != -1 )
        {
          text_pos = CURVE_LEFT_UP;
        }
        else if (p.Find(wxT("left_down")) != -1 )
        {
          text_pos = CURVE_LEFT_DOWN;
        }
        else if (p.Find(wxT("right_down")) != -1 )
        {
          text_pos = CURVE_RIGHT_DOWN;
        }
        else if (p.Find(wxT("right_up")) != -1 )
        {
          text_pos = CURVE_RIGHT_UP;
        }
        else
        {
          text_pos = CURVE_NO_TEXT;
        }
        label = std2wxstr(tmp->GetDataLine());
        p = std2wxstr(tmp->GetAttributeValue("type"));
        p.LowerCase();
        label_pos = CURVE_LABEL_NONE;
        if (p.Find(wxT("prefix")) != -1)
        {
          label_pos = CURVE_LABEL_PREFIX;
        }
        else if (p.Find(wxT("suffix")) != -1)
        {
          label_pos = CURVE_LABEL_SUFFIX;
        }
	else if (p.Find(wxT("replace")) != -1)
	{
          label_pos = CURVE_LABEL_REPLACE;
	}
	double v = tmp->GetAttributeValueAsNumber("value");
	if (v <1 && v > 0)
	{
	  text_value = v;
	}
	int l = (int)tmp->GetAttributeValueAsNumber("legend");
	if (l < 32 && l > -2)
	{
	  legend_pos = l;
	}
	else
	{
	  legend_pos = 0;
	}
    }
}

void Curve::Export(wxTextOutputStream & tstream, const wxString & prefix, const unsigned & n, bool reverse) const
{
  if (n < 2u)
  {
    if (reverse)
    {
      tstream << prefix << wxT("<rcurve>\n");
    }
    else
    {
      tstream << prefix << wxT("<curve>\n");
    }
  }
  else
  {
    if (reverse)
    {
      tstream << prefix << wxT("<rcurve n=\"") << n << wxT("\">\n");
    }
    else
    {
      tstream << prefix << wxT("<curve n=\"") << n << wxT("\">\n");
    }
  }
  {
    wxString pre = prefix + wxT("    ");
    tstream << pre << _("<!-- the font and colour for the curve -->\n");
    ExportPainter(tstream, pre, pen, fnt);
    tstream << pre << _("<!-- the type of the curve. (BSPLINE or POLYLINE) -->\n");
    switch (type)
    {
      case CURVE_BSPLINE :
        tstream << pre << wxT("<type> BSPLINE </type>\n");
        break;
      case CURVE_POLYLINE :
        tstream << pre << wxT("<type> POLYLINE </type>\n");
        break;
    }
    tstream << pre << _("<!-- show or hide. (FALSE or TRUE) -->\n");
    if (isHide)
    {
      tstream << pre << wxT("<hide> TRUE </hide>\n");
    }
    else
    {
      tstream << pre << wxT("<hide> FALSE </hide>\n");
    }
    tstream << pre << _("<!-- the symbol on the curve. The 'filled' attribute can be 'true' or 'false'. -->\n");
    tstream << pre << _("<!-- the type of symbol: -->\n");
    tstream << pre << wxT("<!--     rec, tri_up, tri_down, tri_left, tri_right -->\n");
    tstream << pre << wxT("<!--     diamond, pentagon, hexagon, none -->\n");
    if (is_symbol_filled)
    {
      tstream << pre << wxT("<symbol filled=\"true\"> ");
    }
    else
    {
      tstream << pre << wxT("<symbol filled=\"false\"> ");
    }
    switch(symbol)
    {
      case CURVE_REC :
        tstream << wxT("REC");
        break;
      case CURVE_TRI_DOWN :
        tstream << wxT("TRI_DOWN");
        break;
      case CURVE_TRI_UP :
        tstream << wxT("TRI_UP");
        break;
      case CURVE_TRI_LEFT :
        tstream << wxT("TRI_LEFT");
        break;
      case CURVE_TRI_RIGHT :
        tstream << wxT("TRI_RIGHT");
        break;
      case CURVE_DIAMOND :
        tstream << wxT("DIAMOND");
        break;
      case CURVE_PENTAGON :
        tstream << wxT("PENTAGON");
        break;
      case CURVE_HEXAGON :
        tstream << wxT("HEXAGON");
        break;
      default :
        tstream << wxT("NONE");
        break;
    }
    tstream << wxT("</symbol>\n");
    tstream << pre << _("<!-- the mark on the curve. -->\n");
    tstream << pre << _("<!-- 'pos' attribute can be one of 'left_up', 'left_down', 'right_up', 'right_down', 'none'. -->\n");
    tstream << pre << _("<!-- 'type' attribute can be one of 'prefix', 'suffix', 'replace', 'none'. -->\n");
    tstream << pre << _("<!-- 'value' attribute is a number in [0,1] stands for the position. -->\n");
    tstream << pre << _("<!-- 'legend' attribute is a number stands for the legend number starts from 0. -->\n");
    tstream << pre << wxT("<text pos=\"");
    switch (text_pos)
    {
      case CURVE_LEFT_UP :
        tstream << wxT("left_up");
        break;
      case CURVE_RIGHT_UP :
        tstream << wxT("right_up");
        break;
      case CURVE_LEFT_DOWN :
        tstream << wxT("left_down");
        break;
      case CURVE_RIGHT_DOWN :
        tstream << wxT("right_down");
        break;
      default :
        tstream << wxT("none");
        break;
    }
    tstream << wxT("\" type=\"");
    switch (label_pos)
    {
      case CURVE_LABEL_PREFIX :
        tstream << wxT("prefix");
        break;
      case CURVE_LABEL_SUFFIX :
        tstream << wxT("suffix");
        break;
      case CURVE_LABEL_REPLACE :
        tstream << wxT("replace");
        break;
      default :
        tstream << wxT("none");
        break;
    }
    tstream << wxT("\" value=\"");
    tstream << text_value;
    tstream << wxT("\" legend=\"");
    tstream << legend_pos;
    tstream << wxT("\">");
    tstream << label;
    tstream << wxT("</text>\n");
  }
    if (reverse)
    {
      tstream << prefix << wxT("</rcurve>\n");
    }
    else
    {
      tstream << prefix << wxT("</curve>\n");
    }
}

bool Curve::operator != (const Curve & c) const
{
  if (pen != c.pen)
    return true;
  if (fnt != c.fnt)
    return true;
  if (type != c.type)
    return true;
  if (symbol != c.symbol)
    return true;
  if (text_pos != c.text_pos)
    return true;
  if (label_pos != c.label_pos)
    return true;
  if (text_value != c.text_value)
    return true;
  if (legend_pos != c.legend_pos)
    return true;
  if (label != c.label)
    return true;
  if (is_symbol_filled != c.is_symbol_filled)
    return true;
  if (isHide != c.isHide)
    return true;

  return false;
}


wxString Curve::GetWholeText() const
{
        if (label_pos == CURVE_LABEL_PREFIX)
        {
            return label + wxT(" ") + text;
        }
        else if (label_pos == CURVE_LABEL_SUFFIX)
        {
            return text + wxT(" ") + label;
        }
	else if (label_pos == CURVE_LABEL_REPLACE)
	{
            return label;	
	}
	else
	{
	    return text;
	}
}

PosMgr::PosMgr() :
    pos_x(0),
    pos_y(0),
    size_h(100),
    size_w(100),
    is_show_grid(true),
    grid_width(50),
    grid_height(50),
    sub_grid_x(2),
    sub_grid_y(2),
    grid_x(0u),
    grid_y(0u),
    pen_grid(wxPen(wxT("grey"), 1, wxSOLID)),
    pen_sub_grid(wxPen(wxT("grey"), 1, wxDOT))
{}

bool PosMgr::Load(Element * element)
{
    Element * tmp = NULL;
    if ((tmp = element->FindElement("grid")))
    {
        wxString show = std2wxstr(tmp->FindElementValue("show"));
        show.LowerCase();
        if (show.Find(wxT("true")) != -1)
        {
          is_show_grid = true;
        }
        else
        {
          is_show_grid = false;
        }
        double wh = tmp->FindElementValueAsNumber("width");
        if (wh != 99e99)
        {
          grid_width = (unsigned int)wh;
        }
        wh = tmp->FindElementValueAsNumber("height");
        if (wh != 99e99)
        {
          grid_height = (unsigned int)wh;
        }
        double sub = tmp->FindElementValueAsNumber("subgridx");
        if (sub > 200 || sub < 0)
        {
            sub_grid_x = 0u;
        }
        else
        {
            sub_grid_x = (unsigned)sub;
        }
        sub = tmp->FindElementValueAsNumber("subgridy");
        if (sub > 200 || sub < 0)
        {
            sub_grid_y = 0u;
        }
        else
        {
            sub_grid_y = (unsigned)sub;
        }
        sub = tmp->FindElementValueAsNumber("gridx");
        if (sub > 200 || sub < 0)
        {
            grid_x = 0u;
        }
        else
        {
            grid_x = (unsigned)sub;
        }
        sub = tmp->FindElementValueAsNumber("gridy");
        if (sub > 200 || sub < 0)
        {
            grid_y = 0u;
        }
        else
        {
            grid_y = (unsigned)sub;
        }
        LoadPen(tmp->FindElement("grid_painter"), pen_grid);
        LoadPen(tmp->FindElement("sub_grid_painter"), pen_sub_grid);
    }
    else
    {
      is_show_grid = false;
    }
    
    div_list.clear();
    tmp = element->FindElement("div");
    if (!tmp)
    {
      return false;
    }
    while (tmp)
    {
      wxString col = std2wxstr(tmp->FindElementValue("col"));
      wxString row = std2wxstr(tmp->FindElementValue("row"));
      PosMap div;
      readNumsfromString(div.col_list, col);
      readNumsfromString(div.row_list, row);
      div.cached = false;
      if (div.col_list.empty() || div.row_list.empty())
      {
        return false;
      }
      div_list.push_back(div);
      tmp = element->FindNextElement("div");
    }
    
    return true;
}


void PosMgr::Export(wxTextOutputStream & tstream, const wxString & prefix) const
{
  tstream << prefix << wxT("<pos_mgr>\n");
  {
   wxString pre = prefix + wxT("    ");
   tstream << pre << _("<!-- Grid Setting -->\n");
   tstream << pre << wxT("<grid>\n");
   {
    wxString pre1 = pre + wxT("    ");
    tstream << pre1 << _("<!-- show the grid or not (true or false) -->\n");
    tstream << pre1 << wxT("<show> ");
    if (is_show_grid)
    {
      tstream << wxT("true");
    }
    else
    {
      tstream << wxT("false");
    }
    tstream << wxT(" </show>\n");
    tstream << pre1 << _("<!-- the width of grid (deprecated) -->\n");
    tstream << pre1 << wxT("<width> ") << grid_width << wxT(" </width>\n");
    tstream << pre1 << _("<!-- the height of grid (deprecated) -->\n");
    tstream << pre1 << wxT("<height> ") << grid_height << wxT(" </height>\n");
    tstream << pre1 << _("<!-- the column number of grid  -->\n");
    tstream << pre1 << wxT("<gridx> ") << grid_x << wxT(" </gridx>\n");
    tstream << pre1 << _("<!-- the row number of grid  -->\n");
    tstream << pre1 << wxT("<gridy> ") << grid_y << wxT(" </gridy>\n");
    tstream << pre1 << _("<!-- the column number of sub grid in one grid -->\n");
    tstream << pre1 << wxT("<subgridx> ") << sub_grid_x << wxT(" </subgridx>\n");
    tstream << pre1 << _("<!-- the row number of sub grid in one grid -->\n");
    tstream << pre1 << wxT("<subgridy> ") << sub_grid_y << wxT(" </subgridy>\n");
    tstream << pre1 << _("<!-- the painter setting for the grid -->\n");
    ExportPen(tstream, pre1, pen_grid, wxT("grid_painter"));
    tstream << pre1 << _("<!-- the painter setting for the sub grid -->\n");
    ExportPen(tstream, pre1, pen_sub_grid, wxT("sub_grid_painter"));
   }
   tstream << pre << wxT("</grid>\n");
   tstream << pre << wxT("<!-- divide the grid in different group(s) -->\n");
   for (vector<PosMap>::const_iterator i=div_list.begin(); i != div_list.end(); ++i)
   {
     tstream << pre << wxT("<div>\n");
     {
       wxString pre1 = pre + wxT("    ");
       tstream << pre1 << wxT("<!-- divide column. The column number is the number of these numbers. Each number stands for the power of width.-->\n");
       tstream << pre1 << wxT("<col> ");
       for (vector<wxUint32>::const_iterator j=i->col_list.begin(); j != i->col_list.end(); ++j)
       {
         tstream << *j << wxT(" ");
       }
       tstream << wxT("</col>\n");
       tstream << pre1 << wxT("<!-- divide row. The row number is the number of these numbers. Each number stands for the power of width.-->\n");
       tstream << pre1 << wxT("<row> ");
       for (vector<wxUint32>::const_iterator j=i->row_list.begin(); j != i->row_list.end(); ++j)
       {
         tstream << *j << wxT(" ");
       }
       tstream << wxT("</row>\n");
     }
     tstream << pre << wxT("</div>\n");
   }
  }
  tstream << prefix << wxT("</pos_mgr>\n");
}

void PosMgr::GetPos(const size_t &r, const size_t &c, wxCoord &x, wxCoord &y, size_t &w, size_t &h, const unsigned int &no, wxDC* dc)
{
  if (r >= div_list[no].row_list.size())
  {
    throw "row pos is out of range";
  }
  if (c >= div_list[no].col_list.size())
  {
    throw "col pos is out of range";
  }

  if (!div_list[no].cached)
  {
    cal(dc, no);
  }
  x = div_list[no].col_pos[c];
  w = div_list[no].col_size[c];
  y = div_list[no].row_pos[r];
  h = div_list[no].row_size[r];
}

void PosMgr::ExpireCache(void)
{ 
  for (vector<PosMap>::iterator i = div_list.begin(); i != div_list.end(); ++i)
  {
    i->cached = false;
  }
}
  
void PosMgr::SetX(const wxCoord &x)
{ 
  pos_x = x; 
  ExpireCache();
}
  
void PosMgr::SetY(const wxCoord &y)
{ 
  pos_y = y;
  ExpireCache();
}

void PosMgr::SetW(const size_t &w) 
{ 
  size_w = w;
  ExpireCache();
}

void PosMgr::SetH(const size_t &h) 
{ 
  size_h = h;
  ExpireCache();
}

void PosMgr::cal(wxDC* dc, const unsigned int &no)
{
  if (no >= div_list.size())
  {
    throw "div pos is out of range";
  }


  PosMap & div = div_list[no];
  div.row_pos.clear();
  div.col_pos.clear();
  div.row_size.clear();
  div.col_size.clear();

  double sum=0;
  for (vector<wxUint32>::iterator i = div.row_list.begin(); i != div.row_list.end(); ++i)
  {
    sum += *i;
  }
  double delta = size_h / sum;
  div.row_size.resize(div.row_list.size(), 0);
  double h = size_h;
  while (h > 0)
  {
    vector<wxUint32> dd =  div.row_list;
    bool flag = true;
    while (flag && h > 0)
    {
    vector<wxUint32>::iterator i = dd.begin();
    vector<wxUint32>::iterator j = div.row_size.begin();
    flag = false;
    for (; i != dd.end(); ++i, ++j)
    {
      if ( *i > 0)
      {
	flag = true;
        h -= height;
	--(*i);
        if (h < 0)
        {
          *j += (unsigned int)floor((h+height)/(double)height);
          break;
        }
        *j += (unsigned int)height;
      }
    }
    }
  }
  double tmp = pos_y;
  for (vector<wxUint32>::iterator i = div.row_size.begin(); i != div.row_size.end(); ++i)
  {
    div.row_pos.push_back((wxCoord)tmp);
    tmp += *i;
  }
  
  sum=0;
  for (vector<wxUint32>::iterator i = div.col_list.begin(); i != div.col_list.end(); ++i)
  {
    sum += *i;
  }
  delta = size_w / sum;
  div.col_size.resize(div.col_list.size(), 0);
  double w = size_w;
  while (w > 0)
  {
    vector<wxUint32> dd =  div.col_list;
    bool flag = true;
    while (flag && w > 0)
    {
    vector<wxUint32>::iterator i = dd.begin();
    vector<wxUint32>::iterator j = div.col_size.begin();
    flag = false;
    for (; i != dd.end(); ++i, ++j)
    {
      if ( *i > 0)
      {
	flag = true;
        w -= width;
	--(*i);
        if (w < 0)
        {
          *j += (unsigned int)floor((w+width)/(double)width);
          break;
        }
        *j += (unsigned int)width;
      }
    }
    }
  }
  tmp = pos_x;
  for (vector<wxUint32>::iterator i = div.col_size.begin(); i != div.col_size.end(); ++i)
  {
    div.col_pos.push_back((wxCoord)tmp);
    tmp += *i;
  }
 
  div.cached = true;
}

void PosMgr::DrawGrid(wxDC& dc)
{
    if (!is_show_grid)
        return;

    // Calculate a suitable scaling factor
    double scaleX=(double)(ppi.GetWidth()/ppi_x);
    double scaleY=(double)(ppi.GetHeight()/ppi_y);
    // Use x or y scaling factor, whichever fits on the DC
    double actualScale = wxMin(scaleX,scaleY);
    // Set the scale
    if (grid_x == 0u)
    {
      grid_x = size_w / grid_width;
    }
    else
    {
      grid_width = size_w / grid_x;
    }
    if (grid_y == 0u)
    {
      grid_y = size_h / grid_height;
    }
    else
    {
      grid_height = size_h / grid_y;
    }
    width = grid_width;
    height = grid_height;
    size_w = width * grid_x;
    size_h = height * grid_y;

    // Set the scale
    wxPen tmp_pen = pen_grid;
    tmp_pen.SetWidth(int(pen_grid.GetWidth()*actualScale+0.5));
    dc.SetPen(tmp_pen);
    
    const wxCoord &top = pos_y;
    const wxCoord &left = pos_x;
    const wxCoord bottom = pos_y+size_h;
    const wxCoord right  = pos_x+size_w;

    dc.DrawLine(left, top, left, bottom);
    wxCoord x = left+width;
    while ( x < right)
    {
      dc.DrawLine(x, top, x, bottom);
      x += width;
    }
    dc.DrawLine(right, top, right, bottom);

    dc.DrawLine(left, top, right, top);
    wxCoord y = top+height;
    while ( y < bottom)
    {
      dc.DrawLine(left, y, right, y);
      y += height;
    }
    dc.DrawLine(left, bottom, right, bottom);

    wxPen tmp2_pen = pen_sub_grid;
    tmp2_pen.SetWidth(int(pen_sub_grid.GetWidth()*actualScale+0.5));
    dc.SetPen(tmp2_pen);
    
    if (sub_grid_x > 0)
    {
      wxCoord x = left;
      double sdx = width / (double)sub_grid_x;
      while ( x < right)
      {
        double sx = x+sdx;
        for (unsigned i = 1u; i < sub_grid_x; ++i)
	{
          dc.DrawLine((wxCoord)sx, top, (wxCoord)sx, bottom);
	  sx += sdx;
	}
	x += width;
      }
    }

    if (sub_grid_y > 0)
    {
      wxCoord y = top;
      double sdy = height / (double)sub_grid_y;
      while ( y < bottom)
      {
        double sy = y+sdy;
        for (unsigned i = 1u; i < sub_grid_y; ++i)
	{
          dc.DrawLine(left, (wxCoord)sy, right, (wxCoord)sy);
	  sy += sdy;
	}
	y += height;
      }
    }
}


PlotWindow::PlotWindow(PlotHandler * h) :
  row_pos(0),
  col_pos(0),
  mgr_pos(0),
  axis_dir(X_RIGHT_Y_UP),
  fnt_axis(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)),
  pen_axis(*wxBLACK),
  cur_curve(NULL),
  is_combine_curves(false),
  combine_curves_distance(99e99),
  axis_x_pos(HIGH_OUT),
  axis_x_hide(false),
  auto_x_space(true),
  max_x(1),
  min_x(0),
  x_mark_mode(0),
  x_mark_interval(0.1),
  x_info_from(-1),
  str_x_label(_("x")),
  axis_y_pos(LOW_OUT),
  axis_y_hide(false),
  auto_y_space(true),
  max_y(1),
  min_y(0),
  y_mark_mode(0),
  y_mark_interval(0.1),
  str_y_label(_("y")),
  handler(h),
  strange_mode(false),
  x_pos_strange_mode(0)
{
  Legend tmp;
  tmp.show = true;
  tmp.value_x = 0.5;
  tmp.pos_x = 0.7;
  tmp.pos_y = 0.5;
  tmp.fnt = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
  tmp.pen = *wxBLACK;
  tmp.align_left = true;
  tmp.indicate_left = true;
  legend_list.push_back(tmp);
}

PlotWindow::PlotWindow(PlotHandler * h, Element * element) :
  row_pos(0),
  col_pos(0),
  mgr_pos(0),
  axis_dir(X_RIGHT_Y_UP),
  fnt_axis(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)),
  pen_axis(*wxBLACK),
  cur_curve(NULL),
  axis_x_pos(HIGH_OUT),
  axis_x_hide(false),
  auto_x_space(true),
  max_x(1),
  min_x(0),
  x_mark_mode(0),
  x_mark_interval(0.1),
  x_info_from(-1),
  str_x_label(_("x")),
  axis_y_pos(LOW_OUT),
  axis_y_hide(false),
  auto_y_space(true),
  max_y(1),
  min_y(0),
  y_mark_mode(0),
  y_mark_interval(0.1),
  str_y_label(_("y")),
  handler(h),
  strange_mode(false),
  x_pos_strange_mode(0)
{
  Legend tmp;
  tmp.show = true;
  tmp.value_x = 0.5;
  tmp.pos_x = 0.7;
  tmp.pos_y = 0.5;
  tmp.fnt = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
  tmp.pen = *wxBLACK;
  tmp.align_left = true;
  tmp.indicate_left = true;
  legend_list.push_back(tmp);
  if (!Load(element))
  {
    throw "fail to load in for PlotWindow";
  }
}

PlotWindow::~PlotWindow()
{
  clear();
  ClearChildren();
}

bool PlotWindow::pitch_col(vector<double> &x, vector<double> &y, double &key,  vector <unsigned> & list, Table * table, Condition *data_condition, FGPropertyManager *property)
{
  x.clear();
  y.clear();
  unsigned s = table->GetSize();
  {
    double min = 99e99;
    for (unsigned i = 0u; i < s; ++i)
    {
      if ((*table)[i].GetSize() <= list[i])
         continue;
      if ((*table)[i].GetKey(list[i]) < min)
      {
        min = (*table)[i].GetKey(list[i]);
      }
    }
    if (min == 99e99)
    {
      return false;
    }
    else
    {
      key = min;
      property->SetDouble((*table)[0].GetIndepVar(), key);
      for (unsigned i = 0u; i < s; ++i)
      {
        if ((*table)[i].GetSize() == list[i])
          continue;
        if ((*table)[i].GetKey(list[i]) == min)
        {
	  double a = table->GetKey(i);
	  double f = (*table)[i].GetValue(list[i]);
          property->SetDouble(table->GetIndepVar(), a);
          property->SetDouble((*table)[i].GetName(), f);
	  if (data_condition->Evaluate())
	  {
	    x.push_back(a);
	    y.push_back(f);
	  }
	  ++list[i];
        }
      }
    }
  }
  return true;
}

bool PlotWindow::Load(Element * element)
{
    Element *tmp;
    if ((tmp = element->FindElement("pos")))
    {
      double cv;
      cv = tmp->FindElementValueAsNumber("row");
      if (cv == 99e99)
      {
        wxLogError(_("Error when setting row position. it should be in range [0,99]"));
        return false;
      }
      else
      {
        row_pos = (size_t)cv;
      }
      cv = tmp->FindElementValueAsNumber("col");
      if (cv == 99e99)
      {
        wxLogError(_("Error when setting col position. it should be in range [0,99]"));
        return false;
      }
      else
      {
        col_pos = (size_t)cv;
      }
      cv = tmp->FindElementValueAsNumber("mgr");
      if (cv == 99e99)
      {
        mgr_pos = 0u;
      }
      else
      {
        mgr_pos = (size_t)cv;
      }
    }
    else
    {
      wxLogWarning(_("unkown the position of window, trying pos (0,0)."));
      row_pos = col_pos = mgr_pos = 0u;
    }

    if ((tmp = element->FindElement("dir")))
    {
      wxString xdir = std2wxstr(tmp->FindElementValue("x"));
      xdir.LowerCase();
      wxString ydir = std2wxstr(tmp->FindElementValue("y"));
      ydir.LowerCase();
      if (xdir.Find(wxT("left")) != -1)
      {
          if (ydir.Find(wxT("up")) != -1)
          {
              axis_dir = X_LEFT_Y_UP;
          }
          else if (ydir.Find(wxT("down")) != -1)
          {
              axis_dir = X_LEFT_Y_DOWN;
          }
          else
          {
            wxLogError(_("Error occures when setting y direction"));
            return false;
          }
      }
      else if (xdir.Find(wxT("right")) != -1)
      {
          if (ydir.Find(wxT("up")) != -1)
          {
              axis_dir = X_RIGHT_Y_UP;
          }
          else if (ydir.Find(wxT("down")) != -1)
          {
              axis_dir = X_RIGHT_Y_DOWN;
          }
          else
          {
            wxLogError(_("Error occures when setting y direction"));
            return false;
          }
      }
      else if (xdir.Find(wxT("up")) != -1)
      {
          if (ydir.Find(wxT("left")) != -1)
          {
              axis_dir = X_UP_Y_LEFT;
          }
          else if (ydir.Find(wxT("right")) != -1)
          {
              axis_dir = X_UP_Y_RIGHT;
          }
          else
          {
            wxLogError(_("Error occures when setting y direction"));
            return false;
          }
      }
      else if (xdir.Find(wxT("down")) != -1)
      {
          if (ydir.Find(wxT("left")) != -1)
          {
              axis_dir = X_DOWN_Y_LEFT;
          }
          else if (ydir.Find(wxT("right")) != -1)
          {
              axis_dir = X_DOWN_Y_RIGHT;
          }
          else
          {
            wxLogError(_("Error occures when setting y direction"));
            return false;
          }
      }
      else
      {
        wxLogError(_("unkown x direction"));
        return false;
      }
    }
    else
    {
        wxLogWarning(_("unkown the direction of axis, trying X_RIGHT_Y_UP."));
        axis_dir = X_RIGHT_Y_UP;
    }
    
    if ((tmp = element->FindElement("axis")))
    {
        Element * ax;
        if ((ax = tmp->FindElement("x")))
        {
	  double from = ax->FindElementValueAsNumber("info_from");
	  if (from > 99 || from < -0.4)
	  {
	    x_info_from = -1;
	  }
	  else
	  {
	    x_info_from = (int)from;
	  }
          wxString pos = std2wxstr(ax->FindElementValue("pos"));
          pos.UpperCase();
          if (pos.Find(wxT("LOW_OUT")) != -1)
          {
            axis_x_pos = LOW_OUT;
          }
          else if (pos.Find(wxT("LOW_IN")) != -1)
          {
            axis_x_pos = LOW_IN;
          }
          else if (pos.Find(wxT("HIGH_IN")) != -1)
          {
            axis_x_pos = HIGH_IN;
          }
          else
          {
            axis_x_pos = HIGH_OUT;
          }
          wxString show = std2wxstr(ax->FindElementValue("hide"));
          show.LowerCase();
          if (show.Find(wxT("true")) != -1)
          {
            axis_x_hide = true;
          }
          else
          {
            axis_x_hide = false;
          }
          wxString autospace = std2wxstr(ax->FindElementValue("auto_space"));
          autospace.LowerCase();
          if (autospace.Find(wxT("false")) != -1)
          {
            auto_x_space = false;
          }
          else
          {
            auto_x_space = true;          
          }
          if (!auto_x_space)
          {
            max_cx = ax->FindElementValueAsNumber("max");
            min_cx = ax->FindElementValueAsNumber("min");
          }
          /**************************************************************
          wxString mode = std2wxstr(ax->FindElementValue("mode"));
          mode.LowerCase();
          if (mode.Find(wxT("list")) != -1)
          {
            x_mark_mode = 2;
          } else if (mode.Find(wxT("interval")) != -1)
          {
            x_mark_mode = 1;
          }
          else
          {
            x_mark_mode = 0;
          }
          if (x_mark_mode == 1)
          {
            x_mark_interval = ax->FindElementValueAsNumber("interval");
          }
          else if (x_mark_mode == 2)
          {
              Element * p = ax->FindElement("list");
              if (p)
              {
                  wxString list = wxEmptyString;
                  for (int i=0; i<p->GetNumDataLines(); i++)
                  {
                    list += std2wxstr(p->GetDataLine(i) + " ");
                  }
                  readNumsfromString(x_mark_list, list);
              }
              else
              {
                  wxLogError(_("Fail to find x mark list"));
              }
          }
          ****************************************/
          x_mark_mode = 0;
          str_x_label = std2wxstr(ax->FindElementValue("label"));
        }
        if ((ax = tmp->FindElement("y")))
        {
          wxString pos = std2wxstr(ax->FindElementValue("pos"));
          pos.UpperCase();
          if (pos.Find(wxT("LOW_OUT")) != -1)
          {
            axis_y_pos = LOW_OUT;
          }
          else if (pos.Find(wxT("LOW_IN")) != -1)
          {
            axis_y_pos = LOW_IN;
          }
          else if (pos.Find(wxT("HIGH_IN")) != -1)
          {
            axis_y_pos = HIGH_IN;
          }
          else
          {
            axis_y_pos = HIGH_OUT;
          }
          wxString show = std2wxstr(ax->FindElementValue("hide"));
          show.LowerCase();
          if (show.Find(wxT("true")) != -1)
          {
            axis_y_hide = true;
          }
          else
          {
            axis_y_hide = false;
          }
          wxString autospace = std2wxstr(ax->FindElementValue("auto_space"));
          autospace.LowerCase();
          if (autospace.Find(wxT("false")) != -1)
          {
            auto_y_space = false;
          }
          else
          {
            auto_y_space = true;          
          }
          if (!auto_y_space)
          {
            max_cy = ax->FindElementValueAsNumber("max");
            min_cy = ax->FindElementValueAsNumber("min");
          }
          /**************************************************************
          wxString mode = std2wxstr(ax->FindElementValue("mode"));
          mode.LowerCase();
          if (mode.Find(wxT("list")) != -1)
          {
            y_mark_mode = 2;
          } else if (mode.Find(wxT("interval")) != -1)
          {
            y_mark_mode = 1;
          }
          else
          {
            y_mark_mode = 0;
          }
          if (y_mark_mode == 1)
          {
            y_mark_interval = ax->FindElementValueAsNumber("interval");
          }
          else if (y_mark_mode == 2)
          {
              Element * p = ax->FindElement("list");
              if (p)
              {
                  wxString list = wxEmptyString;
                  for (int i=0; i<p->GetNumDataLines(); i++)
                  {
                    list += std2wxstr(p->GetDataLine(i) + " ");
                  }
                  readNumsfromString(y_mark_list, list);
              }
              else
              {
                  wxLogError(_("Fail to find y mark list"));
              }
          }
          *************************************************************/
          y_mark_mode = 0;
          str_y_label = std2wxstr(ax->FindElementValue("label"));
        }
        LoadPainter(tmp->FindElement("painter"), &pen_axis, &fnt_axis);
    }
    else
    {
        wxLogWarning(_("No info about the axis, trying default values."));
    }
    
    if ((tmp = element->FindElement("legend")))
    {
        wxString show = std2wxstr(tmp->FindElementValue("show"));
        show.LowerCase();
        if (show.Find(wxT("true")) != -1)
        {
          legend_list[0].show = true;
        }
        else
        {
          legend_list[0].show = false;
        }
        double sub = tmp->FindElementValueAsNumber("value");
        if (sub < 9999)
        {
            legend_list[0].value_x = sub;
        }
        sub = tmp->FindElementValueAsNumber("x");
        if (sub < 9999)
        {
            legend_list[0].pos_x  = sub;
        }
        sub = tmp->FindElementValueAsNumber("y");
        if (sub < 9999)
        {
            legend_list[0].pos_y  = sub;
        }
        
        LoadPainter(tmp->FindElement("painter"), &(legend_list[0].pen), &(legend_list[0].fnt));
        
        wxString align = std2wxstr(tmp->FindElementValue("align"));
        align.LowerCase();
        if (align.Find(wxT("right")) == -1)
        {
          legend_list[0].align_left = true;
        }
        else
        {
          legend_list[0].align_left = false;
        }
        wxString indicate = std2wxstr(tmp->FindElementValue("indicate"));
        indicate.LowerCase();
        if (indicate.Find(wxT("right")) == -1)
        {
          legend_list[0].indicate_left = true;
        }
        else
        {
          legend_list[0].indicate_left = false;
        }
    }
    else
    {
      legend_list[0].show = false;
    }
    if (tmp && (tmp = element->FindNextElement("legend")))
    {
      while (tmp)
      {
        Legend l;
        wxString show = std2wxstr(tmp->FindElementValue("show"));
        show.LowerCase();
        if (show.Find(wxT("true")) != -1)
        {
          l.show = true;
        }
        else
        {
          l.show = false;
        }
        double sub = tmp->FindElementValueAsNumber("value");
        if (sub < 9999)
        {
            l.value_x = sub;
        }
	else
        {
            l.value_x = 0.5;
        }
        sub = tmp->FindElementValueAsNumber("x");
        if (sub < 9999)
        {
            l.pos_x = sub;
        }
	else
        {
            l.pos_x = 0.7;
        }
        sub = tmp->FindElementValueAsNumber("y");
        if (sub < 9999)
        {
            l.pos_y = sub;
        }
	else
        {
            l.pos_y = 0.3;
        }
        l.fnt = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        l.pen = *wxBLACK;
 
        LoadPainter(tmp->FindElement("painter"), &(l.pen), &(l.fnt));
        
        wxString align = std2wxstr(tmp->FindElementValue("align"));
        align.LowerCase();
        if (align.Find(wxT("right")) == -1)
        {
          l.align_left = true;
        }
        else
        {
          l.align_left = false;
        }
        wxString indicate = std2wxstr(tmp->FindElementValue("indicate"));
        indicate.LowerCase();
        if (indicate.Find(wxT("right")) == -1)
        {
          l.indicate_left = true;
        }
        else
        {
          l.indicate_left = false;
        }
        legend_list.push_back(l);
        tmp = element->FindNextElement("legend");
      }
    }
    
    while (!curve_list.empty())
    {
      delete curve_list.back();
      curve_list.pop_back();
    }

    while (!curve_tag_list.empty())
    {
      delete curve_tag_list.back().data_condition;
      for (vector <FGFunction *>::iterator i= curve_tag_list.back().func_list.begin(); i != curve_tag_list.back().func_list.end(); ++i)
      {
        delete *i;
      }
      delete curve_tag_list.back().property;
      curve_tag_list.pop_back();
    }

    curve_tag_list.clear();

    is_combine_curves = false;
    combine_curves_distance = 99e99;
    if ((tmp = element->FindElement("combine_curves")))
    {
      is_combine_curves = true;
      combine_curves_distance = tmp->GetAttributeValueAsNumber("distance");
    }

    if ((tmp = element->FindElement("curves")))
    {
      size_t lastn=0u;
      while (tmp)
      {
        Element * data = tmp->FindElement("data");
        if (data)
        {
            CurvesTag tag;
            tag.ns = curve_list.size();
            tag.property = new FGPropertyManager();
	    Element * func = data->FindElement("function");
	    while (func)
	    {
	      tag.func_list.push_back(new FGFunction( tag.property, func));
	      func = data->FindNextElement("function");
	    }
            tag.data_condition = new Condition(data->FindElement("condition"), tag.property);
            tag.data_full_name = std2wxstr(data->GetAttributeValue("name"));
            wxString data_name = tag.data_full_name;
            wxString no = data_name.AfterFirst('[').BeforeFirst(']');
            wxString root_data_name = data_name.BeforeFirst('[');
            Table * table = handler->GetTable(root_data_name);
            if (table)
            {
              while (!no.IsEmpty())
              {
                long i;
                if (no.ToLong(&i))
                {
                  table = &((*table)[i]);
                  if (!table)
                  {
                    wxLogError(_("Fail to find the sub table."));
                    return false;
                  }
		  data_name = data_name.AfterFirst(']');
		  no = data_name.AfterFirst('[').BeforeFirst(']');
                }
                else
                {
                  wxLogError(_("Fail to parse the sub table."));
                  return false;
                }
              }
              tag.table_type = std2wxstr(data->GetAttributeValue("type"));
              tag.table_type.UpperCase();
              int dim;
              if (tag.table_type == wxT("1D"))
              {
                dim = 1;
              }
              else if (tag.table_type == wxT("2D"))
              {
                dim = 2;
              }
              else if (tag.table_type == wxT("3D--"))
              {
                dim = 3;
              }
              else if (tag.table_type == wxT("4D--"))
              {
                dim = 4;
              }
              else
              {
                wxLogError(_("Fail to parse the type of required table."));
                return false;
              }
              switch (dim)
              {
                  case 1 :
                      if (table->GetDimension() == 1)
                      {
                        vector<double> x, y;
			unsigned s = table->GetSize();
                        for (unsigned i =0;
                                i != s; ++i)
                        {
			  double a = table->GetKey(i);
			  double f = table->GetValue(i);
                          tag.property->SetDouble(table->GetIndepVar(), a);
                          tag.property->SetDouble(table->GetName(), f);
                          if (tag.data_condition->Evaluate())
			  {
			    x.push_back(a);
                            y.push_back(f);
			  }
                        }
                        push_back(new Curve(x,y));
			if (str_y_label.empty())
                          str_y_label = std2wxstr(table->GetNameLabel());
			if (str_x_label.empty())
                          str_x_label = std2wxstr(table->GetIndepVarLabel());
                      }
                      else
                      {
                        wxLogError(_("I want 1D table, but you give another."));
                        return false;
                      }
                      break;
                  case 2 :
                      if (table->GetDimension() == 2)
                      {
			if (str_y_label.empty())
                          str_y_label = std2wxstr(table->GetNameLabel());
                        tag.xcr = std2wxstr(data->FindElementValue("x"));
			tag.xcr.UpperCase();
			if (tag.xcr == wxT("COL"))
			{
			  if (str_x_label.empty())
                            str_x_label = std2wxstr(table->GetIndepVarLabel());
			  size_t s = table->GetSize();
			  vector <unsigned> ptr_list;
			  for (size_t i =0; i < s; ++i)
			  {
			    ptr_list.push_back(0u);
			  }
			  bool flag = true;
			  while (flag)
			  {
			    vector<double> x, y;
			    double key;
			    flag = pitch_col(x, y, key, ptr_list, table, tag.data_condition, tag.property);
			    if (x.size() >1 )
			    {
                              Curve * pp = new Curve(x,y);
                              pp->SetText(wxString::Format(wxT("%g"), key));
			      if (is_combine_curves && min_distance(pp, combine_curves_distance))
			      {
			        delete pp;
			      }
			      else
			      {
                              push_back(pp);
			      }
			    }
			  }
			}
			else
			{
                          tag.xcr = wxT("ROW");
			  if (str_x_label.empty())
                            str_x_label = std2wxstr((*table)[0].GetIndepVarLabel());
			  unsigned s = table->GetSize();
                          for (unsigned j=0;
                                  j != s; ++j)
                          {
                            tag.property->SetDouble(table->GetIndepVar(), table->GetKey(j));
			    Table * sub_tab = &(*table)[j];
			    unsigned ss = sub_tab->GetSize();
                            vector<double> x, y;
                            for (unsigned i = 0;
                                i != ss; ++i)
                            {
			      double a = sub_tab->GetKey(i);
			      double f = sub_tab->GetValue(i);
                              tag.property->SetDouble(sub_tab->GetIndepVar(), a);
                              tag.property->SetDouble(sub_tab->GetName(), f);
			      if (tag.data_condition->Evaluate())
			      {
			        x.push_back(a);
                                y.push_back(f);
			      }
                            }
			    if (x.size() > 1)
			    {
                              Curve * pp = new Curve(x,y);
                              pp->SetText(wxString::Format(wxT("%g"),table->GetKey(j)));
			      if (is_combine_curves && min_distance(pp, combine_curves_distance))
			      {
			        delete pp;
			      }
			      else
			      {
                              push_back(pp);
			      }
			    }
			  }
			}
                      }
                      else
                      {
                        wxLogError(_("I want 2D table, but you give another."));
                        return false;
                      }
                      break;
                  case 3 :
                      if (table->GetDimension() == 3)
                      {
			if (str_y_label.empty())
                          str_y_label = std2wxstr(table->GetNameLabel());
			Element *bt = data->FindElement("baseT2D");
			Element *nofilter = data->FindElement("nofilter");
                        wxString base_name = std2wxstr(bt->GetDataLine());
                        tag.base_name = base_name;
                        wxString no = base_name.AfterFirst('[').BeforeFirst(']');
                        wxString root_base_name = base_name.BeforeFirst('[');
                        Table * base_table = handler->GetTable(root_base_name);
                        if (!base_table)
                        {
                            wxLogError(_("Fail to find the base table."));
                            return false;                        
                        }
                        while (!no.IsEmpty())
                        {
                          long i;
                          if (no.ToLong(&i))
                          {
                            base_table = &((*base_table)[i]);
                            if (!base_table)
                            {
                              wxLogError(_("Fail to find the sub table."));
                              return false;
                            }
			    base_name = base_name.AfterFirst(']');
			    no = base_name.AfterFirst('[').BeforeFirst(']');
                          }
                          else
                          {
                             wxLogError(_("Fail to parse the sub table."));
                             return false;
                          }
                        }
                        if (base_table->GetDimension() != 2)
                        {
                          wxLogError(_("I want 2D base table, but you give another."));
                          return false;
                        }
			double dfrom = bt->GetAttributeValueAsNumber("from");
			wxString coord = std2wxstr(bt->GetAttributeValue("coordT3D"));
                        tag.coord = coord;
			int from;
			if (dfrom == 99e99 && coord.IsEmpty())
			{
			  from = 0;
			}
			else if (dfrom == 99e99)
			{
			  from = 3;
			}
			else if (coord.IsEmpty())
			{
			  from = 0;
			}
			else
			{
			  from = (int)dfrom;
			}
                        tag.from = from;
			Table * coord_table=NULL;
			if (!coord.IsEmpty())
			{
                          no = coord.AfterFirst('[').BeforeFirst(']');
                          wxString coord_name = coord.BeforeFirst('[');
                          coord_table = handler->GetTable(coord_name);
                          if (coord_table)
                          {
                            while (!no.IsEmpty())
                            {
                              long i;
                              if (no.ToLong(&i))
                              {
                                coord_table = &((*coord_table)[i]);
                                if (!coord_table)
                                {
                                  wxLogError(_("Fail to find the sub table."));
                                  return false;
                                }
			        coord = coord.AfterFirst(']');
			        no = coord.AfterFirst('[').BeforeFirst(']');
                              }
                              else
                              {
                                wxLogError(_("Fail to parse the sub table."));
                                return false;
                              }
                            }
                            if (from == 3 && coord_table->GetDimension() != 3)
                            {
                              wxLogError(_("I want 3D coord table, but you give another."));
                              return false;
                            }
			    else if ((from == 2 || from == 1) && coord_table->GetDimension() != 2)
                            {
                              wxLogError(_("I want 2D coord table, but you give another.")+wxString::Format(wxT("%d"), coord_table->GetDimension()));
                              return false;
                            }
                          }
			}
			if (from == 0 || from == 3)
			{
                          vector<double> x, a, b, y, yc, xx;
                          size_t dest_num = (*base_table).GetSize();
			  if (nofilter)
			  {
                            tag.nofilter = true;
			    double d = nofilter->GetAttributeValueAsNumber("num");
			    if (d > 2 && d < 9999 )
			    {
			      dest_num = (size_t)d;
			    }
			    else
			    {
			     dest_num = 9999;
			    }
                            tag.dest_num = dest_num;
			  }
                          else
                          {
                            tag.nofilter = true;
                          }
  			  size_t base_table_size = base_table->GetSize();
                          for (unsigned i = 0;i < base_table_size; ++i)
  			  {
  			  Table * sub_base_tab = &(*base_table)[i];
  			  double key = base_table->GetKey(i);
  			  size_t sub_base_table_size = sub_base_tab->GetSize();
                            for (unsigned j = 0;j < sub_base_table_size; ++j)
                            {
			      double sub_key = sub_base_tab->GetKey(j);
			      double sub_value = sub_base_tab->GetValue(j);
  		  	      a.push_back(key);
  			      b.push_back(sub_key);
                              x.push_back(sub_value);
                            }
  			  }
                          size_t  c = table->GetSize();
                          for (unsigned k = 0;
                                k < c; ++k)
                          {
			    Table * body_table = &(*table)[k];
			    Table * coord_sub_table;
  			    double key = table->GetKey(k);
                            tag.property->SetDouble(table->GetIndepVar(), key);
			    xx.clear();
                            y.clear();
			    if (coord_table)
			    {
			      coord_sub_table = &(*coord_table)[k];
			      yc.clear();
			    }
			    vector<double>::iterator m = x.begin();
			    vector<double>::iterator j = b.begin();
                            for (vector<double>::iterator i = a.begin();
                                i != a.end(); ++i, ++j, ++m)
                            {
                              tag.property->SetDouble(body_table->GetIndepVar(), *i);
                              tag.property->SetDouble((*body_table)[0].GetIndepVar(), *j);
                              tag.property->SetDouble(base_table->GetName(), *m);
			      vector<double> l;
			      l.push_back(*j);
			      l.push_back(*i);
			      bool in_range;
			      double val = body_table->GetValue(l, &in_range);
			      if (!in_range)
			      {
			        continue;
			      }
                              tag.property->SetDouble(body_table->GetName(), val);
			      
			      if (coord_table)
			      {
			        double valc = coord_sub_table->GetValue(l, &in_range);
				if (!in_range)
				{
					continue;
				}
                                tag.property->SetDouble(coord_sub_table->GetName(), valc);
				if (tag.data_condition->Evaluate())
				{
                                  y.push_back(val);
			          yc.push_back(valc);
				}
			      }
			      else if (tag.data_condition->Evaluate())
			      {
				xx.push_back(*m);
                                y.push_back(val);
			      }
                            }
			    if (y.size() > 1)
			    {
			    Curve *pp;
			    if (coord_table)
			    {
                                make_order(yc, y, dest_num);
                              pp = new Curve(yc,y);
			    }
			    else
			    {
			        make_order(xx, y, dest_num);
                              pp = new Curve(xx,y);
			    }
                            pp->SetText(wxString::Format(wxT("%g"), table->GetKey(k)));
			      if (is_combine_curves && min_distance(pp, combine_curves_distance))
			      {
			        delete pp;
			      }
			      else
			      {
                              push_back(pp);
			      }
			    }
                          }   
			}
			else if (from == 2)
			{
                          vector<double> x, y;
  			  size_t base_table_size = base_table->GetSize();
                          for (unsigned i = 0;i < base_table_size; ++i)
  			  {
  			    Table * sub_base_tab = &(*base_table)[i];
  			    Table * sub_coord_tab = &(*coord_table)[i];
  			    double key = base_table->GetKey(i);
                            tag.property->SetDouble(base_table->GetIndepVar(), key);
  			    size_t sub_base_table_size = sub_base_tab->GetSize();
			    x.clear();
			    y.clear();
                            for (unsigned j = 0;j < sub_base_table_size; ++j)
                            {
	                      double a = sub_base_tab->GetKey(j);
	                      double f = sub_base_tab->GetValue(j);
	                      double f2 = sub_coord_tab->GetValue(j);
                              tag.property->SetDouble(sub_base_tab->GetIndepVar(), a);
                              tag.property->SetDouble(sub_base_tab->GetName(), f);
                              tag.property->SetDouble(sub_coord_tab->GetName(), f2);
			      if (tag.data_condition->Evaluate())
			      {
                                y.push_back(f);
                                x.push_back(f2);
			      }
                            }
			    if (x.size() > 1)
			    {
			    Curve *pp;
                            make_order(x, y, 9999);
                            pp = new Curve(x,y);
                            pp->SetText(wxString::Format(wxT("%g"), key));
			      if (is_combine_curves && min_distance(pp, combine_curves_distance))
			      {
			        delete pp;
			      }
			      else
			      {
                              push_back(pp);
			      }
			    }
  			  }
			}
			else if (from == 1)
			{
                          vector<double> x, y;
                          //size_t dest_num = (*base_table).GetSize();
  			  Table * sub_bbase_tab = &(*base_table)[-1];
                          size_t sub_bbase_table_size = sub_bbase_tab->GetSize();
  			  size_t base_table_size = base_table->GetSize();
                          for (unsigned k = 0;k < sub_bbase_table_size; ++k)
  			  {
			    double key = sub_bbase_tab->GetKey(k);
                            tag.property->SetDouble(sub_bbase_tab->GetIndepVar(), key);
			    x.clear();
			    y.clear();
                            for (unsigned i = 0;i < base_table_size; ++i)
  			    {
  			      Table * sub_base_tab = &(*base_table)[i];
  			      Table * sub_coord_tab = &(*coord_table)[i];
  			      size_t sub_base_table_size = sub_base_tab->GetSize();
	                      double a = base_table->GetKey(i);
                              tag.property->SetDouble(base_table->GetIndepVar(), a);
                              for (unsigned j = 0;j < sub_base_table_size; ++j)
                              {
				if (sub_base_tab->GetKey(j) == key)
				{
	                      double f = sub_base_tab->GetValue(j);
	                      double f2 = sub_coord_tab->GetValue(j);
                              tag.property->SetDouble(sub_base_tab->GetName(), f);
                              tag.property->SetDouble(sub_coord_tab->GetName(), f2);
			      if (tag.data_condition->Evaluate())
			      {
                                  y.push_back(f);
                                  x.push_back(f2);
			      }
				  break;
				}
                              }
			    }
			    if (x.size() >= 2u)
			    {
			      Curve *pp;
                              make_order(x, y, 9999);
                              pp = new Curve(x,y);
                              pp->SetText(wxString::Format(wxT("%g"), key));
			      if (is_combine_curves && min_distance(pp, combine_curves_distance))
			      {
			        delete pp;
			      }
			      else
			      {
                              push_back(pp);
			      }
			    }
  			  }
			}
			else
			{
                          wxLogError(_("from which dimention to see?"));
                          return false;
			}
                      }
                      else
                      {
                        wxLogError(_("I want 3D table, but you give another."));
                        return false;
                      }
                      break;
                  case 4 :
                      if (table->GetDimension() == 4)
                      {
			if (str_y_label.empty())
                          str_y_label = std2wxstr(table->GetNameLabel());
			Element *bt = data->FindElement("baseT3D");
			Element *nofilter = data->FindElement("nofilter");
			Element *strange = data->FindElement("strange");
                        if (strange)
                        {
                          tag.strange = true;
                        }
                        else
                        {
                          tag.strange = false;
                        }
                        wxString base_name = std2wxstr(bt->GetDataLine());
                        tag.base_name = base_name;
                        wxString no = base_name.AfterFirst('[').BeforeFirst(']');
                        wxString root_base_name = base_name.BeforeFirst('[');
                        Table * base_table = handler->GetTable(root_base_name);
                        if (!base_table)
                        {
                            wxLogError(_("Fail to find the base table."));
                            return false;                        
                        }
                        while (!no.IsEmpty())
                        {
                          long i;
                          if (no.ToLong(&i))
                          {
                            base_table = &((*base_table)[i]);
                            if (!base_table)
                            {
                              wxLogError(_("Fail to find the sub table."));
                              return false;
                            }
			    base_name = base_name.AfterFirst(']');
			    no = base_name.AfterFirst('[').BeforeFirst(']');
                          }
                          else
                          {
                             wxLogError(_("Fail to parse the sub table."));
                             return false;
                          }
                        }
                        if (base_table->GetDimension() != 3)
                        {
                          wxLogError(_("I want 3D base table, but you give another."));
                          return false;
                        }
			double dfrom = bt->GetAttributeValueAsNumber("from");
			wxString coord = std2wxstr(bt->GetAttributeValue("coordT4D"));
                        tag.coord = coord;
			int from;
			if (dfrom == 99e99 && coord.IsEmpty())
			{
			  from = 0;
			}
			else if (dfrom == 99e99)
			{
			  from = 3;
			}
			else if (coord.IsEmpty())
			{
			  from = 0;
			}
			else
			{
			  from = (int)dfrom;
			}
                        tag.from = from;
			Table * coord_table=NULL;
			if (!coord.IsEmpty())
			{
                          no = coord.AfterFirst('[').BeforeFirst(']');
                          wxString coord_name = coord.BeforeFirst('[');
                          coord_table = handler->GetTable(coord_name);
                          if (coord_table)
                          {
                            while (!no.IsEmpty())
                            {
                              long i;
                              if (no.ToLong(&i))
                              {
                                coord_table = &((*coord_table)[i]);
                                if (!coord_table)
                                {
                                  wxLogError(_("Fail to find the sub table."));
                                  return false;
                                }
			        coord_name = coord_name.AfterFirst(']');
			        no = coord_name.AfterFirst('[').BeforeFirst(']');
                              }
                              else
                              {
                                wxLogError(_("Fail to parse the sub table."));
                                return false;
                              }
                            }
                            if (from == 3 && coord_table->GetDimension() != 3)
                            {
                              wxLogError(_("I want 3D coord table, but you give another."));
                              return false;
                            }
			    else if ((from == 2 || from == 1) && coord_table->GetDimension() != 2)
                            {
                              wxLogError(_("I want 2D coord table, but you give another."));
                              return false;
                            }
                          }
			}
			if (from == 0 || from == 3)
			{
                          vector<double> x, a, b, c, y, yc, xx;
                          size_t dest_num = (*base_table).GetSize();
			  if (nofilter)
			  {
                            tag.nofilter = true;
			    double d = nofilter->GetAttributeValueAsNumber("num");
			    if (d > 2 && d < 9999 )
			    {
			      dest_num = (size_t)d;
			    }
			    else
			    {
			     dest_num = 9999;
			    }
                            tag.dest_num = dest_num;
			  }
                          else
                          {
                            tag.nofilter = false;
                          }
  			  size_t base_table_size = base_table->GetSize();
                          for (unsigned i = 0;i < base_table_size; ++i)
  			  {
  			    Table * sub_base_tab = &(*base_table)[i];
  			    double key = base_table->GetKey(i);
  			    size_t sub_base_table_size = sub_base_tab->GetSize();
                            for (unsigned j = 0;j < sub_base_table_size; ++j)
                            {
  			       Table * ssub_base_tab = &(*sub_base_tab)[j];
  			       double key2 = sub_base_tab->GetKey(j);
  			       size_t ssub_base_table_size = ssub_base_tab->GetSize();
                               for (unsigned k = 0;k < ssub_base_table_size; ++k)
                               {
  			        a.push_back(key);
				b.push_back(key2);
  			        c.push_back(ssub_base_tab->GetKey(k));
                                x.push_back(ssub_base_tab->GetValue(k));
			       }
                            }
  			  }
                          vector<double> ys_x;
                          vector<vector<double> > ys_y, ys_xx;
                          size_t  sc = table->GetSize();
                          for (unsigned kk = 0;
                                kk < sc; ++kk)
                          {
			    Table * body_table = &(*table)[kk];
			    Table * coord_sub_table;
  			    double key = table->GetKey(kk);
                            tag.property->SetDouble(table->GetIndepVar(), key);
                            y.clear();
			    xx.clear();
			    if (coord_table)
			    {
			      coord_sub_table = &(*coord_table)[kk];
			      yc.clear();
			    }
			    vector<double>::iterator m = x.begin();
			    vector<double>::iterator k = c.begin();
			    vector<double>::iterator j = b.begin();
                            for (vector<double>::iterator i = a.begin();
                                i != a.end(); ++i, ++j, ++k, ++m)
                            {
                              tag.property->SetDouble(body_table->GetIndepVar(), *i);
                              tag.property->SetDouble((*body_table)[0].GetIndepVar(), *j);
                              tag.property->SetDouble((*body_table)[0][0].GetIndepVar(), *k);
                              tag.property->SetDouble(base_table->GetName(), *m);
			      vector<double> l;
			      l.push_back(*k);
			      l.push_back(*j);
			      l.push_back(*i);
			      bool in_range;
			      double val = body_table->GetValue(l, &in_range);
			      if (!in_range)
			      {
				      continue;
			      }
                              tag.property->SetDouble(body_table->GetName(), val);
			      if (coord_table)
			      {
			        double valc = coord_sub_table->GetValue(l, &in_range);
				if (!in_range)
				{
					continue;
				}
                                tag.property->SetDouble(coord_sub_table->GetName(), valc);
				if (tag.data_condition->Evaluate())
				{
                                  y.push_back(val);
			          yc.push_back(valc);
				}
			      }
			      else if (tag.data_condition->Evaluate())
			      {
				xx.push_back(*m);
                                y.push_back(val);
			      }
                            }
			    if (y.size() > 1)
			    {
			    Curve *pp;
			    if (coord_table)
			    {
                                make_order(yc, y, dest_num);
                              pp = new Curve(yc,y);
			    }
			    else
			    {
			      if (strange && dest_num < 30)
			      {
			        make_order(xx, y, dest_num);			
			      }
			      else
			      {
			        make_order(xx, y, dest_num);			
			      }
			      ys_y.push_back(y);
			      ys_xx.push_back(xx);
			      if (!strange)
                                pp = new Curve(xx,y);
			    }

			    if (!strange)
                              pp->SetText(wxString::Format(wxT("%g"), table->GetKey(kk)));
                            ys_x.push_back(table->GetKey(kk));
			    if (!strange)
			    {
			      if (is_combine_curves && min_distance(pp, combine_curves_distance))
			      {
			        delete pp;
			      }
			      else
			      {
                              push_back(pp);
			      }
			    }
			    }
                          }
			  if (strange)
			  {
			    x_base_strange_mode = strange->GetAttributeValueAsNumber("value");
			    if (x_base_strange_mode == 99e99)
			    {
			      x_base_strange_mode =  ys_x[0];
			    }
			    vector<Matt::BSplineD> bl;
			    for (size_t j = 1; j < ys_y.size(); ++j)
			    {
			      bl.push_back(Matt::BSplineD(ys_xx[j], ys_y[j]));
			    }
		            for (size_t j = 0; j < ys_y[0].size(); ++j)
			    {
		              vector<double> tmp;
			      tmp.push_back(ys_y[0][j]);
			      for (vector<Matt::BSplineD >::iterator k = bl.begin(); k != bl.end(); ++k)
			      {
			        tmp.push_back(k->evaluate(ys_xx[0][j]));
			      }
			      Curve *pp = new Curve(ys_x, tmp);
			      push_back(pp);
			    }
			    strange_mode = true;
			    if (str_x_label.empty())
                              str_x_label = std2wxstr(table->GetIndepVarLabel());
			  }
			}
			else if (from == 2)
			{
                          vector<double> x, y;
  			  size_t base_table_size = base_table->GetSize();
                          for (unsigned i = 0;i < base_table_size; ++i)
  			  {
  			    Table * sub_base_tab = &(*base_table)[i];
  			    Table * sub_coord_tab = &(*coord_table)[i];
  			    double key = base_table->GetKey(i);
                            tag.property->SetDouble(base_table->GetIndepVar(), key);
  			    size_t sub_base_table_size = sub_base_tab->GetSize();
			    x.clear();
			    y.clear();
                            for (unsigned j = 0;j < sub_base_table_size; ++j)
                            {
	                      double a = sub_base_tab->GetKey(j);
	                      double f = sub_base_tab->GetValue(j);
	                      double f2 = sub_coord_tab->GetValue(j);
                              tag.property->SetDouble(sub_base_tab->GetIndepVar(), a);
                              tag.property->SetDouble(sub_base_tab->GetName(), f);
                              tag.property->SetDouble(sub_coord_tab->GetName(), f2);
			      if (tag.data_condition->Evaluate())
			      {
                                y.push_back(f);
                                x.push_back(f2);
			      }
                            }
			    if (x.size() > 1)
			    {
			    Curve *pp;
                            make_order(x, y, 9999);
                            pp = new Curve(x,y);
                            pp->SetText(wxString::Format(wxT("%g"), key));
			      if (is_combine_curves && min_distance(pp, combine_curves_distance))
			      {
			        delete pp;
			      }
			      else
			      {
                              push_back(pp);
			      }
			    }
  			  }
			}
			else if (from == 1)
			{
                          vector<double> x, y;
                          //size_t dest_num = (*base_table).GetSize();
  			  Table * sub_bbase_tab = &(*base_table)[-1];
                          size_t sub_bbase_table_size = sub_bbase_tab->GetSize();
  			  size_t base_table_size = base_table->GetSize();
                          for (unsigned k = 0;k < sub_bbase_table_size; ++k)
  			  {
			    double key = sub_bbase_tab->GetKey(k);
                            tag.property->SetDouble(sub_bbase_tab->GetIndepVar(), key);
			    x.clear();
			    y.clear();
                            for (unsigned i = 0;i < base_table_size; ++i)
  			    {
  			      Table * sub_base_tab = &(*base_table)[i];
  			      Table * sub_coord_tab = &(*coord_table)[i];
  			      size_t sub_base_table_size = sub_base_tab->GetSize();
                              for (unsigned j = 0;j < sub_base_table_size; ++j)
                              {
				if (sub_base_tab->GetKey(j) == key)
				{
	                      double f = sub_base_tab->GetValue(j);
	                      double f2 = sub_coord_tab->GetValue(j);
                              tag.property->SetDouble(sub_base_tab->GetName(), f);
                              tag.property->SetDouble(sub_coord_tab->GetName(), f2);
			      if (tag.data_condition->Evaluate())
			      {
                                  y.push_back(f);
                                  x.push_back(f2);
			      }
				  break;
				}
                              }
			    }
			    if (x.size() >= 2u)
			    {
			      Curve *pp;
                              make_order(x, y, 9999);
                              pp = new Curve(x,y);
                              pp->SetText(wxString::Format(wxT("%g"), key));
			      if (is_combine_curves && min_distance(pp, combine_curves_distance))
			      {
			        delete pp;
			      }
			      else
			      {
                              push_back(pp);
			      }
			    }
  			  }
			}
			else
			{
                          wxLogError(_("from which dimention to see?"));
                          return false;
			}
                      }
                      else
                      {
                        wxLogError(_("I want 3D table, but you give another."));
                        return false;
                      }
                      break;
                  default :
                      break;
              }
            }
            else
            {
              wxLogError(_("Can not find proper table."));
              return false;
            }
            tag.ne = curve_list.size();
            curve_tag_list.push_back(tag);
        }
        else
        {
          wxLogError(_("Can not find data tag!"));
          return false;
        }

	size_t n = curve_list.size() - lastn;
        Element * rcurve = tmp->FindElement("rcurve");
        for (vector<Curve *>::reverse_iterator i = curve_list.rbegin();
                i != curve_list.rend()-lastn && rcurve; )
        {
	  int num;
	  double tmpd = rcurve->GetAttributeValueAsNumber("n");
	  if (tmpd == 99e99 || tmpd <1 )
	  {
	   num =1;
	  }
	  else
	  {
	    num = (int) tmpd;
	  }
	  for (int j=0; j<num && i != curve_list.rend(); ++j)
	  {
            (*i)->Load(rcurve);
	    ++i;
	    --n;
	  }
          rcurve = tmp->FindNextElement("rcurve");
        }
        Element * curve = tmp->FindElement("curve");
        Element * curve_pre = curve;
        if (curve)
        {
	  size_t j=0u;
          for (vector<Curve *>::iterator i = curve_list.begin()+lastn;
                i != curve_list.end() && j < n; )
          {
	    int num;
	    double tmpd = curve->GetAttributeValueAsNumber("n");
	    if (tmpd == 99e99 || tmpd <1 )
	    {
	      num =1;
	    }
	    else
	    {
	      num = (int) tmpd;
	    }
	    for (int k=0; k<num && i != curve_list.end() && j < n; ++k)
	    {
              (*i)->Load(curve);
	      ++i;
	      ++j;
	    }
             if (curve_pre)
             {
               curve_pre = tmp->FindNextElement("curve");
             }
             if (curve_pre)
             {
               curve = curve_pre;
             }
          }
        }
	lastn = curve_list.size();
	tmp = element->FindNextElement("curves");
      }
    }
    else
    {
        wxLogError(_("Can not find curves tag!"));
        return false;
    }

    while (!children.empty())
    {
      delete children.back();
      children.pop_back();
    }
    if ((tmp = element->FindElement("subwindows")))
    {
       Element * win = tmp->FindElement("window");
       while (win)
       {
          try
          {
            children.push_back(new PlotWindow(handler, win));
          }
          catch (...)
          {
            wxLogError(_("Fail to insert sub window."));
            return false;
          }
          win = tmp->FindNextElement("window");
       }
    }
    return true;
}

void PlotWindow::Export(wxTextOutputStream & tstream, const wxString & prefix) const
{
  tstream << prefix << wxT("<window>\n");
  {
    wxString pre = prefix + wxT("    ");
    tstream << pre << _("<!-- the postion of the window -->\n");
    tstream << pre << wxT("<pos>\n");
    tstream << pre << _("    <!-- which row(start from 0) -->\n");
    tstream << pre << wxT("    <row> ") << row_pos << wxT(" </row>\n");
    tstream << pre << _("    <!-- which column(start from 0) -->\n");
    tstream << pre << wxT("    <col> ") << col_pos << wxT(" </col>\n");
    tstream << pre << _("    <!-- which div method(start from 0) -->\n");
    tstream << pre << wxT("    <mgr> ") << mgr_pos << wxT(" </mgr>\n");
    tstream << pre << wxT("</pos>\n");
    tstream << pre << _("<!-- the direction of the axises -->\n");
    tstream << pre << wxT("<dir>\n");
    wxString xdir, ydir;
    switch (axis_dir)
    {
      case X_LEFT_Y_UP :
      case X_LEFT_Y_DOWN :
        xdir = wxT("left");
        break;
      case X_RIGHT_Y_UP :
      case X_RIGHT_Y_DOWN :
        xdir = wxT("right");
        break;
      case X_UP_Y_LEFT :
      case X_UP_Y_RIGHT :
        xdir = wxT("up");
        break;
      case X_DOWN_Y_LEFT :
      case X_DOWN_Y_RIGHT :
        xdir = wxT("down");
        break;
    }
    switch (axis_dir)
    {
      case X_LEFT_Y_UP :
      case X_RIGHT_Y_UP :
        ydir = wxT("up");
        break;
      case X_LEFT_Y_DOWN :
      case X_RIGHT_Y_DOWN :
        ydir = wxT("down");
        break;
      case X_UP_Y_LEFT :
      case X_DOWN_Y_LEFT :
        ydir = wxT("left");
        break;
      case X_UP_Y_RIGHT :
      case X_DOWN_Y_RIGHT :
        ydir = wxT("right");
        break;
    }
    tstream << pre << _("    <!-- the direction of the x axis(one of 'left', 'right', 'up', 'down') -->\n");
    tstream << pre << wxT("    <x> ") << xdir << wxT(" </x>\n");
    tstream << pre << _("    <!-- the direction of the y axis(one of 'left', 'right', 'up', 'down') -->\n");
    tstream << pre << wxT("    <y> ") << ydir << wxT(" </y>\n");
    tstream << pre << wxT("</dir>\n");
    tstream << pre << _("<!-- the axis setting -->\n");
    tstream << pre << wxT("<axis>\n");
    tstream << pre << _("    <!-- the setting for x axis -->\n");
    tstream << pre << wxT("    <x>\n");
    tstream << pre << _("        <!-- coordinate the x axis with another window(id from 0), or -1 if not any. -->\n");
    tstream << pre << wxT("        <info_from> ") << x_info_from << wxT(" </info_from>\n");
    wxString pos;
    switch (axis_x_pos)
    {
      case LOW_OUT :
        pos = wxT("LOW_OUT");
        break;
      case LOW_IN :
        pos = wxT("LOW_IN");
        break;
      case HIGH_IN :
        pos = wxT("HIGH_IN");
        break;
      case HIGH_OUT :
        pos = wxT("HIGH_OUT");
        break;
    }
    tstream << pre << _("        <!-- the position of the axis in the window(LOW_OUT, LOW_IN, HIGH_OUT, HIGH_OUT) -->\n");
    tstream << pre << wxT("        <pos> ") << pos << wxT(" </pos>\n");
    if (axis_x_hide)
    {
      pos = wxT("true");
    }
    else
    {
      pos = wxT("false");
    }
    tstream << pre << _("        <!-- hide or show(true or false) -->\n");
    tstream << pre << wxT("        <hide> ") << pos << wxT(" </hide>\n");
    tstream << pre << _("        <!-- auto calculate the range(true) or given with <max><min> tag(false) -->\n");
    if (auto_x_space)
    {
      pos = wxT("true");
      tstream << pre << wxT("        <auto_space> ") << pos << wxT(" </auto_space>\n");
    }
    else
    {
      pos = wxT("false");
      tstream << pre << wxT("        <auto_space> ") << pos << wxT(" </auto_space>\n");
      tstream << pre << wxT("        <max> ") << max_cx << wxT(" </max>\n");
      tstream << pre << wxT("        <min> ") << min_cx << wxT(" </min>\n");
    }
    tstream << pre << _("        <!-- the lable for x axis -->\n");
    tstream << pre << wxT("        <label> ") << str_x_label << wxT(" </label>\n");
    tstream << pre << wxT("    </x>\n");
    tstream << pre << _("    <!-- the setting for y axis -->\n");
    tstream << pre << wxT("    <y>\n");
    switch (axis_y_pos)
    {
      case LOW_OUT :
        pos = wxT("LOW_OUT");
        break;
      case LOW_IN :
        pos = wxT("LOW_IN");
        break;
      case HIGH_IN :
        pos = wxT("HIGH_IN");
        break;
      case HIGH_OUT :
        pos = wxT("HIGH_OUT");
        break;
    }
    tstream << pre << _("        <!-- the position of the axis in the window(LOW_OUT, LOW_IN, HIGH_OUT, HIGH_OUT) -->\n");
    tstream << pre << wxT("        <pos> ") << pos << wxT(" </pos>\n");
    if (axis_y_hide)
    {
      pos = wxT("true");
    }
    else
    {
      pos = wxT("false");
    }
    tstream << pre << _("        <!-- hide or show(true or false) -->\n");
    tstream << pre << wxT("        <hide> ") << pos << wxT(" </hide>\n");
    tstream << pre << _("        <!-- auto calculate the range(true) or given with <max><min> tag(false) -->\n");
    if (auto_y_space)
    {
      pos = wxT("true");
      tstream << pre << wxT("        <auto_space> ") << pos << wxT(" </auto_space>\n");
    }
    else
    {
      pos = wxT("false");
      tstream << pre << wxT("        <auto_space> ") << pos << wxT(" </auto_space>\n");
      tstream << pre << wxT("        <max> ") << max_cy << wxT(" </max>\n");
      tstream << pre << wxT("        <min> ") << min_cy << wxT(" </min>\n");
    }
    tstream << pre << _("        <!-- the lable for y axis -->\n");
    tstream << pre << wxT("        <label> ") << str_y_label << wxT(" </label>\n");
    tstream << pre << wxT("    </y>\n");
    tstream << pre << _("    <!-- the pen and colour setting for axises -->\n");
    ExportPainter(tstream, pre+wxT("    "), pen_axis, fnt_axis);
    tstream << pre << wxT("</axis>\n");
    tstream << wxT("\n");
    for (LegendListCIter i= legend_list.begin(); i != legend_list.end(); ++i)
    {
      tstream << pre << _("<!-- legend setting -->\n");
      tstream << pre << wxT("<legend>\n");
      {
        wxString pre2 = pre + wxT("    ");
        tstream << pre2 << _("<!-- legend show or not (true or false) -->\n");
        if (i->show)
        {
          tstream << pre2 << wxT("<show> true </show>\n");
        }
        else
        {
          tstream << pre2 << wxT("<show> false </show>\n");
        }
        tstream << pre2 << _("<!-- line from position [0,1] -->\n");
        tstream << pre2 << wxT("<value> ") << i->value_x << wxT(" </value>\n");
        tstream << pre2 << _("<!-- legend position [0,1] -->\n");
        tstream << pre2 << wxT("<x> ") << i->pos_x << wxT(" </x>\n");
        tstream << pre2 << wxT("<y> ") << i->pos_y << wxT(" </y>\n");
        tstream << pre2 << _("<!-- legend painter setting -->\n");
        ExportPainter(tstream, pre2, i->pen, i->fnt);
        tstream << pre2 << _("<!-- legend align (left or right) -->\n");
        if (i->align_left)
        {
          tstream << pre2 << wxT("<align> left </align>\n");
        }
        else
        {
          tstream << pre2 << wxT("<align> right </align>\n");
        }
        tstream << pre2 << _("<!-- legend indicate from (left or right) -->\n");
        if (i->indicate_left)
        {
          tstream << pre2 << wxT("<indicate> left </indicate>\n");
        }
        else
        {
          tstream << pre2 << wxT("<indicate> right </indicate>\n");
        }
      }
      tstream << pre << wxT("</legend>\n");
      tstream << wxT("\n");
    }
    if (is_combine_curves)
    {
      tstream << pre << _("<!-- combine curves within distance -->\n");
      tstream << pre << wxT("<combine_curves distance=\"") << combine_curves_distance << wxT("\"/>\n\n");
    }
    wxString pre2 = pre + wxT("    ");
    wxString pre3 = pre2 + wxT("    ");
    for (CurvesTagListCIter i=curve_tag_list.begin(); i != curve_tag_list.end(); ++i)
    {
      tstream << pre << _("<!-- curves' setting -->\n");
      tstream << pre << wxT("<curves>\n");
      tstream << pre << _("<!-- data from table -->\n");
      tstream << pre << _("<!-- type can be 1D, 2D, 3D- -,4D- - -->\n");
      tstream << pre2 << wxT("<data name=\"");
      tstream << i->data_full_name;
      tstream << wxT("\" type=\"");
      tstream << i->table_type;
      tstream << wxT("\">\n");
      tstream << pre3 << _("<!-- functions for filter if any -->\n");
      for (vector<FGFunction *>::const_iterator j = i->func_list.begin(); j != i->func_list.end(); ++j)
      {
        ExportFunction(tstream, pre3, *j);
      }
      tstream << pre3 << _("<!-- conditions for filter if any -->\n");
      if (i->data_condition->conditions.size() > 0u)
      {
        ExportCondition(tstream, pre3, i->data_condition);
      }
      if (i->table_type == wxT("1D"))
      {
      
      }
      else if (i->table_type == wxT("2D"))
      {
        tstream << pre3 << wxT("<x>") << i->xcr << wxT("</x>\n");
      }
      else if (i->table_type == wxT("3D--"))
      {
        tstream << pre3 << wxT("<baseT2D from=\"") << i->from;
        if (! i->coord.IsEmpty())
        {
         tstream << wxT("\" coordT3D=\"") << i->coord;
        }
        tstream << wxT("\">") << i->base_name << wxT("</baseT2D>\n");
        if (i->nofilter)
        {
          if (i->dest_num >= 9999)
          {
            tstream << pre3 << wxT("<nofilter/>\n");
          }
          else
          {
            tstream << pre3 << wxT("<nofilter num=\"") << i->dest_num << wxT("\"/>\n");
          }
        }
      }
      else if (i->table_type == wxT("4D--"))
      {
        tstream << pre3 << wxT("<baseT3D from=\"") << i->from;
        if (! i->coord.IsEmpty())
        {
         tstream << wxT("\" coordT4D=\"") << i->coord;
        }
        tstream << wxT("\">") << i->base_name << wxT("</baseT3D>\n");
        if (i->nofilter)
        {
          if (i->dest_num >= 9999)
          {
            tstream << pre3 << wxT("<nofilter/>\n");
          }
          else
          {
            tstream << pre3 << wxT("<nofilter num=\"") << i->dest_num << wxT("\"/>\n");
          }
        }
        if (i->strange)
        {
          tstream << pre3 << wxT("<strange/>\n");
        }
      }
      tstream << pre2 << wxT("</data>\n");
      const Curve * last = curve_list[i->ns];
      unsigned n = 1u;
      //TODO regroup problem
      //if (*curve_list[i->ne-1] != *curve_list[i->ne-2])
      //{
      //  ++m;
      //  curve_list[i->ne-1]->Export(tstream, pre2, 1u, true);
      //}
      vector<Curve *>::const_iterator j=curve_list.begin()+i->ns;
      for (++j;j != curve_list.begin()+i->ne; ++j)
      {
        if ( *last != **j)
        {
          last->Export(tstream, pre2, n);
          tstream << wxT("\n");
          last = *j;
          n = 1u;
        }
        else
        {
          ++n;
        }
      }
      last->Export(tstream, pre2, 1u);
      tstream << pre << wxT("</curves>\n");
      tstream << wxT("\n");
    }
    tstream << pre << _("<!-- subwindow setting if any -->\n");
    tstream << pre << wxT("<subwindows>\n");
    for (vector<PlotWindow *>::const_iterator i = children.begin(); i != children.end(); ++i)
    {
      (*i)->Export(tstream, pre2);
      tstream << wxT("\n");
    }
    tstream << pre << wxT("</subwindows>\n");
  }
  tstream << prefix << wxT("</window>\n");
}

double PlotWindow::distance(Curve * c1, Curve * c2)
{
  double min1 = c1->Xmin();
  double min2 = c2->Xmin();
  double min = min1 > min2 ? min1 : min2;
  double dist1 = std::abs(c1->evaluate(min) - c2->evaluate(min)); 
  double max1 = c1->Xmax();
  double max2 = c2->Xmax();
  double max = max1 < max2 ? max1 : max2;
  double dist2 = std::abs(c1->evaluate(max) - c2->evaluate(max)); 
  double dist =  (dist1 > dist2) ? dist1 : dist2;
  unsigned n1 = c1->nNode(); 
  unsigned n2 = c2->nNode(); 
  unsigned n = n1 > n2 ? n1 : n2;
  ++n;
  double dd = 1.0/double(n);
  for(double d = dd; d<1.0; d+=dd)
  {
    double t = min+d*(max-min);
    double tmp = std::abs(c1->evaluate(t) - c2->evaluate(t));
    if (tmp > dist)
    {
      dist = tmp;
    }
  }
  return dist;
}

bool PlotWindow::min_distance(Curve * c, const double & thr)
{
  double rslt = 99e99;
  Curve * best;
  for (vector<Curve *>::iterator i = curve_list.begin();
          i != curve_list.end(); ++i)
  {
    double dist = distance(*i, c);
    if (dist < rslt)
    {
      rslt = dist;
      best = *i;
    }
  }

  if (rslt < thr)
  {
    best->SetText(best->GetText() + wxT(" ") + c->GetText());
    double range1 = best->Xmax() -  best->Xmin();
    double range2 = c->Xmax() -  c->Xmin();
    if (range2 > 1.333 * range1 && rslt < 5 * thr)
    {
	*(dynamic_cast<Matt::BSplineD *>(best)) = *(dynamic_cast<Matt::BSplineD *>(c));
    }
    return true;
  }
  else
  {
    return false;
  }
}

Curve * PlotWindow::GetCurrentCurve()
{
    if (cur_curve)
    {
      return cur_curve;
    }
    else if (!curve_list.empty())
    {
      return (cur_curve = curve_list.front());
    }
    else
    {
      return NULL;
    }
}

Curve * PlotWindow::PopCurrentCurve()
{
    if (cur_curve)
    {
      vector<Curve *>::iterator i = find(curve_list.begin(), curve_list.end(), cur_curve);
      if (i != curve_list.end())
      {
        curve_list.erase(i);
        Curve * cur_curve_bak = cur_curve;
        if (!curve_list.empty())
        {
          cur_curve = curve_list.front();
        }
        else
        {
          cur_curve = NULL;
        }
        return cur_curve_bak;
      }
      else
      {
        cur_curve = NULL;
        return NULL;
      }
    }
    else
    {
      return NULL;
    }
}

void PlotWindow::push_back(Curve * c)
{
    curve_list.push_back(c);
    if (!cur_curve)
    {
      cur_curve = c;
    }
}

void PlotWindow::pop_back()
{
    if (!curve_list.empty())
    {
      if (cur_curve == curve_list.back())
      {
        cur_curve = curve_list.front();
      }
      if (curve_list.size() == 1u)
      {
        cur_curve = NULL;
      }
      delete curve_list.back();
      curve_list.pop_back();
    }
}

bool PlotWindow::empty() const
{
    return curve_list.empty();
}

Curve * PlotWindow::at(size_t i)
{
    if (i < curve_list.size())
    {
      return curve_list[i];
    }
    else
    {
      return NULL;
    }
}

Curve * PlotWindow::pop(size_t i)
{
    if (i < curve_list.size())
    {
      vector<Curve *>::iterator j = curve_list.begin()+i;
      Curve * k = *j;
      curve_list.erase(j);
      if (cur_curve == k)
      {
        cur_curve = curve_list.front();
      }
      if (curve_list.empty())
      {
        cur_curve = NULL;
      }      
      return k;
    }
    else
    {
      return NULL;
    }
}

void PlotWindow::erase(size_t i)
{
  delete pop(i);
}    

void PlotWindow::clear()
{
  while (!curve_list.empty())
  {
    delete curve_list.back();
    curve_list.pop_back();
  }
  cur_curve = NULL;

    while (!curve_tag_list.empty())
    {
      delete curve_tag_list.back().data_condition;
      for (vector <FGFunction *>::iterator i= curve_tag_list.back().func_list.begin(); i != curve_tag_list.back().func_list.end(); ++i)
      {
        delete *i;
      }
      delete curve_tag_list.back().property;
      curve_tag_list.pop_back();
    }
}

void PlotWindow::PopBackChild()
{
    if (!children.empty()) 
    {
        delete children.back();
        children.pop_back();
    }
}

PlotWindow *PlotWindow::GetChild(size_t i)
{
    if (i < children.size())
    {
      return children[i];
    }
    else
    {
      return NULL;
    }
}

PlotWindow *PlotWindow::PopChild(size_t i)
{
  if (i < children.size())
  {
    vector<PlotWindow *>::iterator j = children.begin()+i;
    PlotWindow * k = *j;
    children.erase(j);
    return k;
  }
  else
  {
    return NULL;
  }
}

void PlotWindow::ClearChildren()
{
  while (!children.empty())
  {
    delete children.back();
    children.pop_back();
  }
}

void PlotWindow::Draw(wxDC& dc, PosMgr& pos_mgr)
{
    wxCoord x, y;
    size_t w, h;
    pos_mgr.GetPos(row_pos, col_pos, x, y, w, h, mgr_pos, &dc);
    unsigned gw = pos_mgr.GetGridW();
    unsigned gh = pos_mgr.GetGridH();
    Draw(dc, x, y, w, h, gw, gh);
    for (vector<PlotWindow *>::iterator i = children.begin(); i != children.end(); ++i)
    {
      int j = (*i)->GetXInfoFrom();
      if (j == -1)
      {
	if ( (*i)->strange_mode )
	{
          (*i)->AutoYSpace(false);
          (*i)->SetMaxY(GetMaxY());
          (*i)->SetMinY(GetMinY());
          (*i)->SetYMarkMode(2);
          (*i)->SetYMarkInterval(GetYMarkInterval());
          (*i)->SetYMarkList(GetYMarkList());
          //(*i)->HideAxisY();
	}
	else
	{
          (*i)->AutoXSpace(false);
          (*i)->SetMaxX(GetMaxY());
          (*i)->SetMinX(GetMinY());
          (*i)->SetXMarkMode(2);
          (*i)->SetXMarkInterval(GetYMarkInterval());
          (*i)->SetXMarkList(GetYMarkList());
          (*i)->HideAxisX();
	}
      }
      else
      {
        (*i)->AutoXSpace(false);
        (*i)->SetMaxX(children[j]->GetMaxY());
        (*i)->SetMinX(children[j]->GetMinY());
        (*i)->SetXMarkMode(2);
        (*i)->SetXMarkInterval(children[j]->GetYMarkInterval());
        (*i)->SetXMarkList(children[j]->GetYMarkList());
        (*i)->HideAxisX();
      }
      (*i)->Draw(dc, pos_mgr);

    }
}

void PlotWindow::Draw(wxDC& dc, const wxCoord &x, const wxCoord &y, const size_t &w, const size_t &h, const unsigned & gw, const unsigned & gh)
{
  // get max_y & min_y
  if (auto_y_space)
  {
    max_y = getMaxY();
    min_y = getMinY();
    if (std::abs(max_y-min_y) < 2e-4)
    {
      max_y += 1e-4;
      min_y -= 1e-4;
    }
  }
  else
  {
    max_y = max_cy;
    min_y = min_cy;
  }
  
  // get max_x & min_x
  if (auto_x_space)
  {
    max_x = getMaxX();
    min_x = getMinX();
    if (std::abs(max_x-min_x) < 2e-4)
    {
      max_x += 1e-4;
      min_x -= 1e-4;
    }
  }
  else
  {
    max_x = max_cx;
    min_x = min_cx;
  }
  
 
  unsigned dx, dy;
  if (axis_dir == X_LEFT_Y_UP || axis_dir == X_LEFT_Y_DOWN || axis_dir == X_RIGHT_Y_UP || axis_dir == X_RIGHT_Y_DOWN)
  {
    dx = (unsigned)round(w/(double)gw);
    dy = (unsigned)round(h/(double)gh);
  } 
  else
  {
    dy = (unsigned)round(w/(double)gw);
    dx = (unsigned)round(h/(double)gh);
  }

  // get y interval
  if (y_mark_mode == 0)
    get_interval(min_y, max_y, dy, y_mark_interval, y_mark_list);
  // get x interval
  if (x_mark_mode == 0)
    get_interval(min_x, max_x, dx, x_mark_interval, x_mark_list);
  
  if (!axis_x_hide)
  {
    draw_x_axis(dc, x, y, w, h);
  }
  if (!axis_y_hide)
  {
    draw_y_axis(dc, x, y, w, h);
  }
  draw_curve(dc, x, y, w, h);
  for (LegendListIter i=legend_list.begin(); i != legend_list.end(); ++i)
  {
    if (i->show)
    {
      draw_legend(dc, x, y, w, h, i);
    }
  }
}

void PlotWindow::get_interval(double & min, double & max, unsigned int num, double & mark_interval, vector< double > & mark_list)
{
  // get interval
  double range = max-min;

  double step = range/(double)num;
  int int_log_range = (int)floor( log10( step ) );
  double base = 1.0;
  if (int_log_range > 0)
  {
    for (int i = 0; i < int_log_range; ++i)
      base *= 10;
  }
  if (int_log_range < 0)
  {
    for (int i = 0; i < -int_log_range; ++i)
      base /= 10;
  }
  double delta[] = {1, 2, 2.5, 5, 7.5, 10};
  double tmin, tmax, tmark_interval;
  for (int i =0; delta[i] <= 10; ++i)
  {
      tmark_interval = base*delta[i];
      tmin = floor(min / tmark_interval) * tmark_interval;
      tmax = tmark_interval*num + tmin;
      if ( tmax >= max )
      {
        break;
      }
  }
  double smin, smax, smark_interval;
  for (int i =0; delta[i] <= 10; ++i)
  {
      smark_interval = base*delta[i];
      smax = ceil(max / smark_interval) * smark_interval;
      smin = smax - smark_interval*num;
      if ( smin <= min)
      {
        break;
      }
  }

  double cmp1 = min - tmin;
  double cmp2 = tmax - max;
  double cmp3 = cmp1>cmp2?cmp1:cmp2;

  double cmp1s = min - smin;
  double cmp2s = smax - max;
  double cmp3s = cmp1s>cmp2s?cmp1s:cmp2s;

  if (cmp3 < cmp3s || tmax == 0 || tmin ==0 )
  {
    mark_interval = tmark_interval;
    max = tmax;
    min = tmin;  
  }
  else
  {
    mark_interval = smark_interval;
    max = smax;
    min = smin;  
  }
  
  mark_list.clear();
  for (double i=min; i<=max; i+=mark_interval)
  {
    if (fabs(i)< 1e-14)
      mark_list.push_back(0);
    else
      mark_list.push_back(i);
  }
}

void PlotWindow::DrawIndicator(wxDC& dc, PosMgr& pos_mgr, wxCoord &xpos, wxCoord &ypos)
{
    wxCoord x, y;
    size_t w, h;
    double xvalue, yvalue;
    pos_mgr.GetPos(row_pos, col_pos, x, y, w, h, mgr_pos, &dc);
    if (!GetIndicatePos(x, y, w, h, xpos, ypos, xvalue, yvalue, cur_curve))
        return;
    vector<wxCoord> children_x, children_y;
    for (vector<PlotWindow *>::iterator i = children.begin(); i != children.end(); ++i)
    {
      wxCoord xtmp;
      wxCoord ytmp;      
      if ((*i)->x_info_from >= 0 && (*i)->x_info_from < i-children.begin())
      {
        xtmp=children_x[(*i)->x_info_from];
        ytmp=children_y[(*i)->x_info_from];
        (*i)->DrawIndicator(dc, pos_mgr, xtmp, ytmp);
      }
      else
      {
        xtmp=xpos;
        ytmp=ypos;
        (*i)->DrawIndicator(dc, pos_mgr, xtmp, ytmp);
      }
      children_x.push_back(xtmp);
      children_y.push_back(ytmp);
    }
    DrawIndicator(dc, x, y, w, h, xpos, ypos, xvalue, yvalue);
}

void PlotWindow::DrawFocus(wxDC& dc, PosMgr& pos_mgr, wxCoord &xpos, wxCoord &ypos, int mask)
{
    wxCoord x, y;
    size_t w, h;
    double xvalue, yvalue;
    pos_mgr.GetPos(row_pos, col_pos, x, y, w, h, mgr_pos, &dc);
    DrawFocus(dc, x, y, w, h, xpos, ypos, mask);
    for (vector<PlotWindow *>::iterator i = children.begin(); i != children.end(); ++i)
    {
      (*i)->DrawFocus(dc, pos_mgr, xpos, ypos, mask);
    }
}

bool PlotWindow::SetLegendPos(PosMgr& pos_mgr, wxCoord &xpos, wxCoord &ypos, const int &no)
{
    wxCoord x, y;
    size_t w, h;
    double xvalue, yvalue;
    pos_mgr.GetPos(row_pos, col_pos, x, y, w, h, mgr_pos);
    bool flag;
    double max_ty = getMaxY();
    double max_tx = getMaxX();
    double min_ty = getMinY();
    double min_tx = getMinX();
    double xp, yp;
    double mx_max, my_max, mx_min, my_min;
    switch (axis_dir)
    {
      case X_RIGHT_Y_UP :
        {
           xp = (xpos-x)/double(w)*(max_x-min_x)+min_x;
           xp = (xp - min_tx) / (max_tx - min_tx);
           yp = max_y - (ypos-y)/double(h)*(max_y-min_y);
           yp = (max_ty - yp) / (max_ty - min_ty);
           mx_max = (max_x-max_tx)/(max_tx-min_tx);
           mx_min = (min_x-min_tx)/(max_tx-min_tx);
           my_max = (min_ty-min_y)/(max_ty-min_ty);
           my_min = (max_ty-max_y)/(max_ty-min_ty);
        }
        break;
      case X_RIGHT_Y_DOWN :
        {
           xp = (xpos-x)/double(w)*(max_x-min_x)+min_x;
           xp = (xp - min_tx) / (max_tx - min_tx);
           yp = (ypos-y)/double(h)*(max_y-min_y)+min_y;
           yp = (yp - min_ty) / (max_ty - min_ty);
           mx_max = (max_x-max_tx)/(max_tx-min_tx);
           mx_min = (min_x-min_tx)/(max_tx-min_tx);
           my_max = (max_y-max_ty)/(max_ty-min_ty);
           my_min = (min_y-min_ty)/(max_ty-min_ty);
        }
        break;
      case X_LEFT_Y_UP :
        {
           xp = max_x - (xpos-x)/double(w)*(max_x-min_x);
           xp = (max_tx - xp) / (max_tx - min_tx);
           yp = max_y - (ypos-y)/double(h)*(max_y-min_y);
           yp = (max_ty - yp) / (max_ty - min_ty);
           mx_max = (min_tx-min_x)/(max_tx-min_tx);
           mx_min = (max_tx-max_x)/(max_tx-min_tx);
           my_max = (min_ty-min_y)/(max_ty-min_ty);
           my_min = (max_ty-max_y)/(max_ty-min_ty);
        }
        break;
      case X_LEFT_Y_DOWN :
        {
           xp = max_x - (xpos-x)/double(w)*(max_x-min_x);
           xp = (max_tx - xp) / (max_tx - min_tx);
           yp = (ypos-y)/double(h)*(max_y-min_y)+min_y;
           yp = (yp - min_ty) / (max_ty - min_ty);
           mx_max = (min_tx-min_x)/(max_tx-min_tx);
           mx_min = (max_tx-max_x)/(max_tx-min_tx);
           my_max = (max_y-max_ty)/(max_ty-min_ty);
           my_min = (min_y-min_ty)/(max_ty-min_ty);
        }
        break;
      case X_UP_Y_LEFT :
        {
           yp = max_x - (ypos-y)/double(h)*(max_x-min_x);
           yp = (max_tx - yp) / (max_tx - min_tx);
           xp = max_y - (xpos-x)/double(w)*(max_y-min_y);
           xp = (max_ty - xp) / (max_ty - min_ty);
           my_max = (min_tx-min_x)/(max_tx-min_tx);
           my_min = (max_tx-max_x)/(max_tx-min_tx);
           mx_max = (min_ty-min_y)/(max_ty-min_ty);
           mx_min = (max_ty-max_y)/(max_ty-min_ty);
        }
        break;
      case X_UP_Y_RIGHT :
        {
           yp = max_x - (ypos-y)/double(h)*(max_x-min_x);
           yp = (max_tx - yp) / (max_tx - min_tx);
           xp = (xpos-x)/double(w)*(max_y-min_y)+min_y;
           xp = (xp - min_ty) / (max_ty - min_ty);
           my_max = (min_tx-min_x)/(max_tx-min_tx);
           my_min = (max_tx-max_x)/(max_tx-min_tx);
           mx_max = (max_y-max_ty)/(max_ty-min_ty);
           mx_min = (min_y-min_ty)/(max_ty-min_ty);
        }
        break;
      case X_DOWN_Y_LEFT :
        {
           yp = (ypos-y)/double(h)*(max_x-min_x)+min_x;
           yp = (yp - min_tx) / (max_tx - min_tx);
           xp = max_y - (xpos-x)/double(w)*(max_y-min_y);
           xp = (max_ty - xp) / (max_ty - min_ty);
           my_max = (max_x-max_tx)/(max_tx-min_tx);
           my_min = (min_x-min_tx)/(max_tx-min_tx);
           mx_max = (min_ty-min_y)/(max_ty-min_ty);
           mx_min = (max_ty-max_y)/(max_ty-min_ty);
        }
        break;
      case X_DOWN_Y_RIGHT :
        {
           yp = (ypos-y)/double(h)*(max_x-min_x)+min_x;
           yp = (yp - min_tx) / (max_tx - min_tx);
           xp = (xpos-x)/double(w)*(max_y-min_y)+min_y;
           xp = (xp - min_ty) / (max_ty - min_ty);
           my_max = (max_x-max_tx)/(max_tx-min_tx);
           my_min = (min_x-min_tx)/(max_tx-min_tx);
           mx_max = (max_y-max_ty)/(max_ty-min_ty);
           mx_min = (min_y-min_ty)/(max_ty-min_ty);
        }
        break;
    }
    if (xp>mx_min && xp<1+mx_max && yp>my_min && yp<1+my_max && no < legend_list.size())
    {
      flag = true;
      legend_list[no].pos_x = xp;
      legend_list[no].pos_y = yp;
    }
    else
    {
      flag = false;
    }
    if (!flag)
    {
      for (vector<PlotWindow *>::iterator i = children.begin(); i != children.end(); ++i)
      {
        if ((*i)->SetLegendPos(pos_mgr, xpos, ypos, no))
	{
	  flag = true;
	  break;
	}
      }
    }
    return flag;
}

bool PlotWindow::SetLegendVal(PosMgr& pos_mgr, wxCoord &xpos, wxCoord &ypos, const int &no)
{
    wxCoord x, y;
    size_t w, h;
    double xvalue, yvalue;
    pos_mgr.GetPos(row_pos, col_pos, x, y, w, h, mgr_pos);
    int dirx, diry, posx, posy, rangex, rangey;
    bool exchange;
    AxisDirMap(axis_dir, dirx, diry, posx, posy, rangex, rangey, exchange, x, y, w, h);
    bool flag;
    double max_ty = getMaxY();
    double max_tx = getMaxX();
    double min_ty = getMinY();
    double min_tx = getMinX();
    double xp, yp;
    switch (axis_dir)
    {
      case X_RIGHT_Y_UP :
        {
           xp = (xpos-x)/double(w)*(max_x-min_x)+min_x;
           xp = (xp - min_tx) / (max_tx - min_tx);
           yp = max_y - (ypos-y)/double(h)*(max_y-min_y);
           yp = (max_ty - yp) / (max_ty - min_ty);
        }
        break;
      case X_RIGHT_Y_DOWN :
        {
           xp = (xpos-x)/double(w)*(max_x-min_x)+min_x;
           xp = (xp - min_tx) / (max_tx - min_tx);
           yp = (ypos-y)/double(h)*(max_y-min_y)+min_y;
           yp = (yp - min_ty) / (max_ty - min_ty);
        }
        break;
      case X_LEFT_Y_UP :
        {
           xp = max_x - (xpos-x)/double(w)*(max_x-min_x);
           xp = (max_tx - xp) / (max_tx - min_tx);
           yp = max_y - (ypos-y)/double(h)*(max_y-min_y);
           yp = (max_ty - yp) / (max_ty - min_ty);
        }
        break;
      case X_LEFT_Y_DOWN :
        {
           xp = max_x - (xpos-x)/double(w)*(max_x-min_x);
           xp = (max_tx - xp) / (max_tx - min_tx);
           yp = (ypos-y)/double(h)*(max_y-min_y)+min_y;
           yp = (yp - min_ty) / (max_ty - min_ty);
        }
        break;
      case X_UP_Y_LEFT :
        {
           yp = max_x - (ypos-y)/double(h)*(max_x-min_x);
           yp = (max_tx - yp) / (max_tx - min_tx);
           xp = max_y - (xpos-x)/double(w)*(max_y-min_y);
           xp = (max_ty - xp) / (max_ty - min_ty);
        }
        break;
      case X_UP_Y_RIGHT :
        {
           yp = max_x - (ypos-y)/double(h)*(max_x-min_x);
           yp = (max_tx - yp) / (max_tx - min_tx);
           xp = (xpos-x)/double(w)*(max_y-min_y)+min_y;
           xp = (xp - min_ty) / (max_ty - min_ty);
        }
        break;
      case X_DOWN_Y_LEFT :
        {
           yp = (ypos-y)/double(h)*(max_x-min_x)+min_x;
           yp = (yp - min_tx) / (max_tx - min_tx);
           xp = max_y - (xpos-x)/double(w)*(max_y-min_y);
           xp = (max_ty - xp) / (max_ty - min_ty);
        }
        break;
      case X_DOWN_Y_RIGHT :
        {
           yp = (ypos-y)/double(h)*(max_x-min_x)+min_x;
           yp = (yp - min_tx) / (max_tx - min_tx);
           xp = (xpos-x)/double(w)*(max_y-min_y)+min_y;
           xp = (xp - min_ty) / (max_ty - min_ty);
        }
        break;
    }
    if (!exchange && xp>0 && xp<1 && yp>0 && yp<1 && no < legend_list.size())
    {
      flag = true;
      legend_list[no].value_x = xp;
    }
    else if (exchange && xp>0 && xp<1 && yp>0 && yp<1 && no < legend_list.size())
    {
      flag = true;
      legend_list[no].value_x = yp;    
    }
    else
    {
      flag = false;
    }
    if (!flag)
    {
      for (vector<PlotWindow *>::iterator i = children.begin(); i != children.end(); ++i)
      {
        if ((*i)->SetLegendVal(pos_mgr, xpos, ypos, no))
	{
	  flag = true;
	  break;
	}
      }
    }
    return flag;
}

bool PlotWindow::GetIndicatePos(const wxCoord &x, const wxCoord &y, const size_t &w, const size_t &h, wxCoord &xpos, wxCoord &ypos, double &xvalue, double &yvalue, Curve *curve)
{
  if (!curve)
    return false;

  int dirx, diry, posx, posy, rangex, rangey;
  bool exchange;
  AxisDirMap(axis_dir, dirx, diry, posx, posy, rangex, rangey, exchange, x, y, w, h);

  double sx = max_x-min_x;
  double sy = max_y-min_y;
  if (strange_mode)
  {
    if (x_pos_strange_mode < -0.0001 || x_pos_strange_mode > 1.0001)
      return false;
    if (!exchange)
    {
      if (ypos<y || ypos>y+(wxCoord)h)
        return false;
      xpos = x+wxCoord(x_pos_strange_mode*w+0.5);
      xvalue = (xpos - posx)/double(rangex)*sx*dirx+min_x;
      yvalue = (ypos - posy)/double(rangey)*sy*diry+min_y;
    }
    else
    {
      if (xpos<x || xpos>x+(wxCoord)w)
        return false;
      ypos = y+wxCoord(x_pos_strange_mode*h+0.5);
      xvalue = (ypos - posy)/double(rangey)*sy*diry+min_y;
      yvalue = (xpos - posx)/double(rangex)*sx*dirx+min_x;
    }
    vector<Curve *>::iterator i = curve_list.begin();
    vector<Curve *>::iterator j = curve_list.begin();
    ++i;
    for (; i != curve_list.end(); ++i,++j)
    {
      double tmp1 = (*i)->evaluate(x_base_strange_mode);
      double tmp2 = (*j)->evaluate(x_base_strange_mode);
      if (yvalue <=tmp1 && yvalue > tmp2 || yvalue >=tmp1 && yvalue < tmp2)
      {
        double tmp3 = (*i)->evaluate(xvalue);
        double tmp4 = (*j)->evaluate(xvalue);
	yvalue = (yvalue-tmp1)/(tmp2-tmp1)*(tmp4-tmp3) +tmp3;
        break;
      }
    }
    if (exchange)
    {
      xpos  = int((yvalue-min_y)/sy*rangey)*diry+posy;
      if (xpos<x || xpos>x+(wxCoord)w)
        return false;
    }
    else
    {
      ypos  = int((yvalue-min_y)/sy*rangey)*diry+posy;
      if (ypos<y || ypos>y+(wxCoord)h)
         return false;
    }
    
    return true;
  }
  if (exchange)
  {
    if (ypos<y || ypos>y+(wxCoord)h)
        return false;
    xvalue = (ypos - posx)/double(rangex)*sx*dirx+min_x;
  }
  else
  {
    if (xpos<x || xpos>x+(wxCoord)w)
        return false;
    xvalue = (xpos - posx)/double(rangex)*sx*dirx+min_x;
  }
  yvalue = curve->evaluate(xvalue);
  if (exchange)
  {
    xpos  = int((yvalue-min_y)/sy*rangey)*diry+posy;
    if (xpos<x || xpos>x+(wxCoord)w)
        return false;
  }
  else
  {
    ypos  = int((yvalue-min_y)/sy*rangey)*diry+posy;
    if (ypos<y || ypos>y+(wxCoord)h)
        return false;
  }

  return true;
}


void PlotWindow::DrawIndicator(wxDC& dc, const wxCoord &x, const wxCoord &y, const size_t &w, const size_t &h, const wxCoord &xpos, const wxCoord &ypos, const double &xvalue, const double &yvalue)
{
  dc.SetPen(*wxGREY_PEN);

  // Calculate a suitable scaling factor
  double scaleX=(double)(ppi.GetWidth()/ppi_x);
  double scaleY=(double)(ppi.GetHeight()/ppi_y);
  // Use x or y scaling factor, whichever fits on the DC
  double actualScale = wxMin(scaleX,scaleY);
  // Set the scale
  wxFont tmp_font = fnt_axis;
  tmp_font.SetPointSize(int(tmp_font.GetPointSize()*actualScale+0.5));

  dc.SetFont(tmp_font);
  dc.SetTextForeground(pen_axis.GetColour());

  wxString xstr = wxString::Format(wxT("%g"), xvalue);
  wxString ystr = wxString::Format(wxT("%g"), yvalue);
  wxCoord xsw, xsh, ysw, ysh;
  dc.GetTextExtent(xstr, &xsw, &xsh);
  dc.GetTextExtent(ystr, &ysw, &ysh);

  if (axis_dir == X_LEFT_Y_UP || axis_dir == X_RIGHT_Y_UP || axis_dir == X_LEFT_Y_DOWN || axis_dir == X_RIGHT_Y_DOWN)
  {
    /*
    if (strange_mode)
    {
            dc.DrawLine(x+x_pos_strange_mode*w, y, x+x_pos_strange_mode*w, y+h);
	    return;
    }
    */
    switch (axis_y_pos)
    {
        case LOW_OUT :
            dc.DrawLine(xpos, ypos, x, ypos);
            if (!axis_y_hide)
            {
            dc.DrawRectangle(x-ysw-4, ypos-ysh/2-1, ysw+2, ysh+2);
            dc.DrawText(ystr, x-ysw-3, ypos-ysh/2);
            }
            break;
        case LOW_IN :
            dc.DrawLine(xpos, ypos, x, ypos);
            if (!axis_y_hide)
            {
            dc.DrawRectangle(x+2, ypos-ysh/2-1, ysw+2, ysh+2);
            dc.DrawText(ystr, x+3, ypos-ysh/2);
            }
            break;
        case HIGH_IN :
            dc.DrawLine(xpos, ypos, x+w, ypos);
            if (!axis_y_hide)
            {
            dc.DrawRectangle(x+w-ysw-4, ypos-ysh/2-1, ysw+2, ysh+2);
            dc.DrawText(ystr, x+w-ysw-3, ypos-ysh/2);
            }
            break;
        case HIGH_OUT :
            dc.DrawLine(xpos, ypos, x+w, ypos);
            if (!axis_y_hide)
            {
            dc.DrawRectangle(x+w+2, ypos-ysh/2-1, ysw+2, ysh+2);
            dc.DrawText(ystr, x+w+3, ypos-ysh/2);
            }
            break;
    }
      switch (axis_x_pos)
      {
        case LOW_OUT :
            dc.DrawLine(xpos, ypos, xpos, y);
            if (!axis_x_hide)
            {
            dc.DrawRectangle(xpos-xsw/2-1, y-xsh-4, xsw+2, xsh+2);
            dc.DrawText(xstr, xpos-xsw/2, y-xsh-3);
            }
            break;
        case LOW_IN :
            dc.DrawLine(xpos, ypos, xpos, y);
            if (!axis_x_hide)
            {
            dc.DrawRectangle(xpos-xsw/2-1, y+2, xsw+2, xsh+2);
            dc.DrawText(xstr, xpos-xsw/2, y+3);
            }
            break;
        case HIGH_IN :
            dc.DrawLine(xpos, ypos, xpos, y+h);
            if (!axis_x_hide)
            {
            dc.DrawRectangle(xpos-xsw/2-1, y+h-xsh-4, xsw+2, xsh+2);
            dc.DrawText(xstr, xpos-xsw/2, y+h-xsh-3);
            }
            break;
        case HIGH_OUT :
            dc.DrawLine(xpos, ypos, xpos, y+h);
            if (!axis_x_hide)
            {
            dc.DrawRectangle(xpos-xsw/2-1, y+h+2, xsw+2, xsh+2);
            dc.DrawText(xstr, xpos-xsw/2, y+h+3);
            }
            break;
      }
  } 
  else if (axis_dir == X_UP_Y_LEFT || axis_dir == X_DOWN_Y_LEFT || axis_dir == X_UP_Y_RIGHT || axis_dir == X_DOWN_Y_RIGHT)
  {
    /*
    if (strange_mode)
    {
            dc.DrawLine(x, y+x_pos_strange_mode*h, x+w, y+x_pos_strange_mode*h);
	    return;
    }
    */
      switch (axis_y_pos)
      {
        case LOW_OUT :
            dc.DrawLine(xpos, ypos, xpos, y);
            if (!axis_y_hide)
            {
            dc.DrawRectangle(xpos-ysw/2-1, y-ysh-4, ysw+2, ysh+2);
            dc.DrawText(ystr, xpos-ysw/2, y-ysh-3);
            }
            break;
        case LOW_IN :
            dc.DrawLine(xpos, ypos, xpos, y);
            if (!axis_y_hide)
            {
            dc.DrawRectangle(xpos-ysw/2-1, y+2, ysw+2, ysh+2);
            dc.DrawText(ystr, xpos-ysw/2, y+3);
            }
            break;
        case HIGH_IN :
            dc.DrawLine(xpos, ypos, xpos, y+h);
            if (!axis_y_hide)
            {
            dc.DrawRectangle(xpos-ysw/2-1, y+h-ysh-4, ysw+2, ysh+2);
            dc.DrawText(ystr, xpos-ysw/2, y+h-ysh-3);
            }
            break;
        case HIGH_OUT :
            dc.DrawLine(xpos, ypos, xpos, y+h);
            if (!axis_y_hide)
            {
            dc.DrawRectangle(xpos-ysw/2-1, y+h+2, ysw+2, ysh+2);
            dc.DrawText(ystr, xpos-ysw/2, y+h+3);
            }
            break;
      }
      switch (axis_x_pos)
      {
        case LOW_OUT :
            dc.DrawLine(xpos, ypos, x, ypos);
            if (!axis_x_hide)
            {
            dc.DrawRectangle(x-xsw-4, ypos-xsh/2-1, xsw+2, xsh+2);
            dc.DrawText(xstr, x-xsw-3, ypos-xsh/2);
            }
            break;
        case LOW_IN :
            dc.DrawLine(xpos, ypos, x, ypos);
            if (!axis_x_hide)
            {
            dc.DrawRectangle(x+2, ypos-xsh/2-1, xsw+2, xsh+2);
            dc.DrawText(xstr, x+3, ypos-xsh/2);
            }
            break;
        case HIGH_IN :
            dc.DrawLine(xpos, ypos, x+w, ypos);
            if (!axis_x_hide)
            {
            dc.DrawRectangle(x+w-xsw-4, ypos-xsh/2-1, xsw+2, xsh+2);
            dc.DrawText(xstr, x+w-xsw-3, ypos-xsh/2);
            }
            break;
        case HIGH_OUT :
            dc.DrawLine(xpos, ypos, x+w, ypos);
            if (!axis_x_hide)
            {
            dc.DrawRectangle(x+w+2, ypos-xsh/2-1, xsw+2, xsh+2);
            dc.DrawText(xstr, x+w+3, ypos-xsh/2);
            }
            break;
      }
  }
}

void PlotWindow::DrawFocus(wxDC& dc, const wxCoord &x, const wxCoord &y, const size_t &w, const size_t &h, const wxCoord &xpos, const wxCoord &ypos, int mask)
{
 if (mask & Focus::CURVE && xpos > x && xpos < x+(wxCoord)w && ypos > y && ypos < y+(wxCoord)h)
 {
  int dirx, diry, posx, posy, rangex, rangey;
  bool exchange;
  AxisDirMap(axis_dir, dirx, diry, posx, posy, rangex, rangey, exchange, x, y, w, h);

  double sx = max_x-min_x;
  double sy = max_y-min_y;
  double xvalue;
  wxCoord xpos_new = xpos;
  wxCoord ypos_new = ypos;
  if (exchange)
  {
    xvalue = (ypos - posx)/double(rangex)*sx*dirx+min_x;
  }
  else
  {
    xvalue = (xpos - posx)/double(rangex)*sx*dirx+min_x;
  }
  for (vector<Curve *>::iterator i = curve_list.begin();
          i != curve_list.end(); ++i)
  {
    double yvalue = (*i)->evaluate(xvalue);
    if (exchange)
    {
      xpos_new  = int((yvalue-min_y)/sy*rangey)*diry+posy;
    }
    else
    {
      ypos_new  = int((yvalue-min_y)/sy*rangey)*diry+posy;
    }
    if ( (xpos_new-xpos)*(xpos_new-xpos)+(ypos_new-ypos)*(ypos_new-ypos) < 10)
    {
        wxPen old_pen = (*i)->GetPen();
        wxPen old_pen2 = dc.GetPen();
        (*i)->SetPen(wxPen(*wxRED, 3));
        (*i)->DrawCurve(dc, x,y,w,h,min_x,max_x,min_y,max_y, axis_dir);
        (*i)->SetPen(old_pen);
        dc.SetPen(old_pen2);
        Focus tmp;
        tmp.window = this;
        tmp.type = Focus::CURVE;
        tmp.iter = i - curve_list.begin();
        handler->AddFocus(tmp);
        break;
    }
  }
 }

  wxRect r(xpos-2, ypos-2, 4, 4);
  if (mask & Focus::AXISY && !axis_y_hide && r.Intersects(rect_axis_y))
  {
    wxPoint p[5];
    p[0].x = rect_axis_y.GetX();
    p[0].y = rect_axis_y.GetY();
    p[1].x = p[0].x+rect_axis_y.GetWidth();
    p[1].y = p[0].y;
    p[2].x = p[1].x;
    p[2].y = p[1].y+rect_axis_y.GetHeight();
    p[3].x = p[0].x;
    p[3].y = p[2].y;
    p[4].x = p[0].x;
    p[4].y = p[0].y;
    dc.DrawLines(5, p);
    Focus tmp;
    tmp.window = this;
    tmp.type = Focus::AXISY;
    tmp.iter = 0;
    handler->AddFocus(tmp);
  }
  if (mask & Focus::AXISX && !axis_x_hide && r.Intersects(rect_axis_x))
  {
    wxPoint p[5];
    p[0].x = rect_axis_x.GetX();
    p[0].y = rect_axis_x.GetY();
    p[1].x = p[0].x+rect_axis_x.GetWidth();
    p[1].y = p[0].y;
    p[2].x = p[1].x;
    p[2].y = p[1].y+rect_axis_x.GetHeight();
    p[3].x = p[0].x;
    p[3].y = p[2].y;
    p[4].x = p[0].x;
    p[4].y = p[0].y;
    dc.DrawLines(5, p);
    Focus tmp;
    tmp.window = this;
    tmp.type = Focus::AXISX;
    tmp.iter = 0;
    handler->AddFocus(tmp);
  }

  if (mask & Focus::LEGEND)
  for (LegendListCIter i=legend_list.begin(); i != legend_list.end(); ++i)
  {
   if (r.Intersects(i->rect))
   {
    wxPoint p[5];
    p[0].x = i->rect.GetX();
    p[0].y = i->rect.GetY();
    p[1].x = p[0].x+i->rect.GetWidth();
    p[1].y = p[0].y;
    p[2].x = p[1].x;
    p[2].y = p[1].y+i->rect.GetHeight();
    p[3].x = p[0].x;
    p[3].y = p[2].y;
    p[4].x = p[0].x;
    p[4].y = p[0].y;
    dc.DrawLines(5, p);
    Focus tmp;
    tmp.window = this;
    tmp.type = Focus::LEGEND;
    tmp.iter = i-legend_list.begin();
    handler->AddFocus(tmp);
   }
  }

  if (mask & Focus::WINDOW && xpos >= x && xpos <= x+w && ypos >= y && ypos <= y+h)
  {
    wxPoint p[5];
    p[0].x = x;
    p[0].y = y;
    p[1].x = x+w;
    p[1].y = y;
    p[2].x = x+w;
    p[2].y = y+h;
    p[3].x = x;
    p[3].y = y+h;
    p[4].x = x;
    p[4].y = y;
    dc.DrawLines(5, p);
    Focus tmp;
    tmp.window = this;
    tmp.type = Focus::WINDOW;
    tmp.iter = 0u;
    handler->AddFocus(tmp);
  }
}

bool PlotWindow::SelectCurrentCurve(PosMgr& pos_mgr, const wxCoord &xpos, const wxCoord &ypos)
{
    wxCoord x, y;
    size_t w, h;
    pos_mgr.GetPos(row_pos, col_pos, x, y, w, h);
    bool rslt = SelectCurrentCurve(x, y, w, h, xpos, ypos);
    for (vector<PlotWindow *>::iterator i = children.begin(); i != children.end(); ++i)
    {
      rslt = rslt || (*i)->SelectCurrentCurve(pos_mgr, xpos, ypos);
    }
    return rslt;
}

bool PlotWindow::SelectCurrentCurve(const wxCoord &x, const wxCoord &y, const size_t &w, const size_t &h, const wxCoord &xpos, const wxCoord &ypos)
{
  if (xpos <= x || xpos >= x+(wxCoord)w || ypos <= y || ypos >= y+(wxCoord)h)
    return false;

  int dirx, diry, posx, posy, rangex, rangey;
  bool exchange;
  AxisDirMap(axis_dir, dirx, diry, posx, posy, rangex, rangey, exchange, x, y, w, h);

  double sx = max_x-min_x;
  double sy = max_y-min_y;
  double xvalue;
  wxCoord xpos_new = xpos;
  wxCoord ypos_new = ypos;
  if (exchange)
  {
    xvalue = (ypos - posx)/double(rangex)*sx*dirx+min_x;
  }
  else
  {
    xvalue = (xpos - posx)/double(rangex)*sx*dirx+min_x;
  }
  if (strange_mode)
  {
    if (exchange)
    {
      x_pos_strange_mode = (ypos - y)/double(h);
    }
    else
    {
      x_pos_strange_mode = (xpos - x)/double(w);
    }
    return true;
  }
  for (vector<Curve *>::iterator i = curve_list.begin();
          i != curve_list.end(); ++i)
  {
    double yvalue = (*i)->evaluate(xvalue);
    if (exchange)
    {
      xpos_new  = int((yvalue-min_y)/sy*rangey)*diry+posy;
    }
    else
    {
      ypos_new  = int((yvalue-min_y)/sy*rangey)*diry+posy;
    }
    if ( (xpos_new-xpos)*(xpos_new-xpos)+(ypos_new-ypos)*(ypos_new-ypos) < 10)
    {
      if ((*i)!=cur_curve)
      {
        cur_curve = (*i);
        return true;
      }
    }
  }

  return false;
}


wxRect PlotWindow::draw_vertical_axis(wxDC& dc, const wxCoord &x, const wxCoord &y, const wxCoord &yh, const double &min, const double &max, vector<double> &mark_list, const wxString & text, const bool &right)
{
  wxRect rslt;
  rslt.SetX(x-3);
  rslt.SetWidth(6);

  wxCoord dir;
  if (yh > y)
  {
      dir = 1;
      rslt.SetY(y-2);
      rslt.SetHeight(yh-y+4);
  }
  else
  {
      dir = -1;
      rslt.SetY(yh-2);
      rslt.SetHeight(y-yh+4);
  }

  wxCoord mw, mh;
  DrawTexText(dc, text, x+3, yh, mw, mh, false);

  // Calculate a suitable scaling factor
  double scaleX=(double)(ppi.GetWidth()/ppi_x);
  double scaleY=(double)(ppi.GetHeight()/ppi_y);
  // Use x or y scaling factor, whichever fits on the DC
  double actualScale = wxMin(scaleX,scaleY);
  dc.DrawLine(x, y, x, yh-dir*mh);
  wxPoint p[3];
  p[0].x = x+int(3*actualScale+0.5);
  p[0].y = yh-dir*mh-dir*int(6*actualScale+0.5);
  p[1].x = x;
  p[1].y = yh-dir*mh;
  p[2].x = x-int(3*actualScale+0.5);
  p[2].y = p[0].y;
  dc.DrawPolygon(3, p);

  wxCoord dw;
  if (right)
    dw = -int(3*actualScale+0.5);
  else
    dw = int(3*actualScale+0.5);

  mh /= 2;
  if (right)
  {
    DrawTexText(dc, text, x+int(3*actualScale+0.5), yh-dir*mh-mh, mw, mh);
  }
  else
  {
    DrawTexText(dc, text, x-mw-int(3*actualScale+0.5), yh-dir*mh-mh, mw, mh);
  }

  wxCoord last = -9999;

  double scale=(yh-y)/(max-min);

  for (vector< double >::iterator i = mark_list.begin(); i != mark_list.end(); ++i)
  {
    wxString str = wxString::Format(wxT("%g"), *i);
    dc.GetTextExtent(str, &mw, &mh);
    wxCoord mark_pos = y+wxCoord(scale*(*i-min));
    if (abs(mark_pos+dir*mh-y) > abs(yh-dir*mh-y))
    {
      break;
    }
    dc.DrawLine(x, mark_pos, x+dw, mark_pos);
    if (last < dir*(mark_pos-y-int(2*actualScale+0.5)))
    {
      if (right)
      {
        dc.DrawText(str, x+int(3*actualScale+0.5), mark_pos-mh/2+dir*mh/2);
        rslt.Union(wxRect(x+int(actualScale+0.5), mark_pos-mh/2+dir*mh/2-int(2*actualScale+0.5), mw+int(4*actualScale+0.5), mh+int(4*actualScale+0.5)));
      }
      else
      {
        dc.DrawText(str, x-mw-int(3*actualScale+0.5), mark_pos-mh/2+dir*mh/2);
        rslt.Union(wxRect(x-mw-int(5*actualScale+0.5), mark_pos-mh/2+dir*mh/2-int(2*actualScale+0.5), mw+int(4*actualScale+0.5), mh+int(4*actualScale+0.5)));
      }
      last = dir*(mark_pos-y+dir*mh);
    }
  }

  return rslt;
}

wxRect PlotWindow::draw_horizon_axis(wxDC& dc, const wxCoord &x, const wxCoord &y, const wxCoord &xh, const double &min, const double &max, vector<double> &mark_list, const wxString & text, const bool &bottom)
{
  wxRect rslt;
  rslt.SetY(y-3);
  rslt.SetHeight(6);

  wxCoord dir;
  if (xh > x)
  {
      dir = 1;
      rslt.SetX(x-2);
      rslt.SetWidth(xh-x+4);
  }
  else
  {
      dir = -1;
      rslt.SetX(xh-2);
      rslt.SetWidth(x-xh+4);
  }

  // Calculate a suitable scaling factor
  double scaleX=(double)(ppi.GetWidth()/ppi_x);
  double scaleY=(double)(ppi.GetHeight()/ppi_y);
  // Use x or y scaling factor, whichever fits on the DC
  double actualScale = wxMin(scaleX,scaleY);
  dc.DrawLine(x, y, xh, y);
  wxPoint p[3];
  p[0].x = xh-int(dir*6*actualScale+0.5);
  p[0].y = y+int(3*actualScale+0.5);
  p[1].x = xh;
  p[1].y = y;
  p[2].x = p[0].x;
  p[2].y = y-int(3*actualScale+0.5);
  dc.DrawPolygon(3, p);

  wxCoord dh;
  if (bottom)
    dh = -int(3*actualScale+0.5);
  else
    dh = int(3*actualScale+0.5);

  if (mark_list.size() > 3u)
  {

  wxCoord mw, mh, tw;
  DrawTexText(dc, text, xh, y+int(3*actualScale+0.5), tw, mh, false);
  if (bottom)
  {
    DrawTexText(dc, text, xh-tw/2-dir*(tw/2+int(3*actualScale+0.5)), y+int(3*actualScale+0.5), tw, mh);
  }
  else
  {
    DrawTexText(dc, text, xh-tw/2-dir*(tw/2+int(3*actualScale+0.5)), y-mh-int(3*actualScale+0.5), tw, mh);
  }

  wxCoord last = 0;

  double scale=(xh-x)/(max-min);

  for (vector< double >::iterator i = mark_list.begin(); i != mark_list.end(); ++i)
  {
    wxString str = wxString::Format(wxT("%g"), *i);
    dc.GetTextExtent(str, &mw, &mh);
    wxCoord mark_pos = x+wxCoord(scale*(*i-min));
    if (abs(mark_pos+dir*mw-x) > abs(xh-dir*tw-x))
    {
      break;
    }
    dc.DrawLine(mark_pos, y, mark_pos, y+dh);
    if (last < dir*(mark_pos-x-int(3*actualScale+0.5)))
    {
      if (bottom)
      {
        dc.DrawText(str, mark_pos-mw/2+dir*mw/2, y+int(3*actualScale+0.5));
        rslt.Union(wxRect(mark_pos-mw/2+dir*mw/2-2, y+1, mw+4, mh+4));
      }
      else
      {
        dc.DrawText(str, mark_pos-mw/2+dir*mw/2, y-mh-int(3*actualScale+0.5));
        rslt.Union(wxRect(mark_pos-mw/2+dir*mw/2-2, y-mh-5, mw+4, mh+4));
      }
      last = dir*(mark_pos-x+dir*mw);
    }
  }
  }
  else
  {

  wxCoord mw, mh, tw;
  DrawTexText(dc, text, xh, y+int(3*actualScale+0.5), tw, mh, false);
  if (bottom)
  {
    DrawTexText(dc, text, xh-tw/2+dir*(tw/2+int(3*actualScale+0.5)), y+int(3*actualScale+0.5), tw, mh);
  }
  else
  {
    DrawTexText(dc, text, xh-tw/2+dir*(tw/2+int(3*actualScale+0.5)), y-mh-int(3*actualScale+0.5), tw, mh);
  }

  wxCoord last = 0;

  double scale=(xh-x)/(max-min);

  for (vector< double >::iterator i = mark_list.begin(); i != mark_list.end(); ++i)
  {
    wxString str = wxString::Format(wxT("%g"), *i);
    dc.GetTextExtent(str, &mw, &mh);
    wxCoord mark_pos = x+wxCoord(scale*(*i-min));
    dc.DrawLine(mark_pos, y, mark_pos, y+dh);
    if (last < dir*(mark_pos-x-int(3*actualScale+0.5)))
    {
      if (bottom)
      {
        dc.DrawText(str, mark_pos-mw/2-dir*mw/2, y+int(3*actualScale+0.5));
        rslt.Union(wxRect(mark_pos-mw/2-dir*mw/2-int(2*actualScale+0.5), y+int(1*actualScale+0.5), mw+int(4*actualScale+0.5), mh+int(4*actualScale+0.5)));
      }
      else
      {
        dc.DrawText(str, mark_pos-mw/2-dir*mw/2, y-mh-int(3*actualScale+0.5));
        rslt.Union(wxRect(mark_pos-mw/2-dir*mw/2-int(2*actualScale+0.5), y-mh-int(5*actualScale+0.5), mw+int(4*actualScale+0.5), mh+int(4*actualScale+0.5)));
      }
      last = dir*(mark_pos-x+dir*mw);
    }
  }
  }

  return rslt;
}

void PlotWindow::draw_y_axis(wxDC& dc, const wxCoord &x, const wxCoord &y, const size_t &w, const size_t &h)
{
  // Calculate a suitable scaling factor
  double scaleX=(double)(ppi.GetWidth()/ppi_x);
  double scaleY=(double)(ppi.GetHeight()/ppi_y);
  // Use x or y scaling factor, whichever fits on the DC
  double actualScale = wxMin(scaleX,scaleY);
  // Set the scale
  wxFont tmp_font = fnt_axis;
  tmp_font.SetPointSize(int(tmp_font.GetPointSize()*actualScale+0.5));
  dc.SetFont(tmp_font);
  wxPen tmp_pen = pen_axis;
  tmp_pen.SetWidth(int(pen_axis.GetWidth()*actualScale+0.5));
  dc.SetPen(tmp_pen);
  dc.SetBrush(wxBrush(tmp_pen.GetColour()));
  dc.SetTextForeground(tmp_pen.GetColour());

  wxCoord xh=x+w;
  wxCoord yh=y+h;
  
  if (axis_dir == X_LEFT_Y_UP || axis_dir == X_RIGHT_Y_UP)
  {
    switch (axis_y_pos)
    {
        case LOW_OUT :
            rect_axis_y = draw_vertical_axis(dc, x, yh, y, min_y, max_y, y_mark_list, str_y_label, false);
            break;
        case LOW_IN :
            rect_axis_y = draw_vertical_axis(dc, x, yh, y, min_y, max_y, y_mark_list, str_y_label, true);
            break;
        case HIGH_IN :
            rect_axis_y = draw_vertical_axis(dc, xh, yh, y, min_y, max_y, y_mark_list, str_y_label, false);
            break;
        case HIGH_OUT :
            rect_axis_y = draw_vertical_axis(dc, xh, yh, y, min_y, max_y, y_mark_list, str_y_label, true);
            break;
        default :
            break;
    }
  } 
  else if (axis_dir == X_LEFT_Y_DOWN || axis_dir == X_RIGHT_Y_DOWN)
  {
    switch (axis_y_pos)
    {
        case LOW_OUT :
            rect_axis_y = draw_vertical_axis(dc, x, y, yh, min_y, max_y, y_mark_list, str_y_label, false);
            break;
        case LOW_IN :
            rect_axis_y = draw_vertical_axis(dc, x, y, yh, min_y, max_y, y_mark_list, str_y_label, true);
            break;
        case HIGH_IN :
            rect_axis_y = draw_vertical_axis(dc, xh, y, yh, min_y, max_y, y_mark_list, str_y_label, false);
            break;
        case HIGH_OUT :
            rect_axis_y = draw_vertical_axis(dc, xh, y, yh, min_y, max_y, y_mark_list, str_y_label, true);
            break;
        default :
            break;
    }
  } 
  else if (axis_dir == X_UP_Y_LEFT || axis_dir == X_DOWN_Y_LEFT)
  {
    switch (axis_y_pos)
    {
        case LOW_OUT :
            rect_axis_y = draw_horizon_axis(dc, xh, y, x, min_y, max_y, y_mark_list, str_y_label, false);
            break;
        case LOW_IN :
            rect_axis_y = draw_horizon_axis(dc, xh, y, x, min_y, max_y, y_mark_list, str_y_label, true);
            break;
        case HIGH_IN :
            rect_axis_y = draw_horizon_axis(dc, xh, yh, x, min_y, max_y, y_mark_list, str_y_label, false);
            break;
        case HIGH_OUT :
            rect_axis_y = draw_horizon_axis(dc, xh, yh, x, min_y, max_y, y_mark_list, str_y_label, true);
            break;
        default :
            break;
    }
  }
  else if (axis_dir == X_UP_Y_RIGHT || axis_dir == X_DOWN_Y_RIGHT)
  {
    switch (axis_y_pos)
    {
        case LOW_OUT :
            rect_axis_y = draw_horizon_axis(dc, x, y, xh, min_y, max_y, y_mark_list, str_y_label, false);
            break;
        case LOW_IN :
            rect_axis_y = draw_horizon_axis(dc, x, y, xh, min_y, max_y, y_mark_list, str_y_label, true);
            break;
        case HIGH_IN :
            rect_axis_y = draw_horizon_axis(dc, x, yh, xh, min_y, max_y, y_mark_list, str_y_label, false);
            break;
        case HIGH_OUT :
            rect_axis_y = draw_horizon_axis(dc, x, yh, xh, min_y, max_y, y_mark_list, str_y_label, true);
            break;
        default :
            break;
    }
  }
}

void PlotWindow::draw_x_axis(wxDC& dc, const wxCoord &x, const wxCoord &y, const size_t &w, const size_t &h)
{
  // Calculate a suitable scaling factor
  double scaleX=(double)(ppi.GetWidth()/ppi_x);
  double scaleY=(double)(ppi.GetHeight()/ppi_y);
  // Use x or y scaling factor, whichever fits on the DC
  double actualScale = wxMin(scaleX,scaleY);
  // Set the scale
  wxFont tmp_font = fnt_axis;
  tmp_font.SetPointSize(int(tmp_font.GetPointSize()*actualScale+0.5));
  dc.SetFont(tmp_font);
  wxPen tmp_pen = pen_axis;
  tmp_pen.SetWidth(int(pen_axis.GetWidth()*actualScale+0.5));
  dc.SetPen(tmp_pen);
  dc.SetBrush(wxBrush(tmp_pen.GetColour()));
  dc.SetTextForeground(tmp_pen.GetColour());

  wxCoord xh=x+w;
  wxCoord yh=y+h;
  
  if (axis_dir == X_UP_Y_LEFT || axis_dir == X_UP_Y_RIGHT)
  {
    switch (axis_x_pos)
    {
        case LOW_OUT :
            rect_axis_x = draw_vertical_axis(dc, x, yh, y, min_x, max_x, x_mark_list, str_x_label, false);
            break;
        case LOW_IN :
            rect_axis_x = draw_vertical_axis(dc, x, yh, y, min_x, max_x, x_mark_list, str_x_label, true);
            break;
        case HIGH_IN :
            rect_axis_x = draw_vertical_axis(dc, xh, yh, y, min_x, max_x, x_mark_list, str_x_label, false);
            break;
        case HIGH_OUT :
            rect_axis_x = draw_vertical_axis(dc, xh, yh, y, min_x, max_x, x_mark_list, str_x_label, true);
            break;
        default :
            break;
    }
  } 
  else if (axis_dir == X_DOWN_Y_LEFT || axis_dir == X_DOWN_Y_RIGHT)
  {
    switch (axis_x_pos)
    {
        case LOW_OUT :
            rect_axis_x = draw_vertical_axis(dc, x, y, yh, min_x, max_x, x_mark_list, str_x_label, false);
            break;
        case LOW_IN :
            rect_axis_x = draw_vertical_axis(dc, x, y, yh, min_x, max_x, x_mark_list, str_x_label, true);
            break;
        case HIGH_IN :
            rect_axis_x = draw_vertical_axis(dc, xh, y, yh, min_x, max_x, x_mark_list, str_x_label, false);
            break;
        case HIGH_OUT :
            rect_axis_x = draw_vertical_axis(dc, xh, y, yh, min_x, max_x, x_mark_list, str_x_label, true);
            break;
        default :
            break;
    }
  } 
  else if (axis_dir == X_LEFT_Y_UP || axis_dir == X_LEFT_Y_DOWN)
  {
    switch (axis_x_pos)
    {
        case LOW_OUT :
            rect_axis_x = draw_horizon_axis(dc, xh, y, x, min_x, max_x, x_mark_list, str_x_label, false);
            break;
        case LOW_IN :
            rect_axis_x = draw_horizon_axis(dc, xh, y, x, min_x, max_x, x_mark_list, str_x_label, true);
            break;
        case HIGH_IN :
            rect_axis_x = draw_horizon_axis(dc, xh, yh, x, min_x, max_x, x_mark_list, str_x_label, false);
            break;
        case HIGH_OUT :
            rect_axis_x = draw_horizon_axis(dc, xh, yh, x, min_x, max_x, x_mark_list, str_x_label, true);
            break;
        default :
            break;
    }
  }
  else if (axis_dir == X_RIGHT_Y_UP || axis_dir == X_RIGHT_Y_DOWN)
  {
    switch (axis_x_pos)
    {
        case LOW_OUT :
            rect_axis_x = draw_horizon_axis(dc, x, y, xh, min_x, max_x, x_mark_list, str_x_label, false);
            break;
        case LOW_IN :
            rect_axis_x = draw_horizon_axis(dc, x, y, xh, min_x, max_x, x_mark_list, str_x_label, true);
            break;
        case HIGH_IN :
            rect_axis_x = draw_horizon_axis(dc, x, yh, xh, min_x, max_x, x_mark_list, str_x_label, false);
            break;
        case HIGH_OUT :
            rect_axis_x = draw_horizon_axis(dc, x, yh, xh, min_x, max_x, x_mark_list, str_x_label, true);
            break;
        default :
            break;
    }
  }
}

void PlotWindow::draw_curve(wxDC& dc, const wxCoord &x, const wxCoord &y, const size_t &w, const size_t &h)
{
  if (handler->IsDrawIndicator() && strange_mode)
  {
    if (axis_dir == X_LEFT_Y_UP || axis_dir == X_RIGHT_Y_UP || axis_dir == X_LEFT_Y_DOWN || axis_dir == X_RIGHT_Y_DOWN)
    {
      dc.DrawLine(x+wxCoord(x_pos_strange_mode*w+0.5), y, x+wxCoord(x_pos_strange_mode*w+0.5), y+h);
    }
    else if (axis_dir == X_UP_Y_LEFT || axis_dir == X_DOWN_Y_LEFT || axis_dir == X_UP_Y_RIGHT || axis_dir == X_DOWN_Y_RIGHT)
    {
      dc.DrawLine(x, y+wxCoord(x_pos_strange_mode*h+0.5), x+w, y+wxCoord(x_pos_strange_mode*h+0.5));
    }
  }
  for (vector<Curve *>::iterator i = curve_list.begin(); i != curve_list.end(); ++i)
  {
    if (handler->IsDrawIndicator() && *i == cur_curve && !strange_mode)
    {
      wxPen old_pen = cur_curve->GetPen();
      cur_curve->SetPen(wxPen(*wxRED, old_pen.GetWidth()*2));
      cur_curve->DrawCurve(dc, x, y, w, h, min_x, max_x, min_y, max_y, axis_dir);
      cur_curve->SetPen(old_pen);
    }
    else
    {
      (*i)->DrawCurve(dc, x, y, w, h, min_x, max_x, min_y, max_y, axis_dir);
    }
  }
}

void PlotWindow::draw_legend(wxDC& dc, const wxCoord &x, const wxCoord &y, const size_t &w, const size_t &h, LegendListIter & iter)
{
    unsigned int no = iter - legend_list.begin();
    double xvalue, yvalue;
    wxCoord xpos;
    wxCoord ypos;
    wxCoord xlegend;
    wxCoord ylegend;
    bool x_horizontal_dir;
    double max_ty = getMaxY();
    double max_tx = getMaxX();
    double min_ty = getMinY();
    double min_tx = getMinX();
    if (axis_dir == X_RIGHT_Y_UP || axis_dir == X_RIGHT_Y_DOWN 
            || axis_dir == X_LEFT_Y_UP || axis_dir == X_LEFT_Y_DOWN)
    {
      if (axis_dir == X_RIGHT_Y_UP || axis_dir == X_RIGHT_Y_DOWN)
      {
        xpos = x + (wxCoord)(w*(iter->value_x*(max_tx-min_tx)+min_tx-min_x)/(max_x - min_x));
        xlegend = x + (wxCoord)(w*(iter->pos_x*(max_tx-min_tx)+min_tx-min_x)/(max_x - min_x));
      }
      else
      {
        xpos = x + (wxCoord)(w*(max_x - max_tx + iter->value_x*(max_tx-min_tx))/(max_x - min_x));
        xlegend = x + (wxCoord)(w*(max_x - max_tx + iter->pos_x*(max_tx-min_tx))/(max_x - min_x));
      }
      ypos = y + h/2;
      if (axis_dir == X_LEFT_Y_DOWN || axis_dir == X_RIGHT_Y_DOWN)
      {
        ylegend = y + (wxCoord)(h*(iter->pos_y*(max_ty-min_ty)+min_ty-min_y)/(max_y - min_y));
      }
      else
      {
        ylegend = y + (wxCoord)(h*(max_y - max_ty + iter->pos_y*(max_ty-min_ty))/(max_y - min_y));
      }
      x_horizontal_dir = true;
    }
    else
    {
      xpos = x + w/2;
      if (axis_dir == X_DOWN_Y_RIGHT || axis_dir == X_UP_Y_RIGHT)
      {
        xlegend = x + (wxCoord)(w*(iter->pos_x*(max_ty-min_ty)+min_ty-min_y)/(max_y - min_y));
      }
      else
      {
        xlegend = x + (wxCoord)(w*(max_y - max_ty + iter->pos_x*(max_ty-min_ty))/(max_y - min_y));
      }
      if (axis_dir == X_DOWN_Y_LEFT || axis_dir == X_DOWN_Y_RIGHT)
      {
        ypos = y + (wxCoord)(h*(iter->value_x*(max_tx-min_tx)+min_tx-min_x)/(max_x - min_x));
        ylegend = y + (wxCoord)(h*(iter->pos_y*(max_tx-min_tx)+min_tx-min_x)/(max_x - min_x));
      }
      else
      {
        ypos = y + (wxCoord)(h*(max_x - max_tx + iter->value_x*(max_tx-min_tx))/(max_x - min_x));
        ylegend = y + (wxCoord)(h*(max_x - max_tx + iter->pos_y*(max_tx-min_tx))/(max_x - min_x));
      }
      x_horizontal_dir = false;
    }
    iter->rect.SetX(xlegend);
    iter->rect.SetY(ylegend);
    iter->rect.SetWidth(2);
    iter->rect.SetHeight(2);

    // Calculate a suitable scaling factor
    double scaleX=(double)(ppi.GetWidth()/ppi_x);
    double scaleY=(double)(ppi.GetHeight()/ppi_y);
    // Use x or y scaling factor, whichever fits on the DC
    double actualScale = wxMin(scaleX,scaleY);
    // Set the scale
    wxFont tmp_font = iter->fnt;
    tmp_font.SetPointSize(int(tmp_font.GetPointSize()*actualScale+0.5));
  
    dc.SetFont(tmp_font);
    dc.SetTextForeground(iter->pen.GetColour());
    
    wxPen tmp_pen = iter->pen;
    tmp_pen.SetWidth(int(iter->pen.GetWidth()*actualScale+0.5));
    dc.SetPen(tmp_pen);

    int first = 0;
    wxCoord xpos_bak;
    wxCoord ypos_bak;    
    wxCoord delta=0;
    int sw,sh;
    for (vector<Curve *>::iterator i = curve_list.begin(); 
            i != curve_list.end(); ++i)
    {
      if ((*i)->GetLegendPos() != no )
	continue;
      GetIndicatePos(x, y, w, h, xpos, ypos, xvalue, yvalue, *i);
      if (first == 0)
      {
        ++first;
        xpos_bak = xpos;
        ypos_bak = ypos;
      }
      else
      {
        if (first == 1)
        {
            ++first;
            if (ypos < ypos_bak)
            {
              delta = -sh-2; 
            }
            else
            {
              delta = sh+2;             
            }
            if (!x_horizontal_dir)
            {
              ypos += delta;          
              GetIndicatePos(x, y, w, h, xpos, ypos, xvalue, yvalue, *i);
            }
        }
        ylegend += delta;
      }
      wxString text = (*i)->GetWholeText();
      dc.GetTextExtent(text, &sw, &sh);

    if (handler->IsDrawIndicator() && *i == cur_curve && !strange_mode)
    {
      dc.SetPen(wxPen(*wxRED, tmp_pen.GetWidth()));
      dc.SetTextForeground(*wxRED);
    }
      if (iter->align_left)
      {
        DrawTexText(dc, text, xlegend, ylegend, sw, sh);
        if (iter->indicate_left)
        {
          dc.DrawLine(xpos, ypos, xlegend-2, ylegend+sh/2);
        }
        else
        {
          dc.DrawLine(xpos, ypos, xlegend+sw+2, ylegend+sh/2);
        }
        iter->rect.Union(wxRect(xlegend-2, ylegend-2, sw+4, sh+4));
      }
      else
      {
        DrawTexText(dc, text, xlegend, ylegend, sw, sh, false);
        DrawTexText(dc, text, xlegend-sw, ylegend, sw, sh);
        if (iter->indicate_left)
        {
          dc.DrawLine(xpos, ypos, xlegend-sw-2, ylegend+sh/2);
        }
        else
        {
          dc.DrawLine(xpos, ypos, xlegend+2, ylegend+sh/2);
        }
        iter->rect.Union(wxRect(xlegend-sw-2, ylegend-2, sw+4, sh+4));
      }
    if (handler->IsDrawIndicator() && *i == cur_curve && !strange_mode)
    {
      dc.SetPen(wxPen(*wxBLACK, tmp_pen.GetWidth()));
      dc.SetTextForeground(iter->pen.GetColour());
    }
        if (!x_horizontal_dir)
        {
          ypos += wxCoord(delta*0.333+0.5);          
        }
    }
}


double PlotWindow::getMaxY()
{
  if (curve_list.empty())
    return 1;
  
  vector<Curve *>::iterator i = curve_list.begin();
  double rslt = (*i)->Ymax();
  double tmp;
  ++i;
  while (i != curve_list.end())
  {
    tmp = (*i)->Ymax();
    if (tmp > rslt)
      rslt = tmp;
    ++i;
  }
  return rslt;
}

double PlotWindow::getMinY()
{
  if (curve_list.empty())
    return 0;
  
  vector<Curve *>::iterator i = curve_list.begin();
  double rslt = (*i)->Ymin();
  double tmp;
  ++i;
  while (i != curve_list.end())
  {
    tmp = (*i)->Ymin();
    if (tmp < rslt)
      rslt = tmp;
    ++i;
  }
  return rslt;
}

double PlotWindow::getMaxX()
{
  if (curve_list.empty())
    return 1;
  
  vector<Curve *>::iterator i = curve_list.begin();
  double rslt = (*i)->Xmax();
  double tmp;
  ++i;
  while (i != curve_list.end())
  {
    tmp = (*i)->Xmax();
    if (tmp > rslt)
      rslt = tmp;
    ++i;
  }
  return rslt;
}

double PlotWindow::getMinX()
{
  if (curve_list.empty())
    return 0;
  
  vector<Curve *>::iterator i = curve_list.begin();
  double rslt = (*i)->Xmin();
  double tmp;
  ++i;
  while (i != curve_list.end())
  {
    tmp = (*i)->Xmin();
    if (tmp < rslt)
      rslt = tmp;
    ++i;
  }
  return rslt;
}


void  PlotWindow::GetAxisLabelExtent(wxDC &dc, wxCoord &lw, wxCoord &lh)
{
  wxCoord w1, h1;
  dc.GetTextExtent(str_x_label, &w1, &h1);
  wxCoord w2, h2;
  dc.GetTextExtent(str_y_label, &w2, &h2);
  wxCoord w3=0, h3=0;
  for (vector<PlotWindow *>::iterator i = children.begin(); i != children.end(); ++i)
  {
    wxCoord w4=0, h4=0;
    (*i)->GetAxisLabelExtent(dc, w4, h4);
    if (w4 > w3)
	    w3 = w4;
    if (h4 > h3)
	    h3 = h4;
  }
  lw = w1 > w2 ? (w1 > w3 ? w1 : w3) : (w2 > w3 ? w2 : w3);
  lh = h1 > h2 ? (h1 > h3 ? h1 : h3) : (h2 > h3 ? h2 : h3);
}


BEGIN_EVENT_TABLE(PlotHandler, wxEvtHandler)
EVT_MENU(wxID_OPEN, PlotHandler::OnOpen)
EVT_MENU(wxID_SAVE, PlotHandler::OnSave)
EVT_MENU(wxID_SAVEAS, PlotHandler::OnSaveAs)
EVT_MENU(wxID_PRINT, PlotHandler::OnPrint)
EVT_MENU(wxID_PREVIEW, PlotHandler::OnPrintPreview)
EVT_MENU(wxID_PRINT_SETUP, PlotHandler::OnPageSetup)
EVT_MENU(XRCID("SET_DRAW_INDICATE"), PlotHandler::OnSetDrawIndicator)
EVT_MENU(XRCID("SET_WYSWYG"), PlotHandler::OnSetWYSWYG)
EVT_MENU(wxID_EXIT,  PlotHandler::OnExitToolOrMenuCommand)
EVT_MENU(wxID_COPY,  PlotHandler::OnCopy)
EVT_MENU(XRCID("about_tool_or_menuitem"), PlotHandler::OnAboutToolOrMenuCommand)
END_EVENT_TABLE()

void PlotHandler::OnAboutToolOrMenuCommand(wxCommandEvent& WXUNUSED(event))
{
    wxDialog dlg;
    if (wxXmlResource::Get()->LoadDialog(&dlg, owner, wxT("about_dialog")))
    {
    XRCCTRL(dlg, "package_string", wxStaticText)->SetLabel(wxT(PACKAGE_STRING));
    XRCCTRL(dlg, "package_bugreport", wxStaticText)->SetLabel(wxT(PACKAGE_BUGREPORT));
    dlg.ShowModal();
    }
    else
    {
      wxMessageBox(wxString::Format(wxT("package info : %s \nbug report to %s"), PACKAGE_STRING, PACKAGE_BUGREPORT), _("About"), wxOK|wxICON_INFORMATION, owner);
    }
}

// Define my frame constructor
PlotHandler::PlotHandler(wxWindow * parent) :
  isDrawIndicator(true),
  bitmap(2048,2048), 
  owner(parent),
  WYSWYG(false),
  FileName(wxEmptyString),
  hasLoaded(false)
{
  pos_title = TITLE_UP;
  fnt_title = wxFont(15, wxSWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
  str_title = _("Test Curve");
  pen_title = *wxRED_PEN;
  
  PosMgr::PosMap pm;
  pm.cached = false;
  vector<wxUint32> a;
  a.push_back(1);
  pm.row_list = a;
  pm.col_list = a;
  pos_mgr.GetDiv().push_back(pm);
  
  PlotWindow * curWin = new PlotWindow(this);
  
  double b1[]={0.1, 0.22, 0.31, 0.4, 0.5, 0.64, 0.72, 0.8};
  //double b1[]={0.8, 0.72, 0.61, 0.5, 0.4, 0.34, 0.22, 0.18};
  double c1[]={1, 3, 3, 2, 4, 5, 3, 1};
  //double c1[]={0.8, 0.72, 0.61, 0.5, 0.4, 0.34, 0.22, 0.18};
  int j = 8;
  Curve *cur_curve;
  cur_curve = new Curve(b1, j, c1);
  cur_curve->SetText(wxT("curve 1"));
  cur_curve->SetType(Curve::CURVE_BSPLINE);
  cur_curve->sample(30);
  cur_curve->SetSymbol(Curve::CURVE_NONE);
  cur_curve->SetSymbolFilled();
  cur_curve->SetTextPos(Curve::CURVE_LEFT_UP);
  curWin->push_back(cur_curve);
  curWin->ShowLegend(false);

  curWin->SetRowPos(0);
  curWin->SetColPos(0);
  windows_list.push_back(curWin);

}

PlotHandler::~PlotHandler()
{
  while (!windows_list.empty())
  {
    delete windows_list.back();
    windows_list.pop_back();
  }
}
    
void PlotHandler::OnOpen(wxCommandEvent& event)
{
  if (hasLoaded)
  {
    if (wxMessageBox(_("Do you want to save it before openning a new file"), _("Message"), wxYES_NO, owner) == wxYES)
    {
      Save();
    }
  }
  wxString  filename = wxFileSelector(_("Choose a file to open"), wxT(""), wxT(""), wxT("xml"), wxT("XML files(*.xml)|*.xml"), wxOPEN|wxFILE_MUST_EXIST, owner);
  if ( !filename.empty() )
  {
      Load(filename);
  }
}

bool PlotHandler::Load(const wxString & filename)
{
    FGXMLParse controls_file_parser;
    try
    {
      readXML (wx2stdstr(filename).c_str(), controls_file_parser);
    }
    catch (...)
    {
      wxLogError(_("Fail to parse file ")+filename);
      return false;
    }

    Element * document = controls_file_parser.GetDocument();
    if (document->GetName() != "plot_config")
    {
      wxLogError(_("Unkown XML File Format in ")+filename);
      return false;
    }
    if (document->GetAttributeValueAsNumber("version") < 1)
    {
      wxLogError(_("Too Low XML File Format Version in ")+filename);
      return false;
    }

    FileName = filename;
    if (Load(document))
    {
      hasLoaded = true;
      owner->Refresh();
      //owner->Update();
    }
    else
    {
      wxLogError(_("Fail To Load ")+filename);
      return false;    
    }
    return true;
}

void PlotHandler::OnSave(wxCommandEvent& event)
{
  event.Skip();
  Save();
}

void PlotHandler::OnSaveAs(wxCommandEvent& event)
{
  event.Skip();
  wxString  filename = wxFileSelector(_("Choose a file to save"), wxT(""), FileName.AfterLast (wxFileName::GetPathSeparator()).BeforeLast (wxT ('.')), wxT("xml"), wxT("XML files (*.xml)|*.xml|BMP files (*.bmp)|*.bmp|JPEG files (*.jpg)|*.jpg|PNG files (*.png)|*.png"), wxSAVE|wxOVERWRITE_PROMPT, owner);
  if ( !filename.empty() )
  {
      wxString suf = filename.AfterLast(wxT('.'));
      if (suf.Lower() == wxT("xml"))
      {
        if (!Save(filename))
        {
          wxLogError(wxString(_("Fail to save file as "))+filename);
	}
      }
      else
      {
    wxCoord w = bitmap.GetWidth();
    wxCoord h = bitmap.GetHeight();
    wxMemoryDC mdc;
    {
      int multi =  wxGetNumberFromUser( _("Set the PPI multiplier"), wxString::Format(wxT("PPI = %g X "), ppi_x), _("PPI Setting"), (long)1, (long)1, (long)10, owner);
      w *= multi;
      h *= multi;
      ppi.SetWidth( int(ppi_x * multi+0.5) );
      ppi.SetHeight( int(ppi_y * multi+0.5) );
      wxBitmap tbitmap(w, h);
      mdc.SelectObject(tbitmap);
      draw(mdc, w, h);
      mdc.SelectObject(wxNullBitmap); 
      ppi.Set( int(ppi_x+0.5), int(ppi_y+0.5));

      if (suf == wxT("bmp"))
        tbitmap.SaveFile(filename, wxBITMAP_TYPE_BMP);
      else if (suf == wxT("jpg"))
        tbitmap.SaveFile(filename, wxBITMAP_TYPE_JPEG);
      else if (suf == wxT("png"))
        tbitmap.SaveFile(filename, wxBITMAP_TYPE_PNG);
      else
        wxMessageBox(_("unkown file type"), _("Message"), wxOK|wxICON_ERROR , owner);
    }
      }
  }
}

void PlotHandler::OnCopy(wxCommandEvent& event)
{
  if (wxTheClipboard->Open())
  {
    // This data objects are held by the clipboard, 
    // so do not delete them in the app.
    wxCoord w = bitmap.GetWidth();
    wxCoord h = bitmap.GetHeight();
    wxMemoryDC mdc;
    {
      int multi =  wxGetNumberFromUser( _("Set the PPI multiplier"), wxString::Format(wxT("PPI = %g X "), ppi_x), _("PPI Setting"), (long)1, (long)1, (long)10, owner);
      w *= multi;
      h *= multi;
      ppi.SetWidth( int(ppi_x * multi+0.5) );
      ppi.SetHeight( int(ppi_y * multi+0.5) );
      wxBitmap tbitmap(w, h);
      if (tbitmap.Ok())
      {
        mdc.SelectObject(tbitmap);
        draw(mdc, w, h);
        mdc.SelectObject(wxNullBitmap); 
        wxTheClipboard->SetData( new wxBitmapDataObject(tbitmap) );
      }
      else
      {
        wxMessageBox(_("Fail to create bitmap."), _("Message"), wxOK|wxICON_ERROR , owner);
      }
      ppi.Set( int(ppi_x+0.5), int(ppi_y+0.5) );
    }
    wxTheClipboard->Close();
  }
}


void PlotHandler::OnPrint(wxCommandEvent& event)
{
    wxPrintDialogData printDialogData(printData);

    wxPrinter printer(& printDialogData);
    PlotPrintout printout(this, wxT("My printout"));
    if (!printer.Print(owner, &printout, true /*prompt*/))
    {
        if (wxPrinter::GetLastError() == wxPRINTER_ERROR)
            wxMessageBox(_("There was a problem printing.\nPerhaps your current printer is not set correctly?"), _("Printing"), wxOK);
        else
            wxMessageBox(_("You canceled printing"), _("Printing"), wxOK);
    }
    else
    {
        printData = printer.GetPrintDialogData().GetPrintData();
    }

    /*
    if (IsWYSWYG())
    {
      SetWYSWYG();
    }
    */
}

void PlotHandler::OnPrintPreview(wxCommandEvent& event)
{
    // Pass two printout objects: for preview, and possible printing.
    wxPrintDialogData printDialogData(printData);
    wxPrintPreview *preview = new wxPrintPreview(new PlotPrintout(this), new PlotPrintout(this), & printDialogData);
    if (!preview->Ok())
    {
        delete preview;
        wxMessageBox(_("There was a problem previewing.\nPerhaps your current printer is not set correctly?"), _("Previewing"), wxOK);
        return;
    }

    wxPreviewFrame *frame = new wxPreviewFrame(preview, owner, _("Demo Print Preview"), wxPoint(100, 100), wxSize(600, 650));
    frame->Centre(wxBOTH);
    frame->Initialize();
    frame->Show();

    /*
    if (IsWYSWYG())
    {
      SetWYSWYG();
    }
    */
}

void PlotHandler::OnPageSetup(wxCommandEvent& event)
{
    pageSetupData = printData;

    wxPageSetupDialog pageSetupDialog(owner, &pageSetupData);
    pageSetupDialog.ShowModal();

    printData = pageSetupDialog.GetPageSetupData().GetPrintData();
    pageSetupData = pageSetupDialog.GetPageSetupData();

    /*
    if (IsWYSWYG())
    {
      SetWYSWYG();
    }
    */
}

void PlotHandler::OnSetDrawIndicator(wxCommandEvent& event)
{
    DrawIndicator(event.IsChecked());
}

void PlotHandler::OnSetWYSWYG(wxCommandEvent& event)
{
    SetWYSWYG(event.IsChecked());
}

void PlotHandler::OnExitToolOrMenuCommand(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close.
    owner->Close(true);
}

void PlotHandler::draw(wxDC& dc, wxCoord w, wxCoord h)
{
#if wxCHECK_VERSION(2, 7, 0)
#else
    dc.BeginDrawing();
#endif
    
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();
    dc.SetBackgroundMode(wxTRANSPARENT);

    wxCoord x=0, y=0;

    // Calculate a suitable scaling factor
    double scaleX=(double)(ppi.GetWidth()/ppi_x);
    double scaleY=(double)(ppi.GetHeight()/ppi_y);
    // Use x or y scaling factor, whichever fits on the DC
    double actualScale = wxMin(scaleX,scaleY);
    // Set the scale
    wxFont tmp_font = fnt_title;
    tmp_font.SetPointSize(int(tmp_font.GetPointSize()*actualScale+0.5));
  
    dc.SetFont(tmp_font);
    dc.SetTextForeground(pen_title.GetColour());
    wxCoord tw, th;
    dc.GetTextExtent(str_title, &tw, &th);
    th+=5;
    switch (pos_title)
    {
     case TITLE_UP :
       {
         x = 0;
         y = th;
         h = h-th;
         DrawTexText(dc, str_title, (w-tw)/2, 5, tw, th, false);
         DrawTexText(dc, str_title, (w-tw)/2, 5, tw, th);
         rect_title = wxRect((w-tw)/2-2, 3, tw+4, th+4);
       }
       break;
     case TITLE_DOWN :
       {
         x = 0;
         y = 0;
         h = h-th;
         DrawTexText(dc, str_title, (w-tw)/2, h+5, tw, th, false);
         DrawTexText(dc, str_title, (w-tw)/2, h+5, tw, th);
         rect_title = wxRect((w-tw)/2-2, h+3, tw+4, th+4);
       }
       break;
     default :
       rect_title = wxRect(0, 0, -1, -1);
       break;
    }
    
    wxCoord lw, lh;
    wxCoord lw1, lh1;
    wxCoord lw2, lh2;
    if (windows_list.size() > 0u)
    {
      // Calculate a suitable scaling factor
      double scaleX=(double)(ppi.GetWidth()/ppi_x);
      double scaleY=(double)(ppi.GetHeight()/ppi_y);
      // Use x or y scaling factor, whichever fits on the DC
      double actualScale = wxMin(scaleX,scaleY);
      // Set the scale
      wxFont tmp_font = windows_list[0]->GetAxisFont();
      tmp_font.SetPointSize(int(tmp_font.GetPointSize()*actualScale+0.5));
      dc.SetFont(tmp_font);

      for (vector<PlotWindow *>::iterator i = windows_list.begin(); i != windows_list.end(); ++i)
      {
        (*i)->GetAxisLabelExtent(dc, lw1, lh1);
      }
    }
    lw1 += wxCoord(actualScale * 10);
    dc.GetTextExtent(wxT("123456789012"), &lw2, &lh2);
    lw = lw1 > lw2 ? lw1 : lw2;
    lh = lh1 > lh2 ? lh1 : lh2;
    lh += lh;

    pos_mgr.SetX(x+lw);
    pos_mgr.SetY(y+lh);
    pos_mgr.SetW(w-lw*2);
    pos_mgr.SetH(h-lh*2);
    pos_mgr.DrawGrid(dc);
    for (vector<PlotWindow *>::iterator i = windows_list.begin(); i != windows_list.end(); ++i)
    {
        (*i)->Draw(dc, pos_mgr);
    }

#if wxCHECK_VERSION(2, 7, 0)
#else
    dc.EndDrawing();
#endif
}


void PlotHandler::SetWYSWYG(bool b)
{
    WYSWYG = b;
    if (b)
    {
      int w, h;
      get_paper_size(w, h);
      w = (int)wxGetNumberFromUser( _("Virtual Width Setting"), _("Input"), _("Virtual Size Setting"), (long)w, long(600), long(2048), owner);
      h = (int)wxGetNumberFromUser( _("Virtual Height Setting"), _("Input"), _("Virtual Size Setting"), (long)h, long(600), long(2048), owner);
      canvas->SetVirtualSize(w, h);
      canvas->SetScrollRate(5, 5);
    }
    else
    {
      canvas->SetVirtualSize(-1, -1);
      canvas->SetScrollRate(5, 5);        
    }
    owner->Refresh();
    //owner->Update();
}


void PlotHandler::Draw(wxDC& dc, wxCoord w, wxCoord h)
{
    bitmap.SetHeight(h);
    bitmap.SetWidth(w);
    {
      if (!bitmap.Ok())
      {
        bitmap.Create(3000,3000);
      }
      if (bitmap.Ok())
      {
        wxMemoryDC mdc;
        mdc.SelectObject(bitmap);
        draw(mdc, w, h);
	mdc.SelectObject(wxNullBitmap); 
        dc.DrawBitmap(bitmap, 0, 0, false);
      }
      else
      {
        draw(dc, w, h);
      }
    }
}

void PlotHandler::DrawIndicator(wxDC &dc, wxCoord x, wxCoord y)
{
 if (isDrawIndicator)
 {
  if (bitmap.Ok())
  {
    dc.DrawBitmap(bitmap, 0, 0, false);
  }
  else
  {
    wxCoord w,h;
    canvas->GetVirtualSize(&w, &h);
    Draw(dc, w, h);
  }
  for (vector<PlotWindow *>::iterator i = windows_list.begin(); i != windows_list.end(); ++i)
  {
    for (vector<IndecatorPos>::iterator j = indicator_list.begin(); j != indicator_list.end(); ++j)
    {
      (*i)->DrawIndicator(dc, pos_mgr, j->x, j->y);
    }
    
    (*i)->DrawIndicator(dc, pos_mgr, x, y);
  }
 }
}


void PlotHandler::DrawFocus(wxDC &dc, wxCoord x, wxCoord y, int mask)
{
  if (bitmap.Ok())
  {
    dc.DrawBitmap(bitmap, 0, 0, false);
  }
  else
  {
    wxCoord w,h;
    canvas->GetVirtualSize(&w, &h);
    Draw(dc, w, h);
  }
  ClearFocus();
  dc.SetPen(wxPen(*wxRED, 3));
  if (mask & Focus::TITLE)
  {
    wxRect r(x-2, y-2, 4, 4);
    if (r.Intersects(rect_title))
    {
    wxPoint p[5];
    p[0].x = rect_title.GetX();
    p[0].y = rect_title.GetY();
    p[1].x = p[0].x+rect_title.GetWidth();
    p[1].y = p[0].y;
    p[2].x = p[1].x;
    p[2].y = p[1].y+rect_title.GetHeight();
    p[3].x = p[0].x;
    p[3].y = p[2].y;
    p[4].x = p[0].x;
    p[4].y = p[0].y;
    dc.DrawLines(5, p);
    Focus tmp;
    tmp.window = NULL;
    tmp.type = Focus::TITLE;
    tmp.iter = 0;
    AddFocus(tmp);
    }
  }
  if (mask & Focus::GRID)
  {
    wxRect r(x-2, y-2, 4, 4);
    wxRect rect(pos_mgr.GetX(), pos_mgr.GetY(), pos_mgr.GetW(), pos_mgr.GetH() );
    if (r.Intersects(rect))
    {
    Focus tmp;
    tmp.window = NULL;
    tmp.type = Focus::GRID;
    tmp.iter = 0;
    AddFocus(tmp);
    }
  }
  for (vector<PlotWindow *>::iterator i = windows_list.begin(); i != windows_list.end(); ++i)
  {
    (*i)->DrawFocus(dc, pos_mgr, x, y, mask);
  }
}

bool PlotHandler::SetLegendPos(wxCoord x, wxCoord y, const int & no)
{
  bool flag = false;
  for (vector<PlotWindow *>::iterator i = windows_list.begin(); !flag && i != windows_list.end(); ++i)
  {
    if( (*i)->SetLegendPos(pos_mgr, x, y, no))
    {
      flag = true;
    }
  }
  return flag;
}

bool PlotHandler::SetLegendVal(wxCoord x, wxCoord y, const int &no)
{
  bool flag = false;
  for (vector<PlotWindow *>::iterator i = windows_list.begin(); !flag && i != windows_list.end(); ++i)
  {
    if ( (*i)->SetLegendVal(pos_mgr, x, y, no))
    {
      flag = true;
    }
  }
  return flag;
}

bool PlotHandler::SelectCurrentCurve(wxCoord x, wxCoord y)
{
  bool rslt = false;
  for (vector<PlotWindow *>::iterator i = windows_list.begin();
          i != windows_list.end(); ++i)
  {
    rslt = rslt || (*i)->SelectCurrentCurve(pos_mgr, x, y);
  }
  return rslt;
}

bool PlotHandler::Load(Element * element)
{
    Element * tmp;
    if ((tmp = element->FindElement("size")))
    {
      wxString width = std2wxstr(tmp->FindElementValue("width"));
      wxString height = std2wxstr(tmp->FindElementValue("height"));
      unsigned long w, h;
      if (owner->IsKindOf(CLASSINFO(wxFrame)) || owner->IsKindOf(CLASSINFO(wxDialog)))
      {
        if (width.ToULong(&w) && height.ToULong(&h))
        owner->SetClientSize(wxSize(w, h));
      }
    }
    if ((tmp = element->FindElement("title")))
    {
      str_title = std2wxstr(tmp->FindElementValue("name"));
      if (str_title.IsEmpty())
      {
        pos_title = TITLE_NONE;
      }
      else
      {
        wxString pos = std2wxstr(tmp->FindElementValue("pos"));
        pos.LowerCase();
        if (pos == wxT("up"))
        {
          pos_title = TITLE_UP;
        }
        else if (pos == wxT("down"))
        {
          pos_title = TITLE_DOWN;
        }
        else
        {
          pos_title = TITLE_NONE;        
        }
      }
      LoadPainter(tmp->FindElement("painter"), &pen_title, &fnt_title);
    }
    
    if ((tmp = element->FindElement("pos_mgr")))
    {
      if (!pos_mgr.Load(tmp))
      {
        wxLogError(_("Fail to init pos_mgr."));
        return false;
      }
    }
    else
    {
      wxLogError(_("Fail to find pos_mgr tag."));
      return false;
    }

    table_map.clear();
    table_list_list.clear();
    tmp = element->FindElement("table");
    while (tmp)
    {
      TableList tl = Table::Load(tmp);
      if (tl.size() >= 1u)
      {
        table_list_list.push_back(tl);
      }
      for (TableListIter i = tl.begin(); i != tl.end(); ++i)
      {
	if (table_map.find(std2wxstr(i->GetName())) == table_map.end())
	{
          table_map[std2wxstr(i->GetName())] = *i;
          table_map[std2wxstr(i->GetNameLabel())] = *i;
          table_map[std2wxstr(i->GetName())+wxT("(0)")] = *i;
          table_map[std2wxstr(i->GetNameLabel())+wxT("(0)")] = *i;
	}
	else
	{
	  for (int j = 1; j < 100; ++j)
	  {
	    wxString index = wxString::Format(wxT("(%d)"), j);
	    if (table_map.find(std2wxstr(i->GetName())+index) == table_map.end())
	     {
               table_map[std2wxstr(i->GetName())+index] = *i;
               table_map[std2wxstr(i->GetNameLabel())+index] = *i;
	       break;
	     }
	  }
	}
      }
      tmp = element->FindNextElement("table");
    }

    table_list_clist.clear();
    tmp = element->FindElement("ctable");
    while (tmp)
    {
      TableList tl = Table::Load(tmp);
      if (tl.size() >= 1u)
      {
        table_list_clist.push_back(tl);
      }
      for (TableListIter i = tl.begin(); i != tl.end(); ++i)
      {
	if (table_map.find(std2wxstr(i->GetName())) == table_map.end())
	{
          table_map[std2wxstr(i->GetName())] = *i;
          table_map[std2wxstr(i->GetNameLabel())] = *i;
          table_map[std2wxstr(i->GetName())+wxT("(0)")] = *i;
          table_map[std2wxstr(i->GetNameLabel())+wxT("(0)")] = *i;
	}
	else
	{
	  for (int j = 1; j < 100; ++j)
	  {
	    wxString index = wxString::Format(wxT("(%d)"), j);
	    if (table_map.find(std2wxstr(i->GetName())+index) == table_map.end())
	     {
               table_map[std2wxstr(i->GetName())+index] = *i;
               table_map[std2wxstr(i->GetNameLabel())+index] = *i;
	       break;
	     }
	  }
	}
      }
      tmp = element->FindNextElement("ctable");
    }

    while (!windows_list.empty())
    {
      delete windows_list.back();
      windows_list.pop_back();
    }
    tmp = element->FindElement("window");
    while (tmp)
    {
        try
        {
          windows_list.push_back(new PlotWindow(this, tmp));
        }
        catch (...)
        {
            wxLogError(_("Fail to insert sub window."));
            return false;
        }
        tmp = element->FindNextElement("window");
    }
    return true;
}

bool PlotHandler::Save(const wxString & filename)
{
  if (!filename.IsEmpty())
  {
    FileName = filename;
  }
  wxFileOutputStream stream(FileName);
  if (!stream.Ok())
    return false;

  wxTextOutputStream tstream(stream,wxEOL_UNIX);

  tstream << wxT("<?xml version=\"1.0\"?>\n");
  tstream << wxT("<!--  Generated by PlotHandler for Nomograph -->\n");

  Export(tstream, wxEmptyString);

  return true;
}

void PlotHandler::Export(wxTextOutputStream & tstream, const wxString & prefix) const
{
  tstream << prefix << wxT("<plot_config") << wxT(" version=\"1\">\n");

 {
  wxString pre = prefix + wxT("    ");

  tstream << pre << wxT("<size>\n"); 
  {
    int w, h;
    owner->GetClientSize(&w, &h);
    tstream << pre << wxT("  <width> ") << w << wxT(" </width>\n"); 
    tstream << pre << wxT("  <height> ") << h << wxT(" </height>\n"); 
  }
  tstream << pre << wxT("</size>\n\n"); 

  tstream << pre << wxT("<title>\n"); 
  {
    wxString subpre = pre + wxT("    ");
    tstream << subpre << wxT("<name> ") << str_title << wxT(" </name>\n"); 
    wxString pos;
    switch (pos_title)
    {
      case TITLE_UP :
        pos = wxT("up");
        break;
      case TITLE_DOWN :
        pos = wxT("down");
        break;
      default :
        pos = wxT("none");
    }
    tstream << subpre << wxT("<pos> ") << pos << wxT(" </pos>\n"); 
    ExportPainter(tstream, subpre, pen_title, fnt_title);
  }
  tstream << pre << wxT("</title>\n\n"); 

  pos_mgr.Export(tstream, pre);
  tstream << '\n';

  tstream << pre << wxT("<!-- data in start -->\n");
  for (vector<TableList>::const_iterator i= table_list_list.begin(); i != table_list_list.end(); ++i)
  {
    ExportTable(tstream, pre, *i, wxT("table"));
    tstream << wxT("\n");
  }
  tstream << pre << wxT("<!-- data in end -->\n");
  for (vector<TableList>::const_iterator i= table_list_clist.begin(); i != table_list_clist.end(); ++i)
  {
    ExportTable(tstream, pre, *i, wxT("ctable"));
    tstream << wxT("\n");
  }

  for (vector<PlotWindow *>::const_iterator i= windows_list.begin(); i != windows_list.end(); ++i)
  {
    (*i)->Export(tstream, pre);
    tstream << wxT('\n');
  }

 }

  tstream << prefix  << wxT("</plot_config>") << endl;
}

void PlotHandler::get_paper_size(int &w, int &h)
{
    wxSize s = pageSetupData.GetPaperSize();
    double t = ppi_x/25.4;
    /*
    if (printData.GetOrientation() == wxLANDSCAPE)
    {
      h = int(s.GetWidth()*t);
      w = int(s.GetHeight()*t);
    }
    else
    */
    {
    
      w = int(s.GetWidth()*t);
      h = int(s.GetHeight()*t);    
    }
}


BEGIN_EVENT_TABLE(PlotCanvas, wxScrolledWindow)
EVT_PAINT(PlotCanvas::OnPaint)
EVT_CHAR(PlotCanvas::OnKey)
EVT_MOTION(PlotCanvas::OnMotion)
EVT_LEFT_DOWN(PlotCanvas::OnMouseLeftDown) 
EVT_RIGHT_DOWN(PlotCanvas::OnMouseRightDown) 
EVT_MENU_RANGE(FONT_START, FONT_START+99, PlotCanvas::OnFont) 
EVT_MENU_RANGE(PEN_START, SUB_PEN_START+99, PlotCanvas::OnPen) 
END_EVENT_TABLE()

// Define a constructor for my canvas
PlotCanvas::PlotCanvas(wxWindow *parent, wxWindowID id, PlotHandler *hdl, const wxPoint& pos, const wxSize& size):
    wxScrolledWindow(parent, id, pos, size, wxRETAINED), handler(hdl), mode(0), mask(-1), no(0)
{
    handler->canvas = this;
    SetBackgroundColour(* wxWHITE);

    wxClientDC dc(this);
    DoPrepareDC(dc);
    ppi = dc.GetPPI();
    ppi_y = (double)ppi.GetHeight();
    ppi_x = (double)ppi.GetWidth();
}

// Define the repainting behaviour
void PlotCanvas::OnPaint(wxPaintEvent& event)
{
  wxPaintDC dc(this);
  DoPrepareDC(dc);
  wxCoord wc,hc;
  GetClientSize(&wc, &hc);
  dc.DrawRectangle(0, 0, wc, hc);
  wxCoord w,h;
  GetVirtualSize(&w, &h);
  handler->Draw(dc, w, h);
}

// Define the mouse motion behaviour
void PlotCanvas::OnMotion(wxMouseEvent& event)
{
  SetFocus();
  wxClientDC dc(this);
  DoPrepareDC(dc);
  int xx, yy;
  CalcUnscrolledPosition( event.GetX(), event.GetY(), &xx, &yy); 
  handler->DrawIndicator(dc, xx, yy);
}

// Define the mouse click behaviour
void PlotCanvas::OnMouseLeftDown(wxMouseEvent& event)
{
  event.Skip();
  int xx, yy;
  CalcUnscrolledPosition( event.GetX(), event.GetY(), &xx, &yy); 
  if (mode == 0)
  {
    if (event.ControlDown())
    {
      handler->AddIndicator(xx, yy);
    } 
    else if (handler->SelectCurrentCurve(xx, yy))
    {
      wxClientDC dc(this);
      DoPrepareDC(dc);
      wxCoord w,h;
      GetVirtualSize(&w, &h);
      handler->Draw(dc, w, h);
      handler->DrawIndicator(dc, xx, yy);
    }
  }
  else if (mode == 1)
  {
      SetCursor(wxNullCursor);
      mode = 0;
      if (handler->SetLegendPos(xx, yy, no))
      {
      wxClientDC dc(this);
      DoPrepareDC(dc);
      wxCoord w,h;
      GetVirtualSize(&w, &h);
      handler->Draw(dc, w, h);
      handler->DrawIndicator(dc, xx, yy);
      no=0;
      }
  }
  else if (mode == 2)
  {
      SetCursor(wxNullCursor);
      mode = 0;
      if (handler->SetLegendVal(xx, yy, no))
      {
      wxClientDC dc(this);
      DoPrepareDC(dc);
      wxCoord w,h;
      GetVirtualSize(&w, &h);
      handler->Draw(dc, w, h);
      handler->DrawIndicator(dc, xx, yy);
      no = 0;
      }
  }
}

void PlotCanvas::mkMenu(wxMenu & main)
{
  int n=0;
  for (FocusListCIter i = handler->focus_list.begin(); i != handler->focus_list.end(); ++i, ++n)
  {
    wxString item;// = wxString::Format(wxT("%u "), size_t(i->window) % 0xffu);
    wxMenu * sub = new wxMenu;
    switch (i->type)
    {
      case Focus::WINDOW :
        item += wxT("window");
        break;
      case Focus::LEGEND :
        item += wxT("legend");
        sub->Append(FONT_START+n, _("Select Fo&nt..."));
        sub->Append(PEN_START+n, _("Select &Pen..."));
        break;
      case Focus::AXISX :
        item += wxT("AxisX");
        sub->Append(FONT_START+n, _("Select Fo&nt..."));
        sub->Append(PEN_START+n, _("Select &Pen..."));
        break;
      case Focus::AXISY :
        item += wxT("AxisY");
        sub->Append(FONT_START+n, _("Select Fo&nt..."));
        sub->Append(PEN_START+n, _("Select &Pen..."));
        break;
      case Focus::CURVE :
        item += wxT("curve");
        sub->Append(FONT_START+n, _("Select Fo&nt..."));
        sub->Append(PEN_START+n, _("Select &Pen..."));
        break;
      case Focus::TITLE :
        item += wxT("title");
        sub->Append(FONT_START+n, _("Select Fo&nt..."));
        sub->Append(PEN_START+n, _("Select &Colour..."));
        break;
      case Focus::GRID :
        item += wxT("GRID");
        sub->Append(PEN_START+n, _("Select &Pen for grid..."));
        sub->Append(SUB_PEN_START+n, _("Select &Pen for subgrid..."));
        break;
    }
    main.AppendSubMenu(sub, item);
  }
}

void PlotCanvas::OnMouseRightDown(wxMouseEvent& event)
{
    event.Skip();
    if (event.ControlDown())
    {
      int xx, yy;
      CalcUnscrolledPosition( event.GetX(), event.GetY(), &xx, &yy);
      {
        wxClientDC dc(this);
        DoPrepareDC(dc);
        handler->DrawFocus(dc, xx, yy, mask);
      }
      wxMenu menu;
      mkMenu(menu);
      PopupMenu(&menu);
      Refresh();
      //Update();
    }
    else
    {
      handler->ClearIndicators();
      wxClientDC dc(this);
      DoPrepareDC(dc);
      wxCoord w,h;
      GetVirtualSize(&w, &h);
      handler->Draw(dc, w, h);
      //handler->DrawIndicator(dc, xx, yy);
      SetCursor(wxNullCursor);
      mode = 0;
    }
}

void PlotCanvas::OnFont(wxCommandEvent& event)
{
  int n = event.GetId() - FONT_START;
  Focus f = handler->focus_list[n];
  switch (f.type)
  {
    case Focus::WINDOW :
      {
      
      }
      break;
    case Focus::LEGEND :
      {
        wxFont font = wxGetFontFromUser(this, f.window->GetLegendList()[f.iter].fnt, _("Choose the font for legend drawing."));
        if (font.IsOk())
        {
          f.window->GetLegendList()[f.iter].fnt = font;
	  Refresh();
        }
      }
      break;
    case Focus::AXISX :
    case Focus::AXISY :
      {
        wxFont font = wxGetFontFromUser(this, f.window->GetAxisFont(), _("Choose the font for axis drawing."));
        if (font.IsOk())
        {
          f.window->SetAxisFont(font);
	  Refresh();
        }
      }
      break;
    case Focus::CURVE :
      {
      
      }
      break;
    case Focus::TITLE :
      {
        wxFont font = wxGetFontFromUser(this, handler->GetTitleFont(), _("Choose the font for title drawing."));
        if (font.IsOk())
        {
          handler->SetTitleFont(font);
	  Refresh();
        }
      }
      break;
    case Focus::GRID :
      {
      
      }
      break;
  }
}

void PlotCanvas::OnPen(wxCommandEvent& event)
{
  int n = event.GetId() - PEN_START;
  bool sub_pen = false;
  if (event.GetId() - SUB_PEN_START >=0)
  {
    n = event.GetId() - SUB_PEN_START;
    sub_pen = true;
  }
  Focus f = handler->focus_list[n];
  switch (f.type)
  {
    case Focus::WINDOW :
      {
      
      }
      break;
    case Focus::LEGEND :
      {
        PenSettingDialog pendlg(this, f.window->GetLegendPen(f.iter));
	if (pendlg.ShowModal() == wxID_OK)
	{
	  f.window->SetLegendPen(pendlg.GetPen(), f.iter);
	  Refresh();
	}
      }
      break;
    case Focus::AXISX :
    case Focus::AXISY :
      {
        PenSettingDialog pendlg(this, f.window->GetAxisPen());
	if (pendlg.ShowModal() == wxID_OK)
	{
	  f.window->SetAxisPen(pendlg.GetPen());
	  Refresh();
	}
      }
      break;
    case Focus::CURVE :
      {
        PenSettingDialog pendlg(this, f.window->at(f.iter)->GetPen());
	if (pendlg.ShowModal() == wxID_OK)
	{
	  f.window->at(f.iter)->SetPen(pendlg.GetPen());
	  Refresh();
	}
      }
      break;
    case Focus::TITLE :
      {
          wxColour c = wxGetColourFromUser(this, handler->GetTitlePen().GetColour(), _("Colour Setting"));
          if (c.IsOk())
          {
	    wxPen p(c);
            handler->SetTitlePen(p);
            Refresh();
          }
      }
      break;
    case Focus::GRID :
      if (sub_pen)
      {
        PenSettingDialog pendlg(this, handler->GetPosMgr().GetSubGridPen());
	if (pendlg.ShowModal() == wxID_OK)
	{
	  handler->GetPosMgr().SetSubGridPen(pendlg.GetPen());
	  Refresh();
	}
      }
      else
      {
        PenSettingDialog pendlg(this, handler->GetPosMgr().GetGridPen());
	if (pendlg.ShowModal() == wxID_OK)
	{
	  handler->GetPosMgr().SetGridPen(pendlg.GetPen());
	  Refresh();
	}
      }
      break;
  }
}

void PlotCanvas::OnKey(wxKeyEvent& event)
{
    event.Skip();
    int key = event.GetKeyCode();
    if (event.ControlDown())
    switch (key)
    {
      case WXK_UP :
	{
    //handler->GetPosMgr().SetGridHeight(handler->GetPosMgr().GetGridHeight()+1);
    handler->ClearIndicators();
    wxClientDC dc(this);
    DoPrepareDC(dc);
    wxCoord w,h;
    GetVirtualSize(&w, &h);
    SetVirtualSize(w, h+10);
    SetScrollRate(5, 5);
    Refresh();
    //Update();
    SetCursor(wxNullCursor);
    mode = 0;	
	}
	break;
      case WXK_DOWN :
	{
    //unsigned int gh = handler->GetPosMgr().GetGridHeight()-1;
    //if (gh < 5)
    //  gh =5;
    //handler->GetPosMgr().SetGridHeight(gh);
    handler->ClearIndicators();
    wxClientDC dc(this);
    DoPrepareDC(dc);
    wxCoord w,h;
    GetVirtualSize(&w, &h);
    if (h > 200)
    {
    SetVirtualSize(w, h-10);
    SetScrollRate(5, 5);
    }
    Refresh();
    //Update();
    SetCursor(wxNullCursor);
    mode = 0;	
	}
	break;
      case WXK_RIGHT :
	{
    //handler->GetPosMgr().SetGridWidth(handler->GetPosMgr().GetGridWidth()+1);
    handler->ClearIndicators();
    wxClientDC dc(this);
    DoPrepareDC(dc);
    wxCoord w,h;
    GetVirtualSize(&w, &h);
    SetVirtualSize(w+10, h);
    SetScrollRate(5, 5);
    Refresh();
    //Update();
    SetCursor(wxNullCursor);
    mode = 0;	
	}
	break;
      case WXK_LEFT :
	{
    //unsigned int gh = handler->GetPosMgr().GetGridWidth()-1;
    //if (gh < 5)
    //  gh =5;
    //handler->GetPosMgr().SetGridWidth(gh);
    handler->ClearIndicators();
    wxClientDC dc(this);
    DoPrepareDC(dc);
    wxCoord w,h;
    GetVirtualSize(&w, &h);
    if (w > 300)
    {
    SetVirtualSize(w-10, h);
    SetScrollRate(5, 5);
    }
    Refresh();
    //Update();
    SetCursor(wxNullCursor);
    mode = 0;	
	}
	break;
      default :
     break;	
    }
    else
    switch (key)
    {
      case WXK_ESCAPE :
        SetCursor(wxNullCursor);
	mode = 0;
	break;
      case 'l' :
      case 'L' :
        SetCursor(*wxCROSS_CURSOR);
	mode = 1;
	break;
      case 'v' :
      case 'V' :
        SetCursor(*wxCROSS_CURSOR);
	mode = 2;
	break;
      case WXK_UP :
	{
    handler->GetPosMgr().SetGridY(handler->GetPosMgr().GetGridY()+1u);
    handler->ClearIndicators();
    wxClientDC dc(this);
    DoPrepareDC(dc);
    wxCoord w,h;
    GetVirtualSize(&w, &h);
    handler->Draw(dc, w, h);
    SetCursor(wxNullCursor);
    mode = 0;	
	}
	break;
      case WXK_DOWN :
	{
    unsigned int gh = handler->GetPosMgr().GetGridY()-1u;
    if (gh < 1u)
      gh =1u;
    handler->GetPosMgr().SetGridY(gh);
    handler->ClearIndicators();
    wxClientDC dc(this);
    DoPrepareDC(dc);
    wxCoord w,h;
    GetVirtualSize(&w, &h);
    handler->Draw(dc, w, h);
    SetCursor(wxNullCursor);
    mode = 0;	
	}
	break;
      case WXK_RIGHT :
	{
    handler->GetPosMgr().SetGridX(handler->GetPosMgr().GetGridX()+1u);
    handler->ClearIndicators();
    wxClientDC dc(this);
    DoPrepareDC(dc);
    wxCoord w,h;
    GetVirtualSize(&w, &h);
    handler->Draw(dc, w, h);
    SetCursor(wxNullCursor);
    mode = 0;	
	}
	break;
      case WXK_LEFT :
	{
    unsigned int gh = handler->GetPosMgr().GetGridX()-1u;
    if (gh < 1u)
      gh =1u;
    handler->GetPosMgr().SetGridX(gh);
    handler->ClearIndicators();
    wxClientDC dc(this);
    DoPrepareDC(dc);
    wxCoord w,h;
    GetVirtualSize(&w, &h);
    handler->Draw(dc, w, h);
    SetCursor(wxNullCursor);
    mode = 0;	
	}
	break;
      case '0' :
      case '1' :
      case '2' :
      case '3' :
      case '4' :
      case '5' :
      case '6' :
      case '7' :
      case '8' :
      case '9' :
	no = key - '0';
	break;
      default :
	break;
    }
}

bool PlotPrintout::OnPrintPage(int page)
{
    wxDC *dc = GetDC();
    if (dc)
    {
        if (page == 1)
	{
	    ppi = dc->GetPPI();
            DrawPageOne(dc);
	    ppi.Set(int(ppi_x+0.5), int(ppi_y+0.5));
	}

        return true;
    }
    else
        return false;
}

bool PlotPrintout::OnBeginDocument(int startPage, int endPage)
{
    if (!wxPrintout::OnBeginDocument(startPage, endPage))
        return false;

    return true;
}

void PlotPrintout::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
{
    *minPage = 1;
    *maxPage = 1;
    *selPageFrom = 1;
    *selPageTo = 1;
}

bool PlotPrintout::HasPage(int pageNum)
{
    return (pageNum == 1);
}

void PlotPrintout::DrawPageOne(wxDC *dc)
{
    // Get the size of the DC in pixels
    int w, h;
    dc->GetSize(&w, &h);

    /*
    // Get the resolution of the device in pixels per inch.
    wxSize s = dc->GetPPI();
    
    // Calculate a suitable scaling factor
    double scaleX=(float)(s.GetWidth()/96.0);
    double scaleY=(float)(s.GetHeight()/96.0);

    // Use x or y scaling factor, whichever fits on the DC
    double actualScale = wxMin(scaleX,scaleY);

    // Set the scale
    dc->SetUserScale(actualScale, actualScale);
    w = int(w/actualScale);
    h = int(h/actualScale);
    */
    
    // Let's have at least 50 device units margin
    int marginX = 50;
    int marginY = 50;

    // Calculate the position on the DC for centring the graphic
    float posX = marginX;
    float posY = marginY;

    // Set the origin
    dc->SetDeviceOrigin( (long)posX, (long)posY );

    handler->draw(*dc, w-2*marginX, h-2*marginY);
}



BEGIN_EVENT_TABLE(PenSampleCtrl, wxWindow)
EVT_PAINT(PenSampleCtrl::OnPaint)
END_EVENT_TABLE()

PenSampleCtrl::PenSampleCtrl(const wxPen& pen, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name) :
	wxWindow(parent, id, pos, size, style, name),
	m_pen(pen)
{
}

void PenSampleCtrl::OnPaint(wxPaintEvent& event)
{
  wxPaintDC dc(this);
  dc.SetPen(m_pen);
  wxCoord w,h;
  GetVirtualSize(&w, &h);
  h = h/2;
  dc.DrawLine(0, h, w, h);
}


BEGIN_EVENT_TABLE(PenSettingDialog, wxDialog)
EVT_COMBOBOX(XRCID("combo_box_width"), PenSettingDialog::OnWidth)
EVT_CHOICE(XRCID("choice_style"), PenSettingDialog::OnStyle)
EVT_BUTTON(XRCID("button_colour"), PenSettingDialog::OnColour)
END_EVENT_TABLE()

PenSettingDialog::PenSettingDialog(wxWindow* parent, const wxPen & pen)
{
    wxXmlResource::Get()->LoadDialog(this, parent, wxT("pen_setting_dlg"));
  pen_ctrl = new PenSampleCtrl(pen, this, wxID_ANY);
  wxXmlResource::Get()->AttachUnknownControl(wxT("pen_ctrl"), pen_ctrl, this);
    combo_box_width = XRCCTRL(*this, "combo_box_width", wxComboBox);
    combo_box_width->SetSelection(pen.GetWidth());
    choice_style = XRCCTRL(*this, "choice_style", wxChoice);
    switch (pen.GetStyle())
    {
      case wxSOLID :
	      choice_style->SetStringSelection(_("SOLID"));
	      break;
      case wxTRANSPARENT  :
	      choice_style->SetStringSelection(_("TRANSPARENT"));
	      break;
      case wxDOT  :
	      choice_style->SetStringSelection(_("DOT"));
	      break;
      case wxLONG_DASH  :
	      choice_style->SetStringSelection(_("LONG_DASH"));
	      break;
      case wxSHORT_DASH  :
	      choice_style->SetStringSelection(_("SHORT_DASH"));
	      break;
      case wxDOT_DASH  :
	      choice_style->SetStringSelection(_("DOT_DASH"));
	      break;
      case wxSTIPPLE  :
	      choice_style->SetStringSelection(_("STIPPLE"));
	      break;
      case wxUSER_DASH  :
	      choice_style->SetStringSelection(_("USER_DASH"));
	      break;
      case wxBDIAGONAL_HATCH  :
	      choice_style->SetStringSelection(_("BDIAGONAL_HATCH"));
	      break;
      case wxCROSSDIAG_HATCH  :
	      choice_style->SetStringSelection(_("CROSSDIAG_HATCH"));
	      break;
      case wxFDIAGONAL_HATCH  :
	      choice_style->SetStringSelection(_("FDIAGONAL_HATCH"));
	      break;
      case wxCROSS_HATCH  :
	      choice_style->SetStringSelection(_("CROSS_HATCH"));
	      break;
      case wxHORIZONTAL_HATCH  :
	      choice_style->SetStringSelection(_("HORIZONTAL_HATCH"));
	      break;
      case wxVERTICAL_HATCH  :
	      choice_style->SetStringSelection(_("VERTICAL_HATCH"));
	      break;
    }
}

void PenSettingDialog::OnWidth(wxCommandEvent& event)
{
  long i;
  if (combo_box_width->GetValue().ToLong(&i))
  {
    pen_ctrl->GetPen().SetWidth((int)i);
  }
    pen_ctrl->Refresh();
}

void PenSettingDialog::OnStyle(wxCommandEvent& event)
{
  wxString s = choice_style->GetStringSelection();
  if (s == _("SOLID"))
  {
    pen_ctrl->GetPen().SetStyle(wxSOLID);
  }
  else if (s == _("TRANSPARENT"))
  {
    pen_ctrl->GetPen().SetStyle(wxTRANSPARENT );
  }
  else if (s == _("DOT"))
  {
    pen_ctrl->GetPen().SetStyle(wxDOT );
  }
  else if (s == _("LONG_DASH"))
  {
    pen_ctrl->GetPen().SetStyle(wxLONG_DASH );
  }
  else if (s == _("SHORT_DASH"))
  {
    pen_ctrl->GetPen().SetStyle(wxSHORT_DASH );
  }
  else if (s == _("DOT_DASH"))
  {
    pen_ctrl->GetPen().SetStyle(wxDOT_DASH );
  }
  else if (s == _("BDIAGONAL_HATCH"))
  {
    pen_ctrl->GetPen().SetStyle(wxBDIAGONAL_HATCH);
  }
  else if (s == _("CROSSDIAG_HATCH"))
  {
    pen_ctrl->GetPen().SetStyle(wxCROSSDIAG_HATCH );
  }
  else if (s == _("FDIAGONAL_HATCH"))
  {
    pen_ctrl->GetPen().SetStyle(wxFDIAGONAL_HATCH );
  }
  else if (s == _("CROSS_HATCH"))
  {
    pen_ctrl->GetPen().SetStyle(wxCROSS_HATCH );
  }
  else if (s == _("HORIZONTAL_HATCH"))
  {
    pen_ctrl->GetPen().SetStyle(wxHORIZONTAL_HATCH );
  }
  else if (s == _("VERTICAL_HATCH"))
  {
    pen_ctrl->GetPen().SetStyle(wxVERTICAL_HATCH );
  }
    pen_ctrl->Refresh();
}

void PenSettingDialog::OnColour(wxCommandEvent& event)
{
  wxColour c = wxGetColourFromUser(this, pen_ctrl->GetPen().GetColour(), _("Colour Setting"));
  if (c.IsOk())
  {
    pen_ctrl->GetPen().SetColour(c);
    pen_ctrl->Refresh();
  }
}

