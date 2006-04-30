/////////////////////////////////////////////////////////////////////////////
// Name:        func.h
// Purpose:     shape for FGFCSFunction
// Author:      Matthew Gong
// Created:     07/24/2005
// Copyright:   (c) Matthew Gong
// Licence:     GPL licence
/////////////////////////////////////////////////////////////////////////////


#ifndef _FCSBUILDER_FCSFUNCTION_H_
#define _FCSBUILDER_FCSFUNCTION_H_

#if defined(__GNUG__) && !defined(__APPLE__)
// #pragma interface
#endif

class TreeItemData_function;
#include <wx/treectrl.h>

class FCSFunctionDialog;

void mv2str(JSBSim::Element * el, wxTextOutputStream & out);

class FCSFunction: public ComponentShape
{
  DECLARE_DYNAMIC_CLASS (FCSFunction)

protected:
  wxString function;

public:
  FCSFunction (double w = 0.0, double h = 0.0, const wxString & Name = wxT("function"));
  ~FCSFunction();
  void OnDraw (wxDC & dc);
  
#if wxUSE_PROLOGIO
  void WriteAttributes (wxExpr * clause);
  void ReadAttributes (wxExpr * clause);
#endif
  virtual void ExportXML(wxTextOutputStream & stream, const wxString & prefix);

  virtual wxArrayString ImportXML(JSBSim::Element * el);

  // Does the copying for this object
  void Copy (wxShape & copy);

  friend class FCSFunctionDialog;
};

#endif // _FCSBUILDER_FCSFUNCTION_H_

