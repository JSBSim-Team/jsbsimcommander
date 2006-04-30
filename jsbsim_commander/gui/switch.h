/////////////////////////////////////////////////////////////////////////////
// Name:        switch.h
// Purpose:     shape for FGSwitch
// Author:      Matthew Gong
// Created:     02/05/2005
// Copyright:   (c) Matthew Gong
// Licence:     GPL licence
/////////////////////////////////////////////////////////////////////////////


#ifndef _FCSBUILDER_SWITCH_H_
#define _FCSBUILDER_SWITCH_H_

#if defined(__GNUG__) && !defined(__APPLE__)
// #pragma interface
#endif

#include "condition.h"

/*
 * FGSwitch Componment
 */

class Switch;
class SwitchPropertyDialog;
class TestPropertyDialog;
class Test
{
  public:
    enum eLogic {elUndef=0, eAND, eOR, eDefault};
  protected:
    eLogic Logic;
    wxString Output;
    ConditionList conditions;
  public:
   Test();
   Test(const Test & cond);
   Test& operator =(const Test & cond);
   ~Test() {}
  
#if wxUSE_PROLOGIO
  void WriteAttributes (wxExpr * list);
  void ReadAttributes (wxExpr * list);
#endif
  void ExportXML(wxTextOutputStream & stream, const wxString & prefix, const wxArrayString & inputs);

  void ImportXML(JSBSim::Element * el, wxArrayString & array);

  // Does the copying for this object
  void Copy (Test& copy) const;

  friend class Switch;
  friend class SwitchPropertyDialog;
  friend class TestPropertyDialog;
};

WX_DECLARE_LIST(Test,TestList);

class Switch: public ComponentShape
{
  DECLARE_DYNAMIC_CLASS (Switch)

protected:
  TestList testlist;

public:
  Switch (double w = 0.0, double h = 0.0, const wxString & Name = wxT("switch"));
  void OnDraw (wxDC & dc);
  
#if wxUSE_PROLOGIO
  void WriteAttributes (wxExpr * clause);
  void ReadAttributes (wxExpr * clause);
#endif
  virtual void ExportXML(wxTextOutputStream & stream, const wxString & prefix);

  virtual wxArrayString ImportXML(JSBSim::Element * el);

  wxArrayString GetInputs();
  // Does the copying for this object
  void Copy (wxShape & copy);

  inline TestList & GetTestList()
    {
      return testlist;
    }
};

#endif // _FCSBUILDER_SWITCH_H_

