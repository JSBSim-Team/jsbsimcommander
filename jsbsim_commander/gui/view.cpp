/////////////////////////////////////////////////////////////////////////////
// Name:        view.cpp
// Purpose:     Implements view functionality in OGLEdit
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

#include "doc.h"
#include "view.h"
#include "toolbar.h"
#include "shape.h"
#include "sum.h"
#include "gain.h"
#include "filter.h"
#include "deadband.h"
#include "switch.h"
#include "kinemat.h"
#include "func.h"
  
/*
 * MyCanvas implementations
 */

BEGIN_EVENT_TABLE (MyCanvas, wxShapeCanvas)
  EVT_MOUSE_EVENTS (MyCanvas::OnMouseEvent)
  EVT_PAINT (MyCanvas::OnPaint) 
  EVT_KEY_DOWN  (MyCanvas::OnKey)
  EVT_LEFT_DCLICK(MyCanvas::OnDClickEvent) 
END_EVENT_TABLE ()


/*
 * Define a constructor for my canvas
 */

MyCanvas::MyCanvas (wxWindow * parent, wxWindowID id,
		      const wxPoint & pos, const wxSize & size, long style)
  :wxShapeCanvas (parent, id, pos, size, style)
{
  SetBackgroundColour (*wxWHITE);
  menu.Append (TOOLBAR_ADJ_PROPERTY, _("&Property"));
  menu.AppendSeparator ();
  menu.Append (TOOLBAR_ADJ_REVERSE_SHAPE, _("&Reverse"));
  menu.Append (FCSBUILDER_DEL, _("&Del"));
}


MyCanvas::~MyCanvas (void)
{
}


void
MyCanvas::OnLeftClick (double x, double y, int WXUNUSED (keys))
{
  tmpx = x;
  tmpy = y;
  AddToolBar *toolbar = (dynamic_cast<DiagramDocument *>(GetParent()->GetParent()))->addtb;
  wxClassInfo *info = NULL;
  switch (toolbar->currentlySelected)
    {
    case TOOLBAR_ADD_NODE:
      {
	info = CLASSINFO (SIMOShape);
	break;
      }
    case TOOLBAR_ADD_SOURCE:
      {
	info = CLASSINFO (SourceShape);
	break;
      }
    case TOOLBAR_ADD_DESTINATION:
      {
	info = CLASSINFO (DestinationShape);
	break;
      }
    case TOOLBAR_ADD_SUMMER:
      {
	info = CLASSINFO (Summer);
	break;
      }
    case TOOLBAR_ADD_GAIN:
      {
	info = CLASSINFO (Gain);
	break;
      }
    case TOOLBAR_ADD_FILTER:
      {
	info = CLASSINFO (Filter);
	break;
      }
    case TOOLBAR_ADD_DEADBAND:
      {
	info = CLASSINFO (DeadBand);
	break;
      }
    case TOOLBAR_ADD_SWITCH:
      {
	info = CLASSINFO (Switch);
	break;
      }
    case TOOLBAR_ADD_KINEMAT:
      {
	info = CLASSINFO (Kinemat);
	break;
      }
    case TOOLBAR_ADD_FUNC:
      {
	info = CLASSINFO (FCSFunction);
	break;
      }
    default:
      break;
    }
  if (info)
    {
      {
	      wxShape *theShape;
	      theShape = (wxShape *) info->CreateObject ();
	      theShape->AssignNewIds ();
	      theShape->SetEventHandler(new MyEvtHandler(theShape, theShape));
	      theShape->SetCentreResize (false);
	      theShape->SetPen (wxBLACK_PEN);
	      theShape->SetBrush (wxWHITE_BRUSH);
	      if (theShape->IsKindOf(CLASSINFO(SIMOShape)))
	        theShape->SetSize (10, 10);
	      else  
	        theShape->SetSize (50, 50);
	      GetDiagram ()->AddShape (theShape);
        if (theShape->IsKindOf(CLASSINFO(MISOShape)))
        {
          MISOShape * shape = (MISOShape *)theShape;
          shape->SetName(shape->GetName() + wxString::Format("_%ld", (dynamic_cast<DiagramDocument *>(GetParent()->GetParent()))->unused++));
        }
	      if (theShape->IsKindOf(CLASSINFO(ComponentShape)))
	         ((ComponentShape*)theShape)->SetOrder();

	      theShape->Show (true);

	      wxClientDC dc (theShape->GetCanvas ());
	      theShape->GetCanvas ()->PrepareDC (dc);
	      theShape->Move (dc, x, y);
      }
	    toolbar->ToggleTool(TOOLBAR_ARROW, true);
	    toolbar->OnLeftClick(TOOLBAR_ARROW, true);
    }
  else
  {
     wxShape * shape = (dynamic_cast<DiagramDocument *>(GetParent()->GetParent()))->FindSelectedShape();
     if (shape)
     {
       wxClientDC dc(this);
       PrepareDC(dc);
       shape->Select(false, &dc);
       Redraw(dc);
     }
  }
}


inline
void
MyCanvas::OnRightClick (double x, double y,
			int WXUNUSED (keys))
{
  tmpx = x;
  tmpy = y;
}


inline
void
MyCanvas::OnDragLeft (bool WXUNUSED (draw), double WXUNUSED (x),
		      double WXUNUSED (y), int WXUNUSED (keys))
{
}


inline
void
MyCanvas::OnBeginDragLeft (double WXUNUSED (x), double WXUNUSED (y),
			   int WXUNUSED (keys))
{
}


inline
void
MyCanvas::OnEndDragLeft (double WXUNUSED (x), double WXUNUSED (y),
			 int WXUNUSED (keys))
{
}


inline
void
MyCanvas::OnDragRight (bool WXUNUSED (draw), double WXUNUSED (x),
		       double WXUNUSED (y), int WXUNUSED (keys))
{
}


inline
void
MyCanvas::OnBeginDragRight (double WXUNUSED (x), double WXUNUSED (y),
			    int WXUNUSED (keys))
{
}


inline
void
MyCanvas::OnEndDragRight (double WXUNUSED (x), double WXUNUSED (y),
			  int WXUNUSED (keys))
{
}

inline 
void
MyCanvas::OnMouseEvent (wxMouseEvent & event)
{
  wxShapeCanvas::OnMouseEvent (event);
  event.Skip();
  if (FindFocus() != this)
  {
    SetFocus();
  }
}

void
MyCanvas::OnDClickEvent (wxMouseEvent &  WXUNUSED (event))
{
  int attachment;
  double distance;
  wxDiagram * diag = GetDiagram();
  wxObjectList::compatibility_iterator node = diag->GetShapeList ()->GetFirst ();
  while (node)
    {
      wxShape *eachShape = (wxShape *) node->GetData ();
      if (eachShape->GetParent() == NULL && eachShape->HitTest(tmpx,tmpy,&attachment,&distance) && distance < 50)
      {
        (dynamic_cast<DiagramDocument *>(GetParent()->GetParent()))->SetProperty(eachShape);
	      eachShape->Select(true);
	      return;
      }
      else
	      node = node->GetNext ();
    }
}

inline 
void
MyCanvas::OnPaint (wxPaintEvent & event)
{
  wxShapeCanvas::OnPaint (event);
}


void
MyCanvas::OnKey (wxKeyEvent & event)
{
  DiagramDocument *diagramDoc = (dynamic_cast<DiagramDocument *>(GetParent()->GetParent()));
  AddToolBar *toolbar = diagramDoc->addtb;
  switch (event.GetKeyCode())
  {
    case '1' :
      {
	toolbar->ToggleTool(TOOLBAR_ARROW, true);
	toolbar->OnLeftClick(TOOLBAR_ARROW, true);
      }
      break;
    case '2' :
      {
	toolbar->ToggleTool(TOOLBAR_ADD_SOURCE, true);
	toolbar->OnLeftClick(TOOLBAR_ADD_SOURCE, true);
      }
      break;
    case '3' :
      {
	toolbar->ToggleTool(TOOLBAR_ADD_DESTINATION, true);
	toolbar->OnLeftClick(TOOLBAR_ADD_DESTINATION, true);
      }
      break;
    case '4' :
      {
	toolbar->ToggleTool(TOOLBAR_ADD_SUMMER, true);
	toolbar->OnLeftClick(TOOLBAR_ADD_SUMMER, true);
      }
      break;
    case '5' :
      {
	toolbar->ToggleTool(TOOLBAR_ADD_GAIN, true);
	toolbar->OnLeftClick(TOOLBAR_ADD_GAIN, true);
      }
      break;
    case '6' :
      {
	toolbar->ToggleTool(TOOLBAR_ADD_FILTER, true);
	toolbar->OnLeftClick(TOOLBAR_ADD_FILTER, true);
      }
      break;
    case '7' :
      {
	toolbar->ToggleTool(TOOLBAR_ADD_DEADBAND, true);
	toolbar->OnLeftClick(TOOLBAR_ADD_DEADBAND, true);
      }
      break;
    case '8' :
      {
	toolbar->ToggleTool(TOOLBAR_ADD_SWITCH, true);
	toolbar->OnLeftClick(TOOLBAR_ADD_SWITCH, true);
      }
      break;
    case '9' :
      {
	toolbar->ToggleTool(TOOLBAR_ADD_KINEMAT, true);
	toolbar->OnLeftClick(TOOLBAR_ADD_KINEMAT, true);
      }
      break;
    case '0' :
      {
	toolbar->ToggleTool(TOOLBAR_ADD_FUNC, true);
	toolbar->OnLeftClick(TOOLBAR_ADD_FUNC, true);
      }
      break;
    case WXK_DELETE :
      {
	wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED,FCSBUILDER_DEL);
	wxPostEvent(GetParent()->GetParent(), e);
      }
      break;
    case 'P' :
    case 'p' :
    case WXK_RETURN :
      {
	wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED,TOOLBAR_ADJ_PROPERTY);
	wxPostEvent(GetParent()->GetParent(), e);
      }
      break;
    default :
      break;
  }
  event.Skip();
}

void
MyCanvas::DrawPic (wxDC * dc, const int & maxX, const int & maxY)
{
  // This part was added to preform the print preview and printing functions

  dc->BeginDrawing ();		// Allows optimization of drawing code under MS Windows.
  wxDiagram *diagram_p = GetDiagram ();	// Get the current diagram
  if (diagram_p->GetShapeList ())
    {
      /* wxCursor *old_cursor = NULL; */
      wxObjectList::compatibility_iterator current =
	diagram_p->GetShapeList ()->GetFirst ();

      while (current)		// Loop through the entire list of shapes
	{
	  wxShape *object = (wxShape *) current->GetData ();
	  if (!object->GetParent ())
	    {
	      object->Draw (*dc);	// Draw the shape onto our printing dc
	    }
	  current = current->GetNext ();	// Procede to the next shape in the list
	}
    }
  dc->EndDrawing ();		// Allows optimization of drawing code under MS Windows.
}

int
MyCanvas::GetCanvasWidth()
{
  int result = 0;

  wxDiagram *diagram_p = GetDiagram ();	// Get the current diagram
  if (diagram_p->GetShapeList ())
    {
      /* wxCursor *old_cursor = NULL; */
      wxObjectList::compatibility_iterator current =
	diagram_p->GetShapeList ()->GetFirst ();

      while (current)		// Loop through the entire list of shapes
	{
	  wxShape *object = (wxShape *) current->GetData ();
          if ( !object->IsKindOf(CLASSINFO(wxLineShape)) )
          {
            int tmp = (int)object->GetX();
            if ( tmp > result )
              result = tmp;
          }
	  current = current->GetNext ();	// Procede to the next shape in the list
	}
    }
  return result+100;
}

int
MyCanvas::GetCanvasHeight()
{
  int result = 0;

  wxDiagram *diagram_p = GetDiagram ();	// Get the current diagram
  if (diagram_p->GetShapeList ())
    {
      /* wxCursor *old_cursor = NULL; */
      wxObjectList::compatibility_iterator current =
	diagram_p->GetShapeList ()->GetFirst ();

      while (current)		// Loop through the entire list of shapes
	{
	  wxShape *object = (wxShape *) current->GetData ();
          if ( !object->IsKindOf(CLASSINFO(wxLineShape)) )
          {
            int tmp = (int)object->GetY();
            if ( tmp > result )
              result = tmp;
          }
	  current = current->GetNext ();	// Procede to the next shape in the list
	}
    }
  return result+100;
}


void
MyCanvas::ShowMenu()
{
  PopupMenu(&menu);
}

