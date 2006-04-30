/////////////////////////////////////////////////////////////////////////////
// Name:        shape.cpp
// Purpose:     Implements shape functionality in FCSBuilder
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
#include "shape.h"


#include <wx/listimpl.cpp>
WX_DEFINE_LIST(MyBoolList);


/*
 * MyEvtHandler: an event handler class for all shapes
 */

void MyEvtHandler::OnLeftClick(double x, double y, int keys, int WXUNUSED(attachment))
{
  MyCanvas * canvas = (MyCanvas *)GetShape()->GetCanvas();
  canvas->tmpx = x;
  canvas->tmpy = y;
  wxClientDC dc(canvas);
  canvas->PrepareDC(dc);

  if (keys == 0)
  {
    // Selection is a concept the library knows about
    if (GetShape()->Selected())
    {
      GetShape()->Select(false, &dc);
      GetShape()->GetCanvas()->Redraw(dc); // Redraw because bits of objects will be are missing
    }
    else
    {
      // Ensure no other shape is selected, to simplify Undo/Redo code
      bool redraw = false;
      wxObjectList::compatibility_iterator node = GetShape()->GetCanvas()->GetDiagram()->GetShapeList()->GetFirst();
      while (node)
      {
        wxShape *eachShape = (wxShape *)node->GetData();
        if (eachShape->GetParent() == NULL)
        {
          if (eachShape->Selected())
          {
            eachShape->Select(false, &dc);
            redraw = true;
          }
        }
        node = node->GetNext();
      }
      GetShape()->Select(true, &dc);
      if (redraw)
        GetShape()->GetCanvas()->Redraw(dc);
    }
  }
  else if (keys & KEY_CTRL)
  {
    // Do something for CONTROL
  }
  else
  {
#if wxUSE_STATUSBAR
    //wxGetApp().frame->SetStatusText(wxT("to do"));
#endif // wxUSE_STATUSBAR
  }
}


void MyEvtHandler::OnRightClick(double x, double y, int keys, int WXUNUSED(attachment))
{
  MyCanvas * canvas = (MyCanvas *)GetShape()->GetCanvas();
  canvas->tmpx = x;
  canvas->tmpy = y;
  if (!GetShape()->Selected())
  {
      // Ensure no other shape is selected, to simplify Undo/Redo code
      wxObjectList::compatibility_iterator node = GetShape()->GetCanvas()->GetDiagram()->GetShapeList()->GetFirst();
      while (node)
      {
        wxShape *eachShape = (wxShape *)node->GetData();
        if (eachShape->GetParent() == NULL && eachShape->Selected())
        {
            eachShape->Select(false);
        }
        node = node->GetNext();
      }
      GetShape()->Select(true);
  }
    
  ((MyCanvas *)GetShape()->GetCanvas())->ShowMenu();
}

/*
 * Implement connection of two shapes by right-dragging between them.
 */

void
MyEvtHandler::OnBeginDragRight (double x, double y, int WXUNUSED (keys),
				int attachment)
{

  wxClientDC dc (GetShape()->GetCanvas ());
  GetShape()->GetCanvas ()->PrepareDC (dc);

  if (GetShape()->IsKindOf (CLASSINFO (MISOShape)))
    {
      //output port should be attachment 0
      if (attachment > 0)
	return;
    }

  wxPen dottedPen (wxColour (0, 0, 0), 1, wxDOT);
  dc.SetLogicalFunction (OGLRBLF);
  dc.SetPen (dottedPen);
  double xp, yp;
  GetShape()->GetAttachmentPosition (attachment, &xp, &yp);
  dc.DrawLine ((long) xp, (long) yp, (long) x, (long) y);
  GetShape()->GetCanvas ()->CaptureMouse ();
}


void
MyEvtHandler::OnDragRight (bool WXUNUSED (draw), double x, double y,
			   int WXUNUSED (keys), int attachment)
{
  wxClientDC dc (GetShape()->GetCanvas ());
  GetShape()->GetCanvas ()->PrepareDC (dc);

  if (GetShape()->IsKindOf (CLASSINFO (MISOShape)))
    {
      //output port should be attachment 0
      if (attachment > 0)
	return;
    }

  wxPen dottedPen (wxColour (0, 0, 0), 1, wxDOT);
  dc.SetLogicalFunction (OGLRBLF);
  dc.SetPen (dottedPen);
  double xp, yp;
  GetShape()->GetAttachmentPosition (attachment, &xp, &yp);
  dc.DrawLine ((long) xp, (long) yp, (long) x, (long) y);
}


void
MyEvtHandler::OnEndDragRight (double x, double y, int WXUNUSED (keys),
			      int attachment)
{
  GetShape()->GetCanvas ()->ReleaseMouse ();
  MyCanvas *canvas = (MyCanvas *) GetShape()->GetCanvas ();

  // Check if we're on an object
  int new_attachment;
  wxShape *otherShape =
    canvas->FindFirstSensitiveShape (x, y, &new_attachment, OP_DRAG_RIGHT);

  if (otherShape && !otherShape->IsKindOf (CLASSINFO (wxLineShape)))
    {
      if (GetShape()->IsKindOf (CLASSINFO (MISOShape)))
	{
	  //output port should be attachment 0
	  if (attachment > 0)
	    return;
	  // do not link to myself
	  if (otherShape == GetShape())
	    return;

	  wxNode *node = GetShape()->GetLines ().GetFirst ();
	  while (node)
	    {
	      wxLineShape *p = (wxLineShape *) node->GetData ();
	      //only have one output line
	      if (p->GetFrom () == GetShape())
		return;
	      node = node->GetNext ();
	    }
	}

      if (otherShape->IsKindOf (CLASSINFO (MISOShape)))
	{
	  //input port should not be attachment 0
	  if (new_attachment == 0)
	    return;

	  wxNode *node = otherShape->GetLines ().GetFirst ();
	  while (node)
	    {
	      wxLineShape *p = (wxLineShape *) node->GetData ();
	      //one input port, one input line
	      if (p->GetAttachmentTo () == new_attachment
		  && p->GetTo () == otherShape)
		return;
	      node = node->GetNext ();
	    }

	}
      if (otherShape->IsKindOf(CLASSINFO(SIMOShape)))
	{
	  wxNode *node = otherShape->GetLines ().GetFirst ();
	  while (node)
	    {
	      wxLineShape *p = (wxLineShape *) node->GetData ();
	      if (p->GetTo () == otherShape)
		return;		//otherShape has already had a input line
	      node = node->GetNext ();
	    }
	}
      
  {
      wxLineShape * theShape;
	    theShape = new wxLineShape;
	    theShape->AssignNewIds ();
	    theShape->SetEventHandler(new MyEvtHandler(theShape, theShape));
	    theShape->SetPen (wxBLACK_PEN);
	    theShape->SetBrush (wxRED_BRUSH);

	    wxLineShape *lineShape = theShape;

	    // Yes, you can have more than 2 control points, in which case
	    // it becomes a multi-segment line.
	    lineShape->MakeLineControlPoints (3);
	    lineShape->AddArrow (ARROW_ARROW, ARROW_POSITION_END, 10.0, 0.0,
				 _T ("Normal arrowhead"));

	    canvas->GetDiagram ()->AddShape (theShape);

	    GetShape()->AddLine ((wxLineShape *) theShape, otherShape, attachment, new_attachment);
      
	    if (GetShape()->IsKindOf (CLASSINFO (MISOShape)))
      {
        ((MISOShape *) GetShape())->NormalizeLine ((wxLineShape *) theShape);
      }

	    theShape->Show (true);

	    wxClientDC dc (canvas);
	    canvas->PrepareDC (dc);

	    // It won't get drawn properly unless you move both
	    // connected images
	    GetShape()->Move (dc, GetShape()->GetX (), GetShape()->GetY ());
	    otherShape->Move (dc, otherShape->GetX (), otherShape->GetY ());
  }
    }
}


/*
 * Single Input Multi Output Node
 */

IMPLEMENT_DYNAMIC_CLASS (SIMOShape, wxEllipseShape)

SIMOShape::SIMOShape (double w, double h):
wxEllipseShape (w, h)
{
  
}


/*
 * Multi Input Single Output, the base class for all components
 */

IMPLEMENT_DYNAMIC_CLASS (MISOShape, wxRectangleShape)


MISOShape::MISOShape (double w, double h, const wxString & Type, const wxString & Name):
wxRectangleShape (w, h),
is_draw_reverse (false),
arr_size (5u),
type(Type),
name(Name)
{
}

void
MISOShape::SetNodePos ()
{
  double width = m_width * (is_draw_reverse ? -0.5 : 0.5);
  double height =
    m_height /
    (((wxAttachmentPoint *) GetAttachments ().GetLast ()->GetData ())->m_id +
     1);

  wxAttachmentPoint *tmp;
  wxNode *node = GetAttachments ().GetFirst ();

  while (node)
    {
      tmp = (wxAttachmentPoint *) node->GetData ();
      if (tmp->m_id > 0)
	{
	  tmp->m_x = -width;
	  tmp->m_y = -0.5 * m_height + height * tmp->m_id;
	}
      else
	{
	  tmp->m_x = width;
	  tmp->m_y = 0.0;
	}
      node = node->GetNext ();
    }

}

void
MISOShape::ShowType(wxDC & dc)
{
  wxCoord w,h;
  dc.GetTextExtent(type,&w,&h);
  int ps = type.Find('_');
  if (ps == -1)
    {
      dc.DrawText(type,WXROUND (GetX()-w*0.5),WXROUND (GetY()-h*0.5));
    }
  else
    {
      wxString str1 = type.BeforeLast('_');
      wxString str2 = type.AfterLast('_');
      wxCoord w1,h1,w2,h2;
      dc.GetTextExtent(str1,&w1,&h1);
      dc.GetTextExtent(str2,&w2,&h2);
      w = w1>=w2?w1:w2;
      h = h1+h2+4;
      dc.DrawText(str1,WXROUND (GetX()-w1*0.5),WXROUND (GetY()-h1-2));
      dc.DrawText(str2,WXROUND (GetX()-w2*0.5),WXROUND (GetY()+2));      
    }

  bool flag = false;
  if (w+12>m_width)
    {
      m_width = w+12;
      flag = true;
    }
  if (h+4>m_height)
    {
      m_height = h+4;
      flag = true;
    }
  if (flag)
    {
      SetSize(m_width, m_height);
      Select(false);
    }
}

void
MISOShape::OnDraw (wxDC & dc)
{
  wxRectangleShape::OnDraw (dc);

  wxPoint tmp[3];
  wxAttachmentPoint *p;
  wxNode *node = GetAttachments ().GetFirst ();

  wxCoord w,h;

  dc.GetTextExtent(name,&w,&h);
  dc.DrawText(name,WXROUND (GetX()-w*0.5),WXROUND (GetY()+m_height*0.5+5));
  
  while (node)
    {
      p = (wxAttachmentPoint *) node->GetData ();
      if (p->m_id > 0)
	{
	  if (is_draw_reverse)
	    {
	      tmp[0].x = WXROUND (p->m_x);
	      tmp[0].y = WXROUND (p->m_y) - arr_size;
	      tmp[1].x = WXROUND (p->m_x) - arr_size;
	      tmp[1].y = WXROUND (p->m_y);
	      tmp[2].x = WXROUND (p->m_x);
	      tmp[2].y = WXROUND (p->m_y) + arr_size;
	    }
	  else
	    {
	      tmp[0].x = WXROUND (p->m_x);
	      tmp[0].y = WXROUND (p->m_y) - arr_size;
	      tmp[1].x = WXROUND (p->m_x) + arr_size;
	      tmp[1].y = WXROUND (p->m_y);
	      tmp[2].x = WXROUND (p->m_x);
	      tmp[2].y = WXROUND (p->m_y) + arr_size;
	    }
	}
      else
	{
	  tmp[1].x = WXROUND (p->m_x);
	  tmp[1].y = WXROUND (p->m_y);
	  if (is_draw_reverse)
	    {
	      tmp[0].x = tmp[1].x + arr_size;
	      tmp[0].y = tmp[1].y - arr_size;
	      tmp[2].x = tmp[1].x + arr_size;
	      tmp[2].y = tmp[1].y + arr_size;
	    }
	  else
	    {
	      tmp[0].x = tmp[1].x - arr_size;
	      tmp[0].y = tmp[1].y - arr_size;
	      tmp[2].x = tmp[1].x - arr_size;
	      tmp[2].y = tmp[1].y + arr_size;
	    }
	}
      dc.DrawLines (3, tmp, WXROUND (m_xpos), WXROUND (m_ypos));
      node = node->GetNext ();
    }
}

void
MISOShape::SetSize (double x, double y, bool WXUNUSED (recursive))
{
  m_width = (double) wxMax (x, 1.0);
  m_height = (double) wxMax (y, 1.0);
  SetDefaultRegionSize ();
  SetAttachmentSize (x, y);
}

void
MISOShape::SetAttachmentSize (double WXUNUSED (w), double WXUNUSED (h))
{
  SetNodePos ();
}

#if wxUSE_PROLOGIO
void
MISOShape::WriteAttributes (wxExpr * clause)
{
  wxRectangleShape::WriteAttributes (clause);

  if (is_draw_reverse)
    clause->AddAttributeValue (wxT ("is_draw_reverse"), 1L);
  else
    clause->AddAttributeValue (wxT ("is_draw_reverse"), 0L);

  clause->AddAttributeValueString (wxT ("property node name"), name);
  clause->AddAttributeValueString (wxT ("property node type"), type);
  
}

void
MISOShape::ReadAttributes (wxExpr * clause)
{
  ClearAttachments ();
  wxRectangleShape::ReadAttributes (clause);

  long int tmp;

  clause->GetAttributeValue(wxT ("is_draw_reverse"), tmp);
  if (tmp == 0L)
    is_draw_reverse = false;
  else
    is_draw_reverse = true;

  clause->GetAttributeValue (wxT ("property node name"), name);
  clause->GetAttributeValue (wxT ("property node type"), type);

  SetNodePos ();
}
#endif

void
MISOShape::Copy (wxShape & copy)
{
  wxRectangleShape::Copy (copy);

  wxASSERT (copy.IsKindOf (CLASSINFO (MISOShape)));

  MISOShape & MISOCopy = (MISOShape &) copy;
  MISOCopy.is_draw_reverse = is_draw_reverse;
  MISOCopy.arr_size = arr_size;
  MISOCopy.name = name;
  MISOCopy.type = type;
}

inline int
MISOShape::GetNumberOfAttachments () const
{
  return wxRectangleShape::GetNumberOfAttachments ();
}


inline bool
MISOShape::GetAttachmentPosition (int attachment, double *x, double *y,
				    int nth, int no_arcs, wxLineShape * line)
{
  return wxRectangleShape::GetAttachmentPosition (attachment, x, y, nth,
						  no_arcs, line);
}

void
MISOShape::OnChangeAttachment (int attachment, wxLineShape * line,
				 wxList & ordering)
{
  //check if it should be changed.
  if (line->GetTo () == this && attachment == 0 || line->GetFrom () == this
      && attachment > 0)
    {
      wxClientDC dc (GetCanvas ());
      GetCanvas ()->PrepareDC (dc);

      MoveLinks (dc);

      if (!GetCanvas ()->GetQuickEditMode ())
	GetCanvas ()->Redraw (dc);

      return;
    }

  wxRectangleShape::OnChangeAttachment (attachment, line, ordering);
}

void
MISOShape::NormalizeLine (wxLineShape * line)
{
  double fromx, fromy, tox, toy;
  line->FindLineEndPoints (&fromx, &fromy, &tox, &toy);
  double attach_to = line->GetAttachmentTo();
  double center_to = line->GetTo()->GetAttachments().GetCount() *0.5;
  //mirror
  if (attach_to > center_to)
    attach_to = center_to + center_to - attach_to;
  //where to be based on
  center_to = (center_to+1)*0.5;
  //modify line pos
  double step_size = 5.0;

  if (line->Selected ())
    line->Select (false);

  bool from_reverse = false, to_reverse = false;
  if (line->GetFrom ()->IsKindOf (CLASSINFO (MISOShape))
      && ((MISOShape *) line->GetFrom ())->IsDrawReverse ())
    from_reverse = true;
  if (line->GetTo ()->IsKindOf (CLASSINFO (MISOShape))
      && ((MISOShape *) line->GetTo ())->IsDrawReverse ())
    to_reverse = true;

  if (!from_reverse && !to_reverse && fromx < tox || from_reverse
      && to_reverse && fromx > tox)
    {
      if (to_reverse)
	step_size *= -1;
      double coeff = (fromx + tox) * 0.5;
      // modify coeff by attachment point pos
      coeff -= step_size * ( attach_to - center_to );
      if (line->GetLineControlPoints ()->GetCount () == 4)
	{
	  double c =
	    ((wxRealPoint *) line->GetLineControlPoints ()->Item (1)->
	     GetData ())->x;
	  if (fromx < tox && c < tox - 10 && c > fromx + 10 || fromx > tox
	      && c > tox + 10 && c < fromx - 10)
	    coeff = c;
	}
      line->MakeLineControlPoints (4);
      wxNode *node = line->GetLineControlPoints ()->GetFirst ();
      wxRealPoint *p;
      p = (wxRealPoint *) node->GetData ();
      p->x = fromx;
      p->y = fromy;
      node = node->GetNext ();
      p = (wxRealPoint *) node->GetData ();
      p->x = coeff;
      p->y = fromy;
      node = node->GetNext ();
      p = (wxRealPoint *) node->GetData ();
      p->x = coeff;
      p->y = toy;
      node = node->GetNext ();
      p = (wxRealPoint *) node->GetData ();
      p->x = tox;
      p->y = toy;
    }
  else if (!from_reverse && to_reverse && fromx < tox || from_reverse
	   && !to_reverse && fromx > tox)
    {
      line->MakeLineControlPoints (4);
      wxNode *node = line->GetLineControlPoints ()->GetFirst ();
      wxRealPoint *p;
      p = (wxRealPoint *) node->GetData ();
      p->x = fromx;
      p->y = fromy;
      node = node->GetNext ();
      p = (wxRealPoint *) node->GetData ();
      p->x = tox - 20 * (to_reverse ? -1 : 1);
      p->y = fromy;
      node = node->GetNext ();
      p = (wxRealPoint *) node->GetData ();
      p->x = tox - 20 * (to_reverse ? -1 : 1);
      p->y = toy;
      node = node->GetNext ();
      p = (wxRealPoint *) node->GetData ();
      p->x = tox;
      p->y = toy;
    }
  else if (!from_reverse && to_reverse && fromx > tox || from_reverse
	   && !to_reverse && fromx < tox)
    {
      line->MakeLineControlPoints (4);
      wxNode *node = line->GetLineControlPoints ()->GetFirst ();
      wxRealPoint *p;
      p = (wxRealPoint *) node->GetData ();
      p->x = fromx;
      p->y = fromy;
      node = node->GetNext ();
      p = (wxRealPoint *) node->GetData ();
      p->x = fromx - 20 * (from_reverse ? 1 : -1);
      p->y = fromy;
      node = node->GetNext ();
      p = (wxRealPoint *) node->GetData ();
      p->x = fromx - 20 * (from_reverse ? 1 : -1);
      p->y = toy;
      node = node->GetNext ();
      p = (wxRealPoint *) node->GetData ();
      p->x = tox;
      p->y = toy;
    }
  else if (from_reverse && to_reverse && fromx < tox || !from_reverse
	   && !to_reverse && fromx > tox)
    {
      line->MakeLineControlPoints (6);
      wxNode *node = line->GetLineControlPoints ()->GetFirst ();
      wxRealPoint *p;
      p = (wxRealPoint *) node->GetData ();
      p->x = fromx;
      p->y = fromy;
      node = node->GetNext ();
      p = (wxRealPoint *) node->GetData ();
      p->x = fromx - 20 * (from_reverse ? 1 : -1);
      p->y = fromy;
      node = node->GetNext ();
      p = (wxRealPoint *) node->GetData ();
      p->x = fromx - 20 * (from_reverse ? 1 : -1);
      p->y = (fromy > toy ? fromy : toy) + 60;
      node = node->GetNext ();
      p = (wxRealPoint *) node->GetData ();
      p->x = tox - 20 * (to_reverse ? -1 : 1);
      p->y = (fromy > toy ? fromy : toy) + 60;
      node = node->GetNext ();
      p = (wxRealPoint *) node->GetData ();
      p->x = tox - 20 * (to_reverse ? -1 : 1);
      p->y = toy;
      node = node->GetNext ();
      p = (wxRealPoint *) node->GetData ();
      p->x = tox;
      p->y = toy;
    }

}


void
MISOShape::NormalizeLines ()
{
  wxNode *node = GetLines ().GetFirst ();

  /*****
  while (node)
  {
    wxLineShape * l = (wxLineShape *)node->GetData();
    if (l->GetFrom() == this )
    {
      wxNode * np = l->GetLineControlPoints()->GetFirst();
      wxRealPoint * p0 = (wxRealPoint *)np->GetData();
      np = np->GetNext();
      wxRealPoint * p1 = (wxRealPoint *)np->GetData();
      p1->y = p0->y;
      p1->x = p0->x-20*(is_draw_reverse?1:-1);
    }
    else
    {
      wxNode * np = l->GetLineControlPoints()->GetLast();
      wxRealPoint * p0 = (wxRealPoint *)np->GetData();
      np = np->GetPrevious();
      wxRealPoint * p1 = (wxRealPoint *)np->GetData();
      p1->y = p0->y;
      p1->x = p0->x+20*(is_draw_reverse?1:-1);
    }
    l->Straighten();
    node = node->GetNext();
  }
  ********/
  while (node)
    {
      wxLineShape *l = (wxLineShape *) node->GetData ();
      NormalizeLine (l);
      node = node->GetNext ();
    }
}


void
MISOShape::OnEndSize (double WXUNUSED (x), double WXUNUSED (y))
{
  wxClientDC dc (GetCanvas ());
  GetCanvas ()->PrepareDC (dc);

  FormatText (dc, /* (char*) (const char*) */ type);
}


void
MISOShape::OnMovePost (wxDC & dc, double x, double y, double old_x,
			 double old_y, bool display)
{
  wxRectangleShape::OnMovePost (dc, x, y, old_x, old_y, display);

  NormalizeLines ();

  GetCanvas ()->Refresh ();
}


IMPLEMENT_DYNAMIC_CLASS (SourceShape, MISOShape)
  
SourceShape::SourceShape (double w, double h, const wxString & Name):
  MISOShape (w, h, wxT("Source"), Name)
{
  SetAttachmentMode (ATTACHMENT_MODE_EDGE);

  ClearAttachments ();
  GetAttachments ().Append (new wxAttachmentPoint (0, w * 0.5, 0.0));
  SetCornerRadius (10);
}

void 
SourceShape::OnDraw (wxDC & dc)
{
  MISOShape::OnDraw (dc);

  wxPoint c[7];  
  wxCoord  h,w;
  h = ((wxCoord )m_height - 4*arr_size)/2;
  w = ((wxCoord )m_width - 4*arr_size)/2;
  
  c[0].x = -w;
  c[0].y = h;
  c[1].x = 0;
  c[1].y = c[0].y;
  c[2].x = c[1].x;
  c[2].y = -c[0].y;
  c[3].x = c[0].x;
  c[3].y = -c[0].y;
  dc.DrawLines (4, c, WXROUND (m_xpos), WXROUND (m_ypos));
  c[0].x = 0;
  c[0].y = h/2;
  c[1].x = w/2;
  c[1].y = c[0].y;
  c[2].x = c[1].x;
  c[2].y = 2*c[0].y;
  c[3].x = 2*c[1].x;
  c[3].y = 0;
  c[4].x = c[1].x;
  c[4].y = -c[2].y;
  c[5].x = c[1].x;
  c[5].y = -c[1].y;
  c[6].x = c[0].x;
  c[6].y = -c[0].y;
  wxPen old = dc.GetPen();
  dc.SetPen(wxPen(*wxBLUE));
  dc.DrawLines (7, c, WXROUND (m_xpos), WXROUND (m_ypos));
  dc.SetPen(old);
}

IMPLEMENT_DYNAMIC_CLASS (DestinationShape, MISOShape)

DestinationShape::DestinationShape (double w, double h, const wxString & Name):
  MISOShape (w, h, wxT("Dest"), Name)
{
  SetAttachmentMode (ATTACHMENT_MODE_EDGE);

  ClearAttachments ();
  GetAttachments ().Append (new wxAttachmentPoint (1, -w * 0.5, 0.0));
  SetCornerRadius (10);
}

void 
DestinationShape::OnDraw (wxDC & dc)
{
  MISOShape::OnDraw (dc);

  wxPoint c[7];  
  wxCoord  h,w;
  h = ((wxCoord )m_height - 4*arr_size)/2;
  w = ((wxCoord )m_width - 4*arr_size)/2;
  
  c[0].x = w;
  c[0].y = h;
  c[1].x = 0;
  c[1].y = c[0].y;
  c[2].x = c[1].x;
  c[2].y = -c[0].y;
  c[3].x = c[0].x;
  c[3].y = -c[0].y;
  dc.DrawLines (4, c, WXROUND (m_xpos), WXROUND (m_ypos));
  c[0].x = 0;
  c[0].y = h/2;
  c[1].x = w/2;
  c[1].y = c[0].y;
  c[2].x = c[1].x;
  c[2].y = 2*c[0].y;
  c[3].x = 2*c[1].x;
  c[3].y = 0;
  c[4].x = c[1].x;
  c[4].y = -c[2].y;
  c[5].x = c[1].x;
  c[5].y = -c[1].y;
  c[6].x = c[0].x;
  c[6].y = -c[0].y;
  wxPen old = dc.GetPen();
  dc.SetPen(wxPen(*wxBLUE));
  dc.DrawLines (7, c, WXROUND (m_xpos-w), WXROUND (m_ypos));
  dc.SetPen(old);
}


IMPLEMENT_DYNAMIC_CLASS (ComponentShape, MISOShape)


ComponentShape::ComponentShape (double w, double h, const wxString & Type, const wxString & Name):
  MISOShape (w, h, Type, Name),
  order(-9999),
  clipable(false),
  clipmax(wxEmptyString),
  clipmin(wxEmptyString)
{
  input_sign_list.DeleteContents(true);
}

ComponentShape::~ComponentShape()
{
  //input_sign_list.Clear();
}

void 
ComponentShape::OnDraw (wxDC & dc)
{
  MISOShape::OnDraw (dc);

  wxString str(wxString::Format("%ld",order));
  wxCoord w,h;
  dc.GetTextExtent(str,&w,&h);
  dc.DrawText(str,WXROUND (GetX()-m_width*0.5),WXROUND (GetY()-m_height*0.5-h-2));
  wxPoint tmp[2];
  wxNode *node = GetAttachments ().GetFirst ();
  wxAttachmentPoint * p;
  wxMyBoolListNode *snode = input_sign_list.GetFirst();
  while (node)
    {
      p = (wxAttachmentPoint *) node->GetData ();
      if (p->m_id > 0 && snode)
	{
          if (*(snode->GetData()))
	  {
	    if (is_draw_reverse)
	    {
	      tmp[0].x = WXROUND (p->m_x);
	      tmp[0].y = WXROUND (p->m_y);
	      tmp[1].x = WXROUND (p->m_x) - arr_size;
	      tmp[1].y = WXROUND (p->m_y);
	    }
	    else
	    {
	      tmp[0].x = WXROUND (p->m_x);
	      tmp[0].y = WXROUND (p->m_y);
	      tmp[1].x = WXROUND (p->m_x) + arr_size;
	      tmp[1].y = WXROUND (p->m_y);
	    }
            dc.DrawLines (2, tmp, WXROUND (m_xpos), WXROUND (m_ypos));
	  }
	  snode = snode->GetNext();
	}
      node = node->GetNext ();
    }
  if (clipable)
  {
    wxPoint c[4];
    c[0].x = (int)m_width/2 - 3;
    c[0].y = -(int)m_height/2 + 2;
    c[1].x = c[0].x - arr_size;
    c[1].y = c[0].y;
    c[2].x = c[1].x;
    c[2].y = c[1].y + arr_size;
    c[3].x = c[2].x - arr_size;
    c[3].y = c[2].y;
    dc.DrawLines (4, c, WXROUND (m_xpos), WXROUND (m_ypos));
  }
}


#if wxUSE_PROLOGIO
void
ComponentShape::WriteAttributes (wxExpr * clause)
{
  MISOShape::WriteAttributes (clause);

  clause->AddAttributeValue (wxT ("order"), order);

  if (clipable)
    clause->AddAttributeValue (wxT ("clipable"), 1L);
  else
    clause->AddAttributeValue (wxT ("clipable"), 0L);
  clause->AddAttributeValueString (wxT ("clipmax"), clipmax);
  clause->AddAttributeValueString (wxT ("clipmin"), clipmin);


  wxExpr *list = new wxExpr(wxExprList);
  wxMyBoolListNode * node = input_sign_list.GetFirst();
  while (node)
    {
      bool * b = node->GetData();
      if (*b)
	list->Append(new wxExpr(1L));
      else
	list->Append(new wxExpr(0L));	
      node = node->GetNext();
    }  
  clause->AddAttributeValue(wxT("InputS"), list);
}

void
ComponentShape::ReadAttributes (wxExpr * clause)
{
  MISOShape::ReadAttributes (clause);

  long int tmp;

  clause->GetAttributeValue(wxT ("order"), order);

  clause->GetAttributeValue(wxT ("clipable"), tmp);
  if (tmp == 0L)
    clipable = false;
  else
    clipable = true;
  clause->GetAttributeValue(wxT ("clipmax"), clipmax);
  if (clipable && clipmax.IsEmpty())
  {
    double d;
    clause->GetAttributeValue(wxT ("clipmax"), d);
    clipmax = wxString::Format(wxT("%g"), d);
  }
  clause->GetAttributeValue(wxT ("clipmin"), clipmin);
  if (clipable && clipmin.IsEmpty())
  {
    double d;
    clause->GetAttributeValue(wxT ("clipmin"), d);
    clipmin = wxString::Format(wxT("%g"), d);
  }

  input_sign_list.Clear();
  wxExpr *list = clause->AttributeValue(wxT("InputS"));
  if (list)
  {
    wxExpr *pointExpr = list->GetFirst();
    while (pointExpr)
    {
      if (pointExpr->IntegerValue() )
	input_sign_list.Append(new bool(true));
      else
	input_sign_list.Append(new bool(false));
      pointExpr = pointExpr->GetNext();
    }
  }
}
#endif

void 
ComponentShape::Copy (wxShape & copy)
{
  MISOShape::Copy (copy);

  wxASSERT (copy.IsKindOf (CLASSINFO (ComponentShape)));

  ComponentShape & ComponmentCopy = (ComponentShape &) copy;
  ComponmentCopy.order = order;
  ComponmentCopy.clipable = clipable;
  ComponmentCopy.clipmax = clipmax;
  ComponmentCopy.clipmin = clipmin;

  ComponmentCopy.input_sign_list.Clear();
  wxMyBoolListNode * node = input_sign_list.GetFirst();
  while (node)
    {
      bool * b = node->GetData();
      ComponmentCopy.input_sign_list.Append(new bool(*b));
      node = node->GetNext();
    }
}


void
ComponentShape::SetOrder()
{
  order = 90;

  wxDiagram *diagram = GetCanvas()->GetDiagram ();
  wxObjectList::compatibility_iterator node = diagram->GetShapeList ()->GetFirst ();
  while (node)
    {
      wxShape *eachShape = (wxShape *) node->GetData ();
      if ( eachShape->IsKindOf(CLASSINFO(ComponentShape)))
	{
	  ComponentShape * theShape = (ComponentShape *)eachShape;
	  if ( theShape->GetOrder() > order )
	    order = theShape->GetOrder();
	}
	node = node->GetNext ();
    }
  order += 10;
}


void
ComponentShape::SetOrder(long int & o)
{
  if ( order == o )
    return;

  wxDiagram *diagram = GetCanvas()->GetDiagram ();
  wxObjectList::compatibility_iterator node = diagram->GetShapeList ()->GetFirst ();
  while (node)
    {
      wxShape *eachShape = (wxShape *) node->GetData ();
      if ( eachShape->IsKindOf(CLASSINFO(ComponentShape)))
	{
	  ComponentShape * theShape = (ComponentShape *)eachShape;
	  if ( theShape->GetOrder() == o )
	    {
	      theShape->order = order;
	      order = o;
	      return;
	    }
	}
	node = node->GetNext ();
    }
  order = o;
}

void 
ComponentShape::ExportXML(wxTextOutputStream & stream, const wxString & prefix)
{
  ExportHead(stream, prefix);

  wxString Pre = prefix + wxT("  ");
  ExportInputs(stream,Pre);
  ExportCliper(stream,Pre);
  ExportOutput(stream,Pre);
  
  ExportTail(stream, prefix);
}

void 
ComponentShape::ExportCliper(wxTextOutputStream & stream, const wxString & prefix)
{
  if (clipable)
  {
    stream << prefix << wxT("<clipto>") << endl;
    stream << prefix << wxT("  <min>") << clipmin << wxT("</min>") << endl;
    stream << prefix << wxT("  <max>") << clipmax << wxT("</max>") << endl;
    stream << prefix << wxT("</clipto>") << endl;
  }
}

wxString 
ComponentShape::mkName(wxString name, bool lowercase) 
{
  if (lowercase)
    name.LowerCase();
  
  for(size_t i = 0, n = name.Length(); i < n ; i++) 
  {
    if( name[i] == ' ' )
      name[i]='-';
  }

  return wxT("fcs/") + name;
}

void 
ComponentShape::ExportInputs(wxTextOutputStream & stream, const wxString & prefix)
{
  wxNode * node = GetLines().GetFirst();
  while (node)
  {
    wxLineShape * data = (wxLineShape *)node->GetData();
    if (data->GetTo() == this)  // if this line point to this shape
    {
      wxShape * shape = data->GetFrom(); // input shape
      while (shape && shape->IsKindOf(CLASSINFO(SIMOShape))) //if input shape is a SIMO Shape, find the input shape of the SIMO Shape
      {
        wxNode * nd = shape->GetLines().GetFirst();
	bool flag = false;
	while (nd)
	{
	  wxLineShape * value = (wxLineShape *)nd->GetData();
	  if (value->GetTo() == shape)
	  {
	    shape = value->GetFrom();  // get it!
	    flag = true;
	    break;
	  }
          nd = nd->GetNext();
	}
	if (!flag)
	{
	  shape = 0; // no find
	}
      }
      //output
      if (shape && shape->IsKindOf(CLASSINFO(MISOShape)))
      {
	 int a = data->GetAttachmentTo();
	 bool b = *(input_sign_list.Item(a-1)->GetData());
	 wxString c;
	 if (b)
	   c = wxT("-");
         MISOShape * misoshape = (MISOShape *)shape;
	 if (misoshape->IsKindOf(CLASSINFO(ComponentShape)))
	   stream << prefix << wxT("<input> ")<< c << mkName( misoshape->GetName(), true ) << wxT(" </input>") << endl;
	 else
	   stream << prefix << wxT("<input> ")<< c << misoshape->GetName() << wxT(" </input>") << endl;
      }
    }
    node = node->GetNext();
  }
}

DestinationShape *
ComponentShape::GetOutputNode(SIMOShape * shape)
{
  wxNode * node = shape->GetLines().GetFirst();
  while (node)
  {
    wxLineShape * data = (wxLineShape *)node->GetData();
    if (data->GetFrom()==shape)
    {
      wxShape * other = data->GetTo();
      if (other->IsKindOf(CLASSINFO(SIMOShape)))
      {
	DestinationShape * result;
	if ( result = GetOutputNode((SIMOShape *)other) )
	  return result;
      }
      else if (other->IsKindOf(CLASSINFO(DestinationShape)))
	return (DestinationShape *)other;
    }
    node = node->GetNext();
  }
  return NULL;
}

void 
ComponentShape::ExportOutput(wxTextOutputStream & stream, const wxString & prefix)
{
  wxNode * node = GetLines().GetFirst();
  while (node)
  {
    wxLineShape * data = (wxLineShape *)node->GetData();
    if (data->GetFrom() == this)  // if this line point from this shape
    {
      wxShape * shape = data->GetTo(); // input shape
      DestinationShape * result = NULL;
      if (shape->IsKindOf(CLASSINFO(SIMOShape)))
      {
	result = GetOutputNode((SIMOShape *)shape);
      }
      else if (shape->IsKindOf(CLASSINFO(DestinationShape)))
      {
	result = (DestinationShape *)shape;
      }
      //output
      if (result)
      {
	stream << prefix << wxT("<output> ")<< result->GetName() << wxT(" </output>") << endl;
      }
      break; //only one output
    }
    node = node->GetNext();
  }
}

wxArrayString
ComponentShape::ImportXML(JSBSim::Element * el)
{
  name = el->GetAttributeValue("name").c_str();

  JSBSim::Element * clipto = el->FindElement("clipto");
  if (clipto)
  {
    clipmin = clipto->FindElementValue("min");
    clipmax = clipto->FindElementValue("max");
    if (!clipmin.IsEmpty() && !clipmax.IsEmpty())
      clipable = true;
    else
      clipable = false;
  }
  else
    clipable = false;

  wxArrayString array;
  double w = GetWidth();
  double h = GetHeight();
  input_sign_list.Clear();
  ClearAttachments ();
  GetAttachments ().Append (new wxAttachmentPoint (0,  w * 0.5, 0.0));
  
  JSBSim::Element * output_element = el->FindElement("output");
  if (output_element)
  {
    wxString output_name = output_element->GetDataLine().c_str();
    size_t i = 0u;
    while (output_name[i]==' ' && output_name[i]!='\0')
      ++i;
    wxString tmp;
    while (output_name[i]!=' ' && output_name[i]!='\0')
    {
      tmp += output_name[i++];
    }
    array.Add(tmp);
  }
  else
  {
    array.Add(wxString(wxT("")));
  }
 
  JSBSim::Element * input_element = el->FindElement("input");

  if (!input_element)
    return array;

  int input_port_num = 1;
  while (input_element)
  {
    wxString input_name = input_element->GetDataLine().c_str();
    size_t i = 0u;
    while (input_name[i]==' ' && input_name[i]!='\0')
      ++i;
    bool sign_flag = false;
    if (input_name[i]=='-' && input_name[i]!='\0')
    {
      sign_flag = true;
      ++i;
    }
    wxString tmp;
    while (input_name[i]!=' ' && input_name[i]!='\0')
    {
      tmp += input_name[i++];
    }

    if (!tmp.IsEmpty())
    {
      input_sign_list.Append(new bool(sign_flag));
      GetAttachments ().Append (new wxAttachmentPoint (input_port_num++, -w * 0.5, 0.0));
      array.Add(tmp);
    }
  
    input_element = el->FindNextElement("input");
  }
  
  SetAttachmentSize(w,h);

  return array;
}

