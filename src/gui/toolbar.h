/////////////////////////////////////////////////////////////////////////////
// Name:        toolbar.h
// Purpose:     FCS Builder ToolBar
// Author:      Matthew Gong
// Created:     02/04/2005
// Copyright:   (c) Matthew Gong
// Licence:     GPL licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _FCSBUILDER_TOOLBAR_H_
#define _FCSBUILDER_TOOLBAR_H_

#if defined(__GNUG__) && !defined(__APPLE__)
// #pragma interface
#endif

#include <wx/wx.h>
#include <wx/string.h>
//#include <wx/deprecated/setup.h>
//#include <wx/deprecated/tbarsmpl.h>

#define TOOLPALETTECLASS    wxToolBar


/*
 * ToolBar to add shapes
 */

class AddToolBar:public TOOLPALETTECLASS
{
public:
  int currentlySelected;

  AddToolBar (wxWindow * parent, const wxPoint & pos = wxDefaultPosition,
	      const wxSize & size = wxDefaultSize,
	      long style = wxTB_HORIZONTAL | wxNO_BORDER);
  bool OnLeftClick (int toolIndex, bool toggled);
  void OnMouseEnter (int toolIndex);
  void SetSize (int x, int y, int width, int height,
		int sizeFlags = wxSIZE_AUTO);
};


/*
 * ToolBar to adjust shapes
 */

class AdjToolBar:public TOOLPALETTECLASS
{
public:
  AdjToolBar(wxWindow * parent, const wxPoint & pos = wxDefaultPosition,
	     const wxSize & size = wxDefaultSize,
	     long style = wxTB_HORIZONTAL | wxNO_BORDER)
    :TOOLPALETTECLASS (parent, wxID_ANY, pos, size, style)
  {
  }
};


enum
  {
    TOOLBAR_ADJ_PROPERTY =500,
    TOOLBAR_ADJ_REVERSE_SHAPE,
    TOOLBAR_ADJ_NEW_POINT,
    TOOLBAR_ADJ_DEL_POINT,
    TOOLBAR_ADJ_STRAIGHTEN_LINE,
    TOOLBAR_ARROW = 600,
    TOOLBAR_ADD_NODE,
    TOOLBAR_ADD_SOURCE,
    TOOLBAR_ADD_DESTINATION,
    TOOLBAR_ADD_SUMMER,
    TOOLBAR_ADD_GAIN,
    TOOLBAR_ADD_FILTER,
    TOOLBAR_ADD_DEADBAND,
    TOOLBAR_ADD_SWITCH,
    TOOLBAR_ADD_KINEMAT,
    TOOLBAR_ADD_FUNC,
  };

#endif // _FCSBUILDER_TOOLBAR_H_
