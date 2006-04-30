/////////////////////////////////////////////////////////////////////////////
// Name:        doc.h
// Purpose:     Document classes
// Author:      Matthew Gong
// Created:     02/04/2005
// Copyright:   (c) Matthew Gong
// Licence:     GPL licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _FCSBUILDER_DOC_H_
#define _FCSBUILDER_DOC_H_

#if defined(__GNUG__) && !defined(__APPLE__)
// #pragma interface
#endif

#include <vector>

#include <wx/txtstrm.h>

#include "wx/deprecated/setup.h"

#if wxUSE_PROLOGIO
#include "wx/deprecated/wxexpr.h"
#endif

#include "wx/ogl/ogl.h"
#include "FGXMLElement.h"

/*
 * Override a few members for this application
 */

class MyDiagram:public wxDiagram
{
public:
#if wxUSE_PROLOGIO
  bool OnShapeSave (wxExprDatabase & db, wxShape & shape, wxExpr & expr);
  bool OnShapeLoad (wxExprDatabase & db, wxShape & shape, wxExpr & expr);
#endif
};

/*
 * A diagram document, which contains a series of diagram.
 */

class wxNotebook;

class AddToolBar;
class AdjToolBar;
class MyCanvas;

struct Channel{
  wxString name;
  MyCanvas * canvas;
  MyDiagram * diagram;
};

class DiagramDocument:public wxPanel
{
public:
  DiagramDocument(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
  ~DiagramDocument();
  void ClearAll();

  unsigned long unused;
  
  
  AddToolBar * addtb;
  AdjToolBar * adjtb;
  wxNotebook * notebook;
  std::vector<Channel> channels;
  wxArrayString prop_addition;
  wxString type;
  wxString name;

#if wxUSE_STD_IOSTREAM
  bool SaveObject(MyDiagram * diagram, const wxString & name);
  bool LoadObject(MyDiagram * diagram, const wxString & name);
#endif

  wxDiagram *GetActiveDiagram ();
  wxShape *FindSelectedShape (void);

  bool ExportXML(const wxString & filename);
  bool ImportXML(const wxString & filename);
  void Import(JSBSim::Element * el);
  void Export(wxTextOutputStream & tstream, const wxString & prefix);

//  void OnMenuCommand(int cmd);
  void OnDel (wxCommandEvent & event);
  void OnLineNewPoint (wxCommandEvent & event);
  void OnLineCutPoint (wxCommandEvent & event);
  void OnLineStraighten (wxCommandEvent & event);
  void OnShapeReverse (wxCommandEvent & event);
  void OnSetShapeProperty (wxCommandEvent & event);
  void OnExportXML (wxCommandEvent & event);
  void OnImportXML (wxCommandEvent & event);
  void OnGeneral (wxCommandEvent & event);
  void OnAddChannel (wxCommandEvent & event);
  void OnSnapChannel (wxCommandEvent & event);

  void SetProperty(wxShape * theShape);
  void RemoveLines (wxShape * shape);

  AdjToolBar *CreateAdjToolBar (wxWindow * parent);
  AddToolBar *CreateAddToolBar (wxWindow * parent);
  Channel CreateChannel (const wxString & channel_name);

  DECLARE_EVENT_TABLE ()  
};

// Menu/undo/redo commands
enum
  {
    ID_CANVAS = 1,
    FCSBUILDER_DEL = 100 ,
    FCSBUILDER_ADD_SHAPE ,
    FCSBUILDER_ADD_LINE ,
    FCSBUILDER_ADD_CHANNEL ,
    FCSBUILDER_SNAP_CHANNEL ,
    FCSBUILDER_CHANGE_BACKGROUND_COLOUR ,
    FCSBUILDER_EXPORT,
    FCSBUILDER_IMPORT,
    FCSBUILDER_GENERAL,

    FCSBUILDER_ABOUT = 200
  };

/*
 * Most user interface commands are routed through this, to give us the
 * Undo/Redo mechanism. If you add more commands, such as changing the shape colour,
 * you will need to add members to 'remember' what the user applied (for 'Do') and what the
 * previous state was (for 'Undo').
 * You can have one member for each property to be changed. Assume we also have
 * a pointer member wxShape *shape, which is set to the shape being changed.
 * Let's assume we're changing the shape colour. Our member for this is shapeColour.
 *
 * - In 'Do':
 *   o Set a temporary variable 'temp' to the current colour for 'shape'.
 *   o Change the colour to the new colour.
 *   o Set shapeColour to the _old_ colour, 'temp'.
 * - In 'Undo':
 *   o Set a temporary variable 'temp' to the current colour for 'shape'.
 *   o Change the colour to shapeColour (the old colour).
 *   o Set shapeColour to 'temp'.
 *
 * So, as long as we have a pointer to the shape being changed,
 * we only need one member variable for each property.
 *
 * PROBLEM: when an Add shape command is redone, the 'shape' pointer changes.
 * Assume, as here, that we keep a pointer to the old shape so we reuse it
 * when we recreate.

class DiagramCommand:public wxCommand
{
protected:
  DiagramDocument * doc;
  int cmd;
  wxShape *shape;		// Pointer to the shape we're acting on
  wxShape *fromShape;
  wxShape *toShape;
  int attachFrom;
  int attachTo;
  wxClassInfo *shapeInfo;
  double x;
  double y;
  bool selected;
  bool deleteShape;

  // Storage for property commands
  wxBrush *shapeBrush;
  wxPen *shapePen;
public:
  // Multi-purpose constructor for creating, deleting shapes
    DiagramCommand (const wxString & name, int cmd, DiagramDocument * ddoc,
		    wxClassInfo * shapeInfo = NULL, double x = 0.0, double y =
		    0.0, bool sel = false, wxShape * theShape =
		    NULL, wxShape * fs = NULL, wxShape * ts = NULL, int from =
		    0, int to = 0);

  // Property-changing command constructors
    DiagramCommand (const wxString & name, int cmd, DiagramDocument * ddoc,
		    wxBrush * backgroundColour, wxShape * theShape);

   ~DiagramCommand (void);

  bool Do (void);
  bool Undo (void);

  inline void SetShape (wxShape * s)
  {
    shape = s;
  }
  inline wxShape *GetShape (void)
  {
    return shape;
  }
  inline wxShape *GetFromShape (void)
  {
    return fromShape;
  }
  inline wxShape *GetToShape (void)
  {
    return toShape;
  }
  inline wxClassInfo *GetShapeInfo (void)
  {
    return shapeInfo;
  }
  inline bool GetSelected (void)
  {
    return selected;
  }

  void RemoveLines (wxShape * shape);
};

 */

#endif  // _FCSBUILDER_DOC_H_
