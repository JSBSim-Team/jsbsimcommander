/////////////////////////////////////////////////////////////////////////////
// Name:        filter.h
// Purpose:     shape for FGFilter
// Author:      Matthew Gong
// Created:     02/05/2005
// Copyright:   (c) Matthew Gong
// Licence:     GPL licence
/////////////////////////////////////////////////////////////////////////////


#ifndef _FCSBUILDER_FILTER_H_
#define _FCSBUILDER_FILTER_H_

#if defined(__GNUG__) && !defined(__APPLE__)
// #pragma interface
#endif

/*
 * FGFilter Componment
 */

class Filter: public ComponentShape
{
  DECLARE_DYNAMIC_CLASS (Filter)

protected:
  wxString c1;
  wxString c2;
  wxString c3;
  wxString c4;
  wxString c5;
  wxString c6;

  wxString trigger;

public:
  Filter (double w = 0.0, double h = 0.0, const wxString & Type = wxT("LAG_FILTER"), const wxString & Name = wxT("filter"));
  virtual ~Filter();
  void OnDraw (wxDC & dc);
  
#if wxUSE_PROLOGIO
  void WriteAttributes (wxExpr * clause);
  void ReadAttributes (wxExpr * clause);
#endif
  virtual void ExportXML(wxTextOutputStream & stream, const wxString & prefix);

  virtual wxArrayString ImportXML(JSBSim::Element * el);

  // Does the copying for this object
  void Copy (wxShape & copy);

  inline wxString GetC1() const
    {
      return c1;
    }
  inline void SetC1(const wxString & d)
    {
      c1=d;
    }
  inline wxString GetC2() const
    {
      return c2;
    }
  inline void SetC2(const wxString & d)
    {
      c2=d;
    }
  inline wxString GetC3() const
    {
      return c3;
    }
  inline void SetC3(const wxString & d)
    {
      c3=d;
    }
  inline wxString GetC4() const
    {
      return c4;
    }
  inline void SetC4(const wxString & d)
    {
      c4=d;
    }
  inline wxString GetC5() const
    {
      return c5;
    }
  inline void SetC5(const wxString & d)
    {
      c5=d;
    }
  inline wxString GetC6() const
    {
      return c6;
    }
  inline void SetC6(const wxString & d)
    {
      c6=d;
    }
  inline wxString GetTrigger() const
    {
      return trigger;
    }
  inline void SetTrigger(const wxString & s)
    {
      trigger = s;
    }
};


#endif  // _FCSBUILDER_FILTER_H_
