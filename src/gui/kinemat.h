/////////////////////////////////////////////////////////////////////////////
// Name:        kinemat.h
// Purpose:     shape for FGKinemat
// Author:      Matthew Gong
// Created:     02/05/2005
// Copyright:   (c) Matthew Gong
// Licence:     GPL licence
/////////////////////////////////////////////////////////////////////////////


#ifndef _FCSBUILDER_KINEMAT_H_
#define _FCSBUILDER_KINEMAT_H_

#if defined(__GNUG__) && !defined(__APPLE__)
// #pragma interface
#endif

struct MySettingPair
{
  double pos;
  double time;
};

WX_DECLARE_LIST(MySettingPair,MySettingPairList);


/*
 * FGKinemat Componment
 */

class Kinemat: public ComponentShape
{
  DECLARE_DYNAMIC_CLASS (Kinemat)

protected:
  bool isscale;
  MySettingPairList pairlist;

public:
  Kinemat (double w = 0.0, double h = 0.0, const wxString & Name = wxT("kinemat"));
  virtual ~Kinemat();
  void OnDraw (wxDC & dc);
  
#if wxUSE_PROLOGIO
  void WriteAttributes (wxExpr * clause);
  void ReadAttributes (wxExpr * clause);
#endif
  virtual void ExportXML(wxTextOutputStream & stream, const wxString & prefix);

  virtual wxArrayString ImportXML(JSBSim::Element * el);

  // Does the copying for this object
  void Copy (wxShape & copy);

  inline void SetScale(const bool & b)
    {
      isscale = b;
    }
  inline bool IsScale() const
    {
      return isscale;
    }
  MySettingPairList & GetPairList()
    {
      return pairlist;
    }
};

#endif // _FCSBUILDER_KINEMAT_H_
