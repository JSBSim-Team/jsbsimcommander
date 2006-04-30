/////////////////////////////////////////////////////////////////////////////
// Name:        toolbar.cpp
// Purpose:     FCS Builder ToolBars
// Author:      Matthew Gong
// Created:     02/04/2005
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

//#include <wx/deprecated/setup.h>
//#include <wx/deprecated/resource.h>
#include <wx/config.h>
#include <wx/laywin.h>

#include <wx/toolbar.h>

#include <ctype.h>
#include <math.h>

#include "doc.h"
#include "toolbar.h"

// Include pixmaps
#ifndef __WXMSW__
#include "arrow.xpm"
#include "source.xpm"
#include "destination.xpm"
#include "summer.xpm"
#include "gain.xpm"
#include "filter.xpm"
#include "deadband.xpm"
#include "switch.xpm"
#include "kinemat.xpm"
#include "node.xpm"
#include "func.xpm"

#include "general.xpm"
#include "channel.xpm"

#include "property.xpm"
#include "reverse.xpm"
#include "newpoint.xpm"
#include "cutpoint.xpm"
#include "straight.xpm"
#include "snap.xpm"
#endif

/*
 * ToolBar to add shapes
 */

AddToolBar::AddToolBar (wxWindow * parent, const wxPoint & pos, const wxSize & size, long style):
TOOLPALETTECLASS (parent, wxID_ANY, pos, size, style)
{
  currentlySelected = -1;
}

bool
AddToolBar::OnLeftClick (int toolIndex, bool toggled)
{
  // BEGIN mutual exclusivity code
  if (toggled && (currentlySelected != -1)
      && (toolIndex != currentlySelected))
    ToggleTool (currentlySelected, false);

  if (toggled)
    currentlySelected = toolIndex;
  else if (currentlySelected == toolIndex)
    currentlySelected = -1;
  //  END mutual exclusivity code

  return true;
}

inline
void
AddToolBar::OnMouseEnter (int WXUNUSED (toolIndex))
{
}

inline
void
AddToolBar::SetSize (int x, int y, int width, int height,
			    int sizeFlags)
{
  TOOLPALETTECLASS::SetSize (x, y, width, height, sizeFlags);
}


/*
 * Create and initialise the 'add' toolbar
 */

AddToolBar *
DiagramDocument::CreateAddToolBar (wxWindow * parent)
{
  wxBitmap PaletteNode (wxBITMAP(node));
  wxBitmap PaletteSource (wxBITMAP(source));
  wxBitmap PaletteDestination (wxBITMAP(destination));
  wxBitmap PaletteSummer (wxBITMAP(summer));
  wxBitmap PaletteGain (wxBITMAP(gain));
  wxBitmap PaletteFilter (wxBITMAP(filter));
  wxBitmap PaletteDeadBand (wxBITMAP(deadband));
  wxBitmap PaletteKinemat (wxBITMAP(kinemat));
  wxBitmap PaletteSwitch(wxBITMAP(switch));
  wxBitmap PaletteArrow (wxBITMAP(arrow));
  wxBitmap PaletteFunc (wxBITMAP(func));

  AddToolBar *toolbar =
    new AddToolBar (parent, wxDefaultPosition, wxDefaultSize);

  toolbar->SetMargins (5, 5);
  toolbar->SetToolBitmapSize (wxSize (32, 32));

  toolbar->AddTool (TOOLBAR_ARROW, _("Pointer"), PaletteArrow, wxNullBitmap,
		    wxITEM_CHECK, _("Pointer"), _("Pointer"), NULL);

  toolbar->AddSeparator();
  toolbar->AddTool (TOOLBAR_ADD_SOURCE, _("Source"), PaletteSource,
		    wxNullBitmap, wxITEM_CHECK, _("Source"), _("Source"),
		    NULL);
  toolbar->AddTool (TOOLBAR_ADD_DESTINATION, _("Destination"),
		    PaletteDestination, wxNullBitmap, wxITEM_CHECK,
		    _("Destination"), _("Destination"), NULL);

  toolbar->AddSeparator();
  toolbar->AddTool (TOOLBAR_ADD_SUMMER, _("Summer"), PaletteSummer,
		    wxNullBitmap, wxITEM_CHECK, _("Summer"), _("Summer"),
		    NULL);
  toolbar->AddTool (TOOLBAR_ADD_GAIN, _("Gain"), PaletteGain,
		    wxNullBitmap, wxITEM_CHECK, _("Gain"), _("Gain"),
		    NULL);
  toolbar->AddTool (TOOLBAR_ADD_FILTER, _("Filter"), PaletteFilter,
		    wxNullBitmap, wxITEM_CHECK, _("Filter"), _("Filter"),
		    NULL);
  toolbar->AddTool (TOOLBAR_ADD_DEADBAND, _("DeadBand"), PaletteDeadBand,
		    wxNullBitmap, wxITEM_CHECK, _("Dead Band"), _("Dead Band"), NULL);
  toolbar->AddTool (TOOLBAR_ADD_SWITCH, _("Switch"), PaletteSwitch,
		    wxNullBitmap, wxITEM_CHECK, _("Switch"), _("Switch"), NULL);
  toolbar->AddTool (TOOLBAR_ADD_KINEMAT, _("Kinemat"), PaletteKinemat,
		    wxNullBitmap, wxITEM_CHECK, _("Kinemat"), _("Kinemat"),
		    NULL);
  toolbar->AddTool (TOOLBAR_ADD_FUNC, _("FCSFunction"), PaletteFunc,
		    wxNullBitmap, wxITEM_CHECK, _("FCSFunction"), _("FCSFunction"),
		    NULL);

  toolbar->AddSeparator();
  toolbar->AddTool (TOOLBAR_ADD_NODE, _("node"), PaletteNode,
		    wxNullBitmap, wxITEM_CHECK, _("node"), _("node"),
		    NULL);

  toolbar->Realize ();

  toolbar->ToggleTool (TOOLBAR_ARROW, true);
  toolbar->currentlySelected = TOOLBAR_ARROW;

  return toolbar;
}


/*
 * Create and initialise the 'adj' toolbar
 */

AdjToolBar *
DiagramDocument::CreateAdjToolBar (wxWindow * parent)
{
  // Create the actual toolbar
  AdjToolBar *m_diagramToolBar =
    new AdjToolBar (parent, wxDefaultPosition, wxDefaultSize);

  wxBitmap *bitmaps[5];

  bitmaps[0] = new wxBitmap (wxBITMAP(property));
  bitmaps[1] = new wxBitmap (wxBITMAP(reverse));
  bitmaps[2] = new wxBitmap (wxBITMAP(newpoint));
  bitmaps[3] = new wxBitmap (wxBITMAP(cutpoint));
  bitmaps[4] = new wxBitmap (wxBITMAP(straight));

  m_diagramToolBar->AddTool (FCSBUILDER_GENERAL, _("General Property"), wxBITMAP(general), wxNullBitmap, wxITEM_NORMAL, _("General Property"), _("General Property"), NULL);

  m_diagramToolBar->AddTool (FCSBUILDER_ADD_CHANNEL, _("Add Channel"), wxBITMAP(channel), wxNullBitmap, wxITEM_NORMAL, _("Add Channel"), _("Add Channel"), NULL);
  m_diagramToolBar->AddTool (FCSBUILDER_SNAP_CHANNEL, _("Snap Channel"), wxBITMAP(snap), wxNullBitmap, wxITEM_NORMAL, _("Snap Channel"), _("Snap Channel"), NULL);
  m_diagramToolBar->AddSeparator();


  m_diagramToolBar->AddTool(TOOLBAR_ADJ_PROPERTY, *bitmaps[0], wxNullBitmap, false, wxDefaultCoord, wxDefaultCoord, NULL, _("Set the property of the Shape"));
  m_diagramToolBar->AddSeparator();

  m_diagramToolBar->AddTool(TOOLBAR_ADJ_REVERSE_SHAPE, *bitmaps[1], wxNullBitmap, false, wxDefaultCoord, wxDefaultCoord, NULL, _("Reverse Shape"));
  m_diagramToolBar->AddSeparator();

  m_diagramToolBar->AddTool (TOOLBAR_ADJ_NEW_POINT, *bitmaps[2],
			     wxNullBitmap, false, wxDefaultCoord,
			     wxDefaultCoord, NULL, _("New line point"));
  m_diagramToolBar->AddTool (TOOLBAR_ADJ_DEL_POINT, *bitmaps[3],
			     wxNullBitmap, false, wxDefaultCoord,
			     wxDefaultCoord, NULL, _("Cut line point"));
  m_diagramToolBar->AddTool (TOOLBAR_ADJ_STRAIGHTEN_LINE, *bitmaps[4],
			     wxNullBitmap, false, wxDefaultCoord,
			     wxDefaultCoord, NULL, _("Straighten lines"));

  m_diagramToolBar->Realize ();

  int i;
  for (i = 0; i < 5; ++i)
    delete bitmaps[i];

  return m_diagramToolBar;
}
