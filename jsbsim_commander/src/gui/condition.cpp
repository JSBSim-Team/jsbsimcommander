/////////////////////////////////////////////////////////////////////////////
// Name:        condition.cpp
// Purpose:     support FGCondition in FCSBuilder
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
#include "condition.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(ConditionList);

Condition::Condition() :
  isGroup(false),
  Logic(elUndef),
  Comparison(ecUndef)
{
  conditions.DeleteContents(true);
}

Condition::Condition(const Condition & cond)
{
  cond.Copy(*this);
}

Condition & Condition::operator = (const Condition & cond)
{
  cond.Copy(*this);
  return *this;
}

void Condition::Copy(Condition & copy) const
{
  copy.conditions.DeleteContents(true);
  copy.conditions.Clear();
  if (isGroup)
  {
    wxConditionListNode * node = conditions.GetFirst();
    while (node)
    {
      Condition * data = node->GetData();
      copy.conditions.Append(new Condition(*data));
      node = node->GetNext();
    }
  }
  copy.isGroup    = isGroup;
  copy.Logic      = Logic;
  copy.TestParam1 = TestParam1;
  copy.TestParam2 = TestParam2;
  copy.Comparison = Comparison;
}

#if wxUSE_PROLOGIO
void Condition::WriteAttributes(wxExpr * list)
{
  if (isGroup)
  {
    list->Append(new wxExpr(1l));
    list->Append(new wxExpr((long)Logic));
    wxConditionListNode * node = conditions.GetFirst();
    while (node)
    {
      Condition * data = node->GetData();
      wxExpr *condExpr = new wxExpr(wxExprList);
      data->WriteAttributes(condExpr);
      list->Append(condExpr);
      node = node->GetNext();
    }
  }
  else
  {
    list->Append(new wxExpr(0l));
    list->Append(new wxExpr(wxExprString,TestParam1));
    list->Append(new wxExpr(wxExprString,TestParam2));
    list->Append(new wxExpr((long)Comparison));
  }
}

void Condition::ReadAttributes(wxExpr * list)
{
  wxExpr * expr = list->GetFirst();
  
  if (expr->IntegerValue()==0u)
  {
    isGroup = false;
    expr = expr->GetNext();
    TestParam1 = expr->StringValue();
    expr = expr->GetNext();
    TestParam2 = expr->StringValue();
    expr = expr->GetNext();
    Comparison = (eComparison)expr->IntegerValue();
  }
  else
  {
    isGroup = true;
    expr    = expr->GetNext();
    Logic   = (eLogic)expr->IntegerValue();
    expr    = expr->GetNext();
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
}
#endif

void Condition::ExportXML(wxTextOutputStream & stream, const wxString & prefix, const wxArrayString & inputs)
{
  if (isGroup)
  {
    wxString logic;
    switch (Logic)
    {
      case eAND :
	logic = wxT("AND");
	break;
      case eOR :
	logic = wxT("OR");
	break;
      default :
	logic = wxT("Undef");
    }
    stream << prefix << wxT("<CONDITION_GROUP LOGIC=\"") << logic << wxT("\">") << endl;
    
    wxString Pre = prefix + wxT("  ");
    wxConditionListNode * node = conditions.GetFirst();
    while (node)
    {
      Condition * data = node->GetData();
      data->ExportXML(stream, Pre, inputs);
      node = node->GetNext();
    }
  
    stream << prefix << wxT("</CONDITION_GROUP>") << endl;
  }
  else
  {
    wxString param1;    
    if (TestParam1.StartsWith(wxT("$INPUT"),&param1))
    {
      unsigned long val = 1ul;
      param1.ToULong(&val);
      param1 = inputs[(int)val-1];
    }
    else
    {
      param1 = TestParam1;
    }
    
    wxString param2;
    if (TestParam2.StartsWith(wxT("$INPUT"),&param2))
    {
      unsigned long val = 1ul;
      param2.ToULong(&val);
      param2 = inputs[(int)val-1];
    }
    else
    {
      param2 = TestParam2;
    }
    
    wxString comp;
    switch (Comparison)
    {
      case eEQ :
	comp = wxT(" == ");
	break;
      case eNE :
	comp = wxT(" != ");
	break;
      case eGT :
	comp = wxT(" gt ");
	break;
      case eGE :
	comp = wxT(" ge ");
	break;
      case eLT :
	comp = wxT(" lt ");
	break;
      case eLE :
	comp = wxT(" le ");
	break;
      default :
	comp = wxT(" le ");
	break;
    }
    
    stream << prefix << param1 << comp << param2 << endl;
  }

}

void Condition::ImportXML(JSBSim::Element * element, wxArrayString & array)
{
  //todo
  string logic;
  JSBSim::Element* condition_element;

  isGroup = true;
  Logic       = elUndef;
  conditions.clear();

  logic = element->GetAttributeValue("logic");
  if (logic == "OR") Logic = eOR;
  else if (logic == "AND") Logic = eAND;
  else { // error
    cerr << "Unrecognized LOGIC token " << logic << " in switch component: " << logic << endl;
  }
  condition_element = element->GetElement();
  while (condition_element) {
    Condition * cond = new Condition;
    cond->ImportXML(condition_element, array);
    conditions.Append(cond);
    condition_element = element->GetNextElement();
  }
  for (int i=0; i<element->GetNumDataLines(); i++) {
    Condition * cond = new Condition;
    cond->ImportXML(element->GetDataLine(i), array);
    conditions.Append(cond);
  }
}

void Condition::ImportXML(string test, wxArrayString & array)
{
  //todo
  string property1, property2, conditional;

  map <string, eComparison> mComparison;
  mComparison["EQ"] = eEQ;
  mComparison["NE"] = eNE;
  mComparison["GT"] = eGT;
  mComparison["GE"] = eGE;
  mComparison["LT"] = eLT;
  mComparison["LE"] = eLE;
  mComparison["eq"] = eEQ;
  mComparison["ne"] = eNE;
  mComparison["gt"] = eGT;
  mComparison["ge"] = eGE;
  mComparison["lt"] = eLT;
  mComparison["le"] = eLE;
  mComparison["=="] = eEQ;
  mComparison["!="] = eNE;
  mComparison[">"]  = eGT;
  mComparison[">="] = eGE;
  mComparison["<"]  = eLT;
  mComparison["<="] = eLE;

  isGroup = false;
  Comparison  = ecUndef;
  Logic       = elUndef;
  conditions.clear();

  int start = 0, end = 0;
  start = test.find_first_not_of(" ");
  end = test.find_first_of(" ", start+1);
  property1 = test.substr(start,end-start);
  start = test.find_first_not_of(" ",end);
  end = test.find_first_of(" ",start+1);
  conditional = test.substr(start,end-start);
  start = test.find_first_not_of(" ",end);
  end = test.find_first_of(" ",start+1);
  property2 = test.substr(start,end-start);

  TestParam1 = alias(property1.c_str(), array);
  Comparison = mComparison[conditional];
  if (property2.find_first_not_of("-.0123456789eE") == string::npos) {
    TestParam2 = property2.c_str();
  } else {
    TestParam2 = alias(property2.c_str(), array);
  }
}


wxString alias(wxString value, wxArrayString & array)
{
  int result = array.Index(value, false, true);
  if ( result == wxNOT_FOUND || result == 0 )
  {
    result = array.Add(value);
  }
  return wxT("$INPUT") + wxString::Format("%d", result);
}


