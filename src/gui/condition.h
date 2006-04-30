/////////////////////////////////////////////////////////////////////////////
// Name:        condition.h
// Purpose:     shape for FGCondition
// Author:      Matthew Gong
// Created:     02/05/2005
// Copyright:   (c) Matthew Gong
// Licence:     GPL licence
/////////////////////////////////////////////////////////////////////////////


#ifndef _FCSBUILDER_CONDITION_H_
#define _FCSBUILDER_CONDITION_H_

#if defined(__GNUG__) && !defined(__APPLE__)
// #pragma interface
#endif

extern wxString alias(wxString value, wxArrayString & array);

/*
 * FGCondition 
 */
class Condition;
WX_DECLARE_LIST(Condition,ConditionList);

class TestPropertyDialog;
class ConditionPropertyDialog;
class Condition
{
public:
  enum eComparison {ecUndef=0, eEQ, eNE, eGT, eGE, eLT, eLE};
  enum eLogic {elUndef=0, eAND, eOR};

protected:
  bool isGroup;
  
  eLogic Logic;
  ConditionList conditions;
 
  wxString TestParam1, TestParam2;
  eComparison Comparison;

public:
  Condition ();
  Condition (const Condition & cond);
  Condition & operator =(const Condition & cond);
  ~Condition () {}
  
#if wxUSE_PROLOGIO
  void WriteAttributes (wxExpr * list);
  void ReadAttributes (wxExpr * list);
#endif
  void ExportXML(wxTextOutputStream & stream, const wxString & prefix, const wxArrayString & inputs);

  void ImportXML(JSBSim::Element * el, wxArrayString & array);
  void ImportXML(string test , wxArrayString & array);

  // Does the copying for this object
  void Copy (Condition & copy) const;

  friend class TestPropertyDialog;
  friend class ConditionPropertyDialog;
  friend class SwitchPropertyDialog;
};

#endif // _FCSBUILDER_CONDITION_H_
