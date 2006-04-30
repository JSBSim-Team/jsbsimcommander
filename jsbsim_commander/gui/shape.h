/////////////////////////////////////////////////////////////////////////////
// Name:        shape.h
// Purpose:     shape classes
// Author:      Matthew Gong
// Created:     02/04/2005
// Copyright:   (c) Matthew Gong
// Licence:     GPL licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _FCSBUILDER_SHAPE_H_
#define _FCSBUILDER_SHAPE_H_

#if defined(__GNUG__) && !defined(__APPLE__)
// #pragma interface
#endif

#include <wx/deprecated/setup.h>

#if wxUSE_PROLOGIO
#include <wx/deprecated/wxexpr.h>
#endif

#include <wx/txtstrm.h>
#include "wx/ogl/ogl.h"

#include "FGXMLElement.h"

/*
 * All shape event behaviour is routed through this handler, so we don't
 * have to derive from each shape class. We plug this in to each shape.
 */
 
class MyEvtHandler: public wxShapeEvtHandler
{
 public:
  MyEvtHandler(wxShapeEvtHandler *prev = NULL, wxShape *shape = NULL)
    :wxShapeEvtHandler(prev, shape)
  {
  }
  ~MyEvtHandler(void)
  {
  }

  void OnLeftClick(double x, double y, int keys = 0, int attachment = 0);
  void OnRightClick(double x, double y, int keys = 0, int attachment = 0);
  void OnBeginDragRight(double x, double y, int keys = 0, int attachment = 0);
  void OnDragRight(bool draw, double x, double y, int keys = 0, int attachment = 0);
  void OnEndDragRight(double x, double y, int keys = 0, int attachment = 0);
};


/*
 * shape classes so we have a 1:1 mapping
 * between palette symbol and unique class
 */


/*
 * Single Input Multi Output , use it as a node of lines
 */
class SIMOShape:public wxEllipseShape
{
  DECLARE_DYNAMIC_CLASS (SIMOShape)
public:
  SIMOShape (double w = 0.0, double h = 0.0);
};


/*
 * Multi Input Single Output, the base class for all shapes
 */
class MISOShape:public wxRectangleShape
{
  DECLARE_DYNAMIC_CLASS (MISOShape) 
protected:
  bool is_draw_reverse;
  unsigned int arr_size;

  wxString type;
  wxString name;

  void SetNodePos ();
  void ShowType(wxDC & dc);

public:
  MISOShape (double w = 0.0, double h = 0.0, const wxString & Type = wxT("notype"), const wxString & Name = wxT("noname"));

  //called after addline or move to adjust the line(s) to the shape
  void NormalizeLine (wxLineShape * line);
  void NormalizeLines ();

  inline bool IsDrawReverse () const
    {
      return is_draw_reverse;
    }
  inline void SetDrawReverse (const bool & b)
    {
      is_draw_reverse = b;
      SetNodePos ();
      NormalizeLines ();
    }

  inline unsigned int GetArrowSize() const
    {
      return arr_size;
    }
  inline void SetArrowSize(const unsigned int & s)
    {
      arr_size = s;
    }

  inline wxString GetName() const
    {
      return name;
    }
  inline void SetName(const wxString & s)
    {
      name = s;
    }

  inline wxString GetType() const
    {
      return type;
    }
  inline void SetType(const wxString & t)
    {
      type = t;
    }

#if wxUSE_PROLOGIO
  void WriteAttributes (wxExpr * clause);
  void ReadAttributes (wxExpr * clause);
#endif

  // Does the copying for this object
  void Copy (wxShape & copy);

  void OnEndSize (double x, double y);
  void OnMovePost (wxDC & dc, double x, double y, double oldX, double oldY,
		   bool display = TRUE);
  void OnDraw (wxDC & dc);
  void OnChangeAttachment (int attachment, wxLineShape * line,
			   wxList & ordering);
  void SetAttachmentSize (double w, double h);

  int GetNumberOfAttachments () const;
  bool GetAttachmentPosition (int attachment, double *x, double *y,
			      int nth = 0, int no_arcs = 1,
			      wxLineShape * line = NULL);
  void SetSize (double x, double y, bool recursive = true);
};

class SourceShape:public MISOShape
{
  DECLARE_DYNAMIC_CLASS (SourceShape)
public:
  SourceShape (double w = 0.0, double h = 0.0, const wxString & Name = wxT("noname"));
  void OnDraw (wxDC & dc);
};

class DestinationShape:public MISOShape
{
  DECLARE_DYNAMIC_CLASS (DestinationShape)
public:
  DestinationShape (double w = 0.0, double h = 0.0, const wxString & Name = wxT("noname"));
  void OnDraw (wxDC & dc);
};


/*
 * for input sign list
 */

WX_DECLARE_LIST(bool,MyBoolList);


/*
 * basic compontment for all
 */

class ComponentShape:public MISOShape
{
  DECLARE_DYNAMIC_CLASS (ComponentShape)

protected:
  //order to run in the line of all FCS componments
  long int order;

  //clipable and cliper
  bool clipable;
  wxString clipmax;
  wxString clipmin;
  
  //the num of input ports and the sign of them( + -- false; - -- true)
  MyBoolList input_sign_list;
  
public:
  static wxString mkName(wxString name, bool lowercase);

  ComponentShape (double w = 0.0, double h = 0.0, const wxString & Type = wxT("componment"), const wxString & Name = wxT("noname"));
  virtual ~ComponentShape();
  
  void OnDraw (wxDC & dc);
#if wxUSE_PROLOGIO
  void WriteAttributes (wxExpr * clause);
  void ReadAttributes (wxExpr * clause);
#endif
  virtual void ExportXML(wxTextOutputStream & stream, const wxString & prefix);
  void ExportInputs(wxTextOutputStream & stream, const wxString & prefix);
  void ExportOutput(wxTextOutputStream & stream, const wxString & prefix);
  void ExportCliper(wxTextOutputStream & stream, const wxString & prefix);
   
  inline void ExportHead(wxTextOutputStream & stream, const wxString & prefix)
  {
    stream << endl << prefix << wxT("<component name=\"") << name << wxT("\" type=\"") << type << wxT("\">") << endl;
  }
    
  inline void ExportTail(wxTextOutputStream & stream, const wxString & prefix)
  {
    stream<< prefix << wxT("</component>") << endl;  
  }

  virtual wxArrayString ImportXML(JSBSim::Element * el);

  static DestinationShape * GetOutputNode(SIMOShape * shape);
  // Does the copying for this object
  void Copy (wxShape & copy);

  inline bool IsClipable() const
    {
      return clipable;
    }
  inline void SetClipable(const bool & b)
    {
      clipable = b;
    }
  inline wxString GetClipMax() const
    {
      return clipmax;
    }
  inline void SetClipMax(const wxString & d)
    {
      clipmax = d;
    }
  inline wxString GetClipMin() const
    {
      return clipmin;
    }
  inline void SetClipMin(const wxString & d)
    {
      clipmin = d;
    }

  inline MyBoolList& GetInputSignList()
    {
      return input_sign_list;
    }

  inline long int GetOrder() const
    {
      return order;
    }
  //check and set
  void SetOrder();
  void SetOrder(long int & o);

};


#endif // _FCSBUILDER_SHAPE_H_
