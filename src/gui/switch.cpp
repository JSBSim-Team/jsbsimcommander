/////////////////////////////////////////////////////////////////////////////
// Name:        switch.cpp
// Purpose:     support FGSwitch in FCSBuilder
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
#include "switch.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(TestList);

Test::Test() :
  Logic(elUndef)
{
  conditions.DeleteContents(true);
}

Test::Test(const Test & t)
{
  t.Copy(*this);
}

Test & Test::operator = (const Test & t)
{
  t.Copy(*this);
  return *this;
}

void Test::Copy(Test & copy) const
{
  copy.conditions.DeleteContents(true);
  copy.conditions.Clear();
  wxConditionListNode * node = conditions.GetFirst();
  while (node)
  {
    Condition * data = node->GetData();
    copy.conditions.Append(new Condition(*data));
    node = node->GetNext();
  }
  copy.Output     = Output;
  copy.Logic      = Logic;
}

#if wxUSE_PROLOGIO
void Test::WriteAttributes(wxExpr * list)
{
  list->Append(new wxExpr((long)Logic));
  list->Append(new wxExpr(wxExprString,Output));

  wxConditionListNode * node = conditions.GetFirst();
  while (node)
  {
    Condition * data = node->GetData();
    wxExpr *pointExpr = new wxExpr(wxExprList);
    data->WriteAttributes(pointExpr);
    list->Append(pointExpr);
    node = node->GetNext();
  }
}

void Test::ReadAttributes(wxExpr * list)
{
  wxExpr  * expr = list->GetFirst();
  
  Logic = (eLogic)expr->IntegerValue();
  expr  = expr->GetNext();

  Output = expr->StringValue();
  expr   = expr->GetNext();

  conditions.DeleteContents(true);
  conditions.Clear();
  while (expr)
  {
    Condition * data = new Condition;
    data->ReadAttributes(expr);
    conditions.Append(data);
    expr = expr->GetNext();
  }
}
#endif


void 
Test::ExportXML(wxTextOutputStream & stream, const wxString & prefix, const wxArrayString & inputs)
{
  wxString value;
  if (Output.StartsWith(wxT("$INPUT"),&value))
  {
    unsigned long val = 1ul;
    value.ToULong(&val);
    value = inputs[(int)val-1];
  }
  else if (Output.StartsWith(wxT("-$INPUT"),&value))
  {
    unsigned long val = 1ul;
    value.ToULong(&val);
    value = wxT("-") + inputs[(int)val-1];
  }
  else
  {
    value = Output;
  }
  
  if (Logic == eDefault)
  {
    stream << prefix << wxT("<default value=\"") << value << wxT("\"/>") << endl;
    return;
  }
  
  if (Logic == eAND)
  {
    stream << prefix << wxT("<test logic=\"AND\" value=\"") << value << wxT("\">") << endl;  
  }
  else if (Logic == eOR)
  {
    stream << prefix << wxT("<test logic=\"OR\" value=\"") << value << wxT("\">") << endl;    
  }
  else
  {
    std::cerr<< "strange things happened. undefined test." << std::endl;
  }

  wxString Pre = prefix + wxT("  ");
  
  wxConditionListNode * node = conditions.GetFirst();
  while (node)
  {
    Condition * data = node->GetData();
    data->ExportXML(stream, Pre, inputs);
    node = node->GetNext();
  }
  
  if (Logic == eAND || Logic == eOR)
    stream << prefix << wxT("</test>") << endl;
}


void Test::ImportXML(JSBSim::Element * test_element, wxArrayString & array)
{
    string value, logic;
    JSBSim::Element  *condition_element;
    conditions.Clear();
    
    if (test_element->GetName() == "default") {
      Logic = Test::eDefault;
    } else if (test_element->GetName() == "test") { // here's a test
      logic = test_element->GetAttributeValue("logic");
      if (logic == "OR") Logic = Test::eOR;
      else if (logic == "AND") Logic = Test::eAND;
      else { // error
        cerr << "Unrecognized LOGIC token " << logic <<  endl;
      }
      for (int i=0; i<test_element->GetNumDataLines(); i++)
      {
	Condition * cond = new Condition;
	cond->ImportXML( test_element->GetDataLine(i), array );
        conditions.Append( cond );
      }

      condition_element = test_element->GetElement(); // retrieve condition groups
      while (condition_element) {
	Condition * cond = new Condition;
	cond->ImportXML( condition_element, array );
        conditions.Append(cond);
        condition_element = test_element->GetNextElement();
      }

    }

    if (test_element->GetName() != "output") { // this is not an output element
      value = test_element->GetAttributeValue("value").c_str();
      if (value.empty()) {
        cerr << "No VALUE supplied for switch component" << endl;
      }
        if (value.find_first_not_of("-.0123456789eE") == string::npos) {
          // if true (and execution falls into this block), "value" is a number.
          Output = value.c_str();
        } else {
          // "value" must be a property if execution passes to here.
	  wxString sign;
          if (value[0] == '-') {
            sign = "-";
            value.erase(0,1);
          } else {
            sign = "";
          }
          Output = sign + alias(value.c_str(), array);
        }
      
    }
}


IMPLEMENT_DYNAMIC_CLASS (Switch, ComponentShape)

Switch::Switch (double w, double h, const wxString & Name )
  :ComponentShape(w, h, wxT("SWITCH"), Name)
{
  SetAttachmentMode (ATTACHMENT_MODE_EDGE);

  ClearAttachments ();
  GetAttachments ().Append (new wxAttachmentPoint (0, w * 0.5, 0.0));
  GetAttachments ().Append (new wxAttachmentPoint (1, -w * 0.5, 15.0));
  GetAttachments ().Append (new wxAttachmentPoint (2, -w * 0.5, -15.0));

  input_sign_list.Append(new bool(false));
  input_sign_list.Append(new bool(false));

  testlist.DeleteContents(true);
  Test * t = new Test;
  t->Logic = Test::eDefault;
  t->Output = wxT("0");
  testlist.Append(t);
}

void
Switch::OnDraw (wxDC & dc)
{
  ComponentShape::OnDraw (dc);
  wxCoord  h, w, r;
  h = (int)m_height/6;
  w = (int)m_width/6;
  r = 3;
  dc.DrawCircle((wxCoord)m_xpos-w, (wxCoord)m_ypos-h, r);
  dc.DrawCircle((wxCoord)m_xpos-w, (wxCoord)m_ypos+h, r);
  dc.DrawCircle((wxCoord)m_xpos+w, (wxCoord)m_ypos, r);
  wxPoint c[2];
  c[0].x = -(int)m_width/2 + 2*arr_size;
  c[0].y = -h;
  c[1].x = -w-r;
  c[1].y = c[0].y;
  dc.DrawLines (2, c, WXROUND (m_xpos), WXROUND (m_ypos));
  c[0].x = -(int)m_width/2 + 2*arr_size;
  c[0].y = h;
  c[1].x = -w-r;
  c[1].y = c[0].y;
  dc.DrawLines (2, c, WXROUND (m_xpos), WXROUND (m_ypos));
  c[0].x = (int)m_width/2 - 2*arr_size;
  c[0].y = 0;
  c[1].x = w+r;
  c[1].y = c[0].y;
  dc.DrawLines (2, c, WXROUND (m_xpos), WXROUND (m_ypos));
  c[0].x = -w + r;
  c[0].y = -h;
  c[1].x = w - r;
  c[1].y = 0;
  dc.DrawLines (2, c, WXROUND (m_xpos), WXROUND (m_ypos));
}

#if wxUSE_PROLOGIO
void 
Switch::WriteAttributes (wxExpr * clause)
{
  ComponentShape::WriteAttributes (clause);

  // Write user-defined attachment points, if any
  if (testlist.GetCount() > 0)
  {
    wxExpr *list = new wxExpr(wxExprList);
    wxTestListNode *node = testlist.GetFirst();
    while (node)
    {
      Test *point = node->GetData();
      wxExpr *pointExpr = new wxExpr(wxExprList);
      point->WriteAttributes(pointExpr);
      list->Append(pointExpr);
      node = node->GetNext();
    }
    clause->AddAttributeValue(wxT("Test List"), list);
  }

}


void 
Switch::ReadAttributes (wxExpr * clause)
{
  ComponentShape::ReadAttributes (clause);

  testlist.Clear();
  wxExpr *list = clause->AttributeValue(wxT("Test List"));
  if (list)
  {
    wxExpr *pointExpr = list->GetFirst();
    while (pointExpr)
    {
      Test *point = new Test;
      point->ReadAttributes(pointExpr);
      testlist.Append(point);
      pointExpr = pointExpr->GetNext();
    }
  }
}
#endif

void 
Switch::Copy (wxShape & copy)
{
  ComponentShape::Copy (copy);

  wxASSERT (copy.IsKindOf (CLASSINFO (Switch)));

  Switch & SwitchCopy = (Switch &) copy;

  SwitchCopy.testlist.DeleteContents(true);
  SwitchCopy.testlist.Clear();
  wxTestListNode * node = testlist.GetFirst();
  while (node)
  {
    Test * data = node->GetData();
    SwitchCopy.testlist.Append(new Test(*data));
    node = node->GetNext();
  }
}


void 
Switch::ExportXML(wxTextOutputStream & stream, const wxString & prefix)
{
  ExportHead(stream, prefix);

  wxString Pre = prefix + wxT("  ");
 
  wxArrayString inputs = GetInputs();
  wxTestListNode * node = testlist.GetFirst();
  while (node)
  {
    Test * data = node->GetData();
    data->ExportXML(stream, Pre, inputs);
    node = node->GetNext();
  }
  
  ExportCliper(stream,Pre);
  ExportOutput(stream,Pre);
  
  ExportTail(stream, prefix);
}

wxArrayString
Switch::GetInputs()
{
  wxArrayString tmp;
  {
    int n = input_sign_list.GetCount();
    for (int i=0; i<n; ++i)
    {
      tmp.Add(wxT("(noname)")); //default name. it should be replaced.
    }
  }
  wxNode * node = GetLines().GetFirst();
  while (node)
  {
    wxLineShape * data = (wxLineShape *)node->GetData();
    if (data->GetTo() == this)  // if this line point to this shape
    {
      wxShape * shape = data->GetFrom(); // input shape
      while (shape && shape->IsKindOf(CLASSINFO(SIMOShape))) //if input shape is a SIMO Shape, find the input shape of the SIMO Shape
      {
        wxNode * nd = shape->GetLines().GetFirst();
	bool flag = false;
	while (nd)
	{
	  wxLineShape * value = (wxLineShape *)nd->GetData();
	  if (value->GetTo() == shape)
	  {
	    shape = value->GetFrom();  // get it!
	    flag = true;
	    break;
	  }
          nd = nd->GetNext();
	}
	if (!flag)
	{
	  shape = 0; // no find
	}
      }
      //output
      if (shape && shape->IsKindOf(CLASSINFO(MISOShape)))
      {
	 int a = data->GetAttachmentTo();
	 bool b = *(input_sign_list.Item(a-1)->GetData());
	 wxString c;
	 if (b)
	   c = wxT("-");
         MISOShape * misoshape = (MISOShape *)shape;
	 if (misoshape->IsKindOf(CLASSINFO(ComponentShape)))
	   tmp[a-1] =  c + mkName( misoshape->GetName(), true ) ;
	 else
	   tmp[a-1] =  c + misoshape->GetName() ;
      }
    }
    node = node->GetNext();
  }
  return tmp;
}


wxArrayString
Switch::ImportXML(JSBSim::Element * el)
{
  wxArrayString strings = ComponentShape::ImportXML(el);

  testlist.Clear();
  JSBSim::Element *test_element;

  test_element = el->GetElement();
  while (test_element && test_element->GetName()!="output") {
    Test * t = new Test;
    t->ImportXML(test_element, strings);
    testlist.Append(t);
    test_element = el->GetNextElement();
  }

  double w = GetWidth();
  double h = GetHeight();
  for (size_t i = 1; i<strings.Count(); ++i)
  {
      input_sign_list.Append(new bool(false));
      GetAttachments ().Append (new wxAttachmentPoint (i, -w * 0.5, 0.0));
  }
  SetAttachmentSize(w,h);

  return strings;
}


