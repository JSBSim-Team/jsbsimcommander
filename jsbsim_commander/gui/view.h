/////////////////////////////////////////////////////////////////////////////
// Name:        view.h
// Purpose:     View-related classes
// Author:      Matthew Gong
// Created:     02/04/2005
// Copyright:   (c) Matthew Gong
// Licence:     GPL licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _FCSBUILDER_VIEW_H_
#define _FCSBUILDER_VIEW_H_

#if defined(__GNUG__) && !defined(__APPLE__)
// #pragma interface "view.h"
#endif

#include "wx/ogl/ogl.h"

class MyCanvas:public wxShapeCanvas
{
public:
  double tmpx,tmpy;

  MyCanvas (wxWindow * parent = NULL, wxWindowID id = wxID_ANY,
	    const wxPoint & pos = wxDefaultPosition, const wxSize & size =
	    wxDefaultSize, long style = wxRETAINED);
  ~MyCanvas (void);

  void OnMouseEvent (wxMouseEvent & event);
  void OnPaint (wxPaintEvent & event);
  void OnKey (wxKeyEvent & event);
  void OnDClickEvent (wxMouseEvent & event);

  void DrawPic (wxDC * dc, const int & maxX, const int & maxY);
  int MyCanvas::GetCanvasWidth();
  int MyCanvas::GetCanvasHeight();
  
  virtual void OnLeftClick (double x, double y, int keys = 0);
  virtual void OnRightClick (double x, double y, int keys = 0);

  virtual void OnDragLeft (bool draw, double x, double y, int keys = 0);	// Erase if draw false
  virtual void OnBeginDragLeft (double x, double y, int keys = 0);
  virtual void OnEndDragLeft (double x, double y, int keys = 0);

  virtual void OnDragRight (bool draw, double x, double y, int keys = 0);	// Erase if draw false
  virtual void OnBeginDragRight (double x, double y, int keys = 0);
  virtual void OnEndDragRight (double x, double y, int keys = 0);

  void ShowMenu();

protected:
  wxMenu menu;

  DECLARE_EVENT_TABLE ()
};

#endif  // _FCSBUILDER_VIEW_H_
