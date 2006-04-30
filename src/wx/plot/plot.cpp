/////////////////////////////////////////////////////////////////////////////
// Name:        plot.cpp
// Purpose:     wxPlotWindow
// Author:      Robert Roebling
// Modified by: Matthew Gong
// Created:     12/01/2000
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "plot.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/object.h"
#include "wx/font.h"
#include "wx/colour.h"
#include "wx/settings.h"
#include "wx/sizer.h"
#include "wx/log.h"
#include "wx/intl.h"
#include "wx/dcclient.h"
#endif

#include "wx/plot/plot.h"
#include "wx/bmpbuttn.h"
#include "wx/module.h"

#include <math.h>

// ----------------------------------------------------------------------------
// XPMs
// ----------------------------------------------------------------------------

#if !defined(__WXMSW__) && !defined(__WXPM__)
    #include "wx/plot/plot_enl.xpm"
    #include "wx/plot/plot_shr.xpm"
    #include "wx/plot/plot_zin.xpm"
    #include "wx/plot/plot_zot.xpm"
    #include "wx/plot/plot_up.xpm"
    #include "wx/plot/plot_dwn.xpm"
#endif

//----------------------------------------------------------------------------
// event types
//----------------------------------------------------------------------------

DEFINE_EVENT_TYPE(wxEVT_PLOT_SEL_CHANGING);
DEFINE_EVENT_TYPE(wxEVT_PLOT_SEL_CHANGED);
DEFINE_EVENT_TYPE(wxEVT_PLOT_AREA_CLICKED);
DEFINE_EVENT_TYPE(wxEVT_PLOT_CLICKED);
DEFINE_EVENT_TYPE(wxEVT_PLOT_DOUBLECLICKED);
DEFINE_EVENT_TYPE(wxEVT_PLOT_ZOOM_IN);
DEFINE_EVENT_TYPE(wxEVT_PLOT_ZOOM_OUT);
DEFINE_EVENT_TYPE(wxEVT_PLOT_VALUE_SEL_CREATING);
DEFINE_EVENT_TYPE(wxEVT_PLOT_VALUE_SEL_CREATED);
DEFINE_EVENT_TYPE(wxEVT_PLOT_VALUE_SEL_CHANGING);
DEFINE_EVENT_TYPE(wxEVT_PLOT_VALUE_SEL_CHANGED);
DEFINE_EVENT_TYPE(wxEVT_PLOT_AREA_SEL_CREATING);
DEFINE_EVENT_TYPE(wxEVT_PLOT_AREA_SEL_CREATED);
DEFINE_EVENT_TYPE(wxEVT_PLOT_AREA_SEL_CHANGING);
DEFINE_EVENT_TYPE(wxEVT_PLOT_AREA_SEL_CHANGED);
DEFINE_EVENT_TYPE(wxEVT_PLOT_BEGIN_X_LABEL_EDIT);
DEFINE_EVENT_TYPE(wxEVT_PLOT_END_X_LABEL_EDIT);
DEFINE_EVENT_TYPE(wxEVT_PLOT_BEGIN_Y_LABEL_EDIT);
DEFINE_EVENT_TYPE(wxEVT_PLOT_END_Y_LABEL_EDIT);
DEFINE_EVENT_TYPE(wxEVT_PLOT_BEGIN_TITLE_EDIT);
DEFINE_EVENT_TYPE(wxEVT_PLOT_END_TITLE_EDIT);
DEFINE_EVENT_TYPE(wxEVT_PLOT_AREA_CREATE);
DEFINE_EVENT_TYPE(wxEVT_PLOT_AREA_MOTION);

//----------------------------------------------------------------------------
// accessor functions for the bitmaps (may return NULL, check for it!)
//----------------------------------------------------------------------------

static wxBitmap *GetEnlargeBitmap();
static wxBitmap *GetShrinkBitmap();
static wxBitmap *GetZoomInBitmap();
static wxBitmap *GetZoomOutBitmap();
static wxBitmap *GetUpBitmap();
static wxBitmap *GetDownBitmap();

//-----------------------------------------------------------------------------
// consts
//-----------------------------------------------------------------------------

#define wxPLOT_SCROLL_STEP  30

//-----------------------------------------------------------------------------
// wxPlotEvent
//-----------------------------------------------------------------------------

wxPlotEvent::wxPlotEvent( wxEventType commandType, int id )
    : wxNotifyEvent( commandType, id )
{
    m_curve = (wxPlotCurve*) NULL;
    m_zoom = 1.0;
    m_position = 0;
}

//-----------------------------------------------------------------------------
// wxPlotCurve
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxPlotCurve, wxObject)

wxPlotCurve::wxPlotCurve( int offsetY, double startY, double endY )
  //Matt's change start
  : x_label(wxEmptyString), y_label(wxEmptyString), x_start(0)
  //Matt's change end
{
    m_offsetY = offsetY;
    m_startY = startY;
    m_endY = endY;
}

//-----------------------------------------------------------------------------
// wxPlotOnOffCurve
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxPlotOnOffCurve, wxObject)

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(wxArrayPlotOnOff);

wxPlotOnOffCurve::wxPlotOnOffCurve( int offsetY )
{
    m_offsetY = offsetY;
    m_minX = -1;
    m_maxX = -1;
}

void wxPlotOnOffCurve::Add( wxInt32 on, wxInt32 off, void *clientData )
{
    wxASSERT_MSG( on > 0, _T("plot index < 0") );
    wxASSERT( on <= off );

    if (m_minX == -1)
        m_minX = on;
    if (off > m_maxX)
        m_maxX = off;

    wxPlotOnOff *v = new wxPlotOnOff;
    v->m_on = on;
    v->m_off = off;
    v->m_clientData = clientData;
    m_marks.Add( v );
}

size_t wxPlotOnOffCurve::GetCount()
{
    return m_marks.GetCount();
}

wxInt32 wxPlotOnOffCurve::GetOn( size_t index )
{
    wxPlotOnOff *v = &m_marks.Item( index );
    return v->m_on;
}

wxInt32 wxPlotOnOffCurve::GetOff( size_t index )
{
    wxPlotOnOff *v = &m_marks.Item( index );
    return v->m_off;
}

void* wxPlotOnOffCurve::GetClientData( size_t index )
{
    wxPlotOnOff *v = &m_marks.Item( index );
    return v->m_clientData;
}

wxPlotOnOff *wxPlotOnOffCurve::GetAt( size_t index )
{
    return &m_marks.Item( index );
}

void wxPlotOnOffCurve::DrawOnLine( wxDC &dc, wxCoord y, wxCoord start, wxCoord end, void *WXUNUSED(clientData) )
{
    dc.DrawLine( start, y, start, y-30 );
    dc.DrawLine( start, y-30, end, y-30 );
    dc.DrawLine( end, y-30, end, y );
}

void wxPlotOnOffCurve::DrawOffLine( wxDC &dc, wxCoord y, wxCoord start, wxCoord end )
{
    dc.DrawLine( start, y, end, y );
}

//-----------------------------------------------------------------------------
// wxPlotArea
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxPlotArea, wxPanel)

BEGIN_EVENT_TABLE(wxPlotArea, wxPanel)
    EVT_PAINT(        wxPlotArea::OnPaint)
    EVT_LEFT_DOWN(    wxPlotArea::OnMouse)
    EVT_LEFT_DCLICK(  wxPlotArea::OnMouse)
    EVT_RIGHT_DOWN(  wxPlotArea::OnMouseRClick)
    EVT_MOTION(  wxPlotArea::OnMouseMotion) 
END_EVENT_TABLE()

wxPlotArea::wxPlotArea( wxPlotWindow *parent )
        : wxPanel( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER, _T("plotarea") )
{
    m_owner = parent;

    m_zooming = false;

    SetBackgroundColour( *wxWHITE );
}

void wxPlotArea::OnMouseMotion( wxMouseEvent &event )
{
    //Matt's change start
    wxPlotEvent event1( wxEVT_PLOT_AREA_MOTION , m_owner->GetId() );
    event1.SetX(event.GetX());
    event1.SetY(event.GetY());
    m_owner->GetEventHandler()->ProcessEvent( event1 );
    //Matt's change end
}

void wxPlotWindow::OnPlotAreaMotion( wxPlotEvent& event )
{
  MakeMark(event.GetX());
  event.Skip();
}

void wxPlotWindow::MakeMark(wxCoord x)
{
    int client_width;
    int client_height;
    m_area->GetClientSize( &client_width, &client_height);

    double double_client_height = (double)client_height;
    double double_client_width = (double)client_width;
    double range;
    double end;
    if ( m_samerange )
    {
        double tmp_start;
        double tmp_end;
        bool first = true;
        wxList::compatibility_iterator node = m_curves.GetFirst();
        while (node)
        {
            wxPlotCurve *curve = (wxPlotCurve*) node->GetData();
            if (first || curve->GetStartY() < tmp_start)
            {
              tmp_start = curve->GetStartY();
            }
            if (first || curve->GetEndY() > tmp_end)
            {
              tmp_end  = curve->GetEndY();
            }
            if (first)
            {
              first = false;
            }
            node = node->GetNext();
        }
        range = tmp_end - tmp_start;
        end = tmp_end;
    }
    wxPlotCurve *curve = GetCurrentCurve();
    if (curve)
    {
          if (! m_samerange )
          {
            range = curve->GetEndY() - curve->GetStartY();
            end = curve->GetEndY();
          }
          double offset = curve->GetStartXVal();
  
          double ups = GetUnitsPerValue() / GetZoom();
  
          double start_x =  offset;
          double end_x = (client_width) * ups + offset;
          double range_x = end_x - start_x;

          double dy = curve->GetY( (wxInt32)(x/GetZoom()) );

          wxClientDC dc(m_area);
          PrepareDC( dc );
          wxString text = wxString::Format(wxT("%9.4f, %9.4f"), start_x+x*range_x/double_client_width, dy);
          wxCoord textw, texth;
          dc.GetTextExtent(text, &textw, &texth);
          dc.SetLogicalFunction(wxINVERT);
          if (old_x >= 0 && old_x <= client_width)
          {
            dc.DrawLine(old_x, 0, old_x, client_height);
            dc.SetTextForeground(*wxWHITE);
            dc.DrawText(old_mark, client_width-old_w, client_height-old_h);
          }
          dc.SetTextForeground(*wxBLACK);
          dc.DrawLine(x, 0, x, client_height);
          dc.DrawText(text, client_width-textw, client_height-texth);
          old_x = x;
	  old_w = textw;
	  old_h = texth;
	  old_mark = text;
    }
}

void wxPlotArea::OnMouseRClick( wxMouseEvent &event )
{
    int client_width;
    int client_height;
    GetClientSize( &client_width, &client_height);
    int view_x=0;
    int view_y=0;
    //m_owner->GetViewStart( &view_x, &view_y );
    view_x *= wxPLOT_SCROLL_STEP;
    view_y *= wxPLOT_SCROLL_STEP;

    wxCoord x = event.GetX();
    wxCoord y = event.GetY();
    x += view_x;
    y += view_y;

    double double_client_height = (double)client_height;
    double double_client_width = (double)client_width;
    double range;
    double end;
    if ( m_owner->m_samerange )
    {
        double tmp_start;
        double tmp_end;
        bool first = true;
        wxList::compatibility_iterator node = m_owner->m_curves.GetFirst();
        while (node)
        {
            wxPlotCurve *curve = (wxPlotCurve*) node->GetData();
            if (first || curve->GetStartY() < tmp_start)
            {
              tmp_start = curve->GetStartY();
            }
            if (first || curve->GetEndY() > tmp_end)
            {
              tmp_end  = curve->GetEndY();
            }
            if (first)
            {
              first = false;
            }
            node = node->GetNext();
        }
        range = tmp_end - tmp_start;
        end = tmp_end;
    }
    wxPlotCurve *curve = m_owner->GetCurrentCurve();
    if (curve)
    {
          if (! m_owner->m_samerange )
          {
            range = curve->GetEndY() - curve->GetStartY();
            end = curve->GetEndY();
          }
          double offset = curve->GetStartXVal();
  
          double ups = m_owner->GetUnitsPerValue() / m_owner->GetZoom();
  
          double start_x = view_x * ups + offset;
          double end_x = (view_x + client_width) * ups + offset;
          double range_x = end_x - start_x;

          wxCoord offset_y = curve->GetOffsetY();
          double yvalue = curve->GetY( (wxInt32)(x/m_owner->GetZoom()) );
          double dy = (end - yvalue) / range;
          y = (wxCoord)(dy * double_client_height) - offset_y - 1;

          wxClientDC dc(this);
          m_owner->PrepareDC( dc );
          dc.SetPen( *wxRED_PEN );
          dc.DrawLine(x-3, y, x+3, y);
          dc.DrawLine(x, y-3, x, y+3);
          wxString text = wxString::Format(wxT("(%g, %g)"), start_x+x*range_x/double_client_width, yvalue);
          wxCoord textw, texth;
          dc.GetTextExtent(text, &textw, &texth);
          if (y+texth > client_height)
          {
            if (x+textw > client_width)
            {
              dc.DrawText(text, x-textw, client_height-texth);
            }
            else
            {
              dc.DrawText(text, x+3, client_height-texth);
            }
          }
          else
          {
            if (x+textw > client_width)
            {
              dc.DrawText(text, x-textw, y);
            }
            else
            {
              dc.DrawText(text, x, y);
            }
          }
    }
}

void wxPlotArea::OnMouse( wxMouseEvent &event )
{
    //Matt's change start
    wxPlotEvent event1( wxEVT_PLOT_AREA_CLICKED , m_owner->GetId() );
    m_owner->GetEventHandler()->AddPendingEvent( event1 );
    //Matt's change end

    int client_width;
    int client_height;
    GetClientSize( &client_width, &client_height);
    int view_x=0;
    int view_y=0;
    //m_owner->GetViewStart( &view_x, &view_y );
    view_x *= wxPLOT_SCROLL_STEP;
    view_y *= wxPLOT_SCROLL_STEP;

    wxCoord x = event.GetX();
    wxCoord y = event.GetY();
    x += view_x;
    y += view_y;

    //Matt's change start
    double double_client_height = (double)client_height;
    double double_client_width = (double)client_width;
    double range;
    double end;
    if ( m_owner->m_samerange )
    {
        double tmp_start;
        double tmp_end;
        bool first = true;
        wxList::compatibility_iterator node = m_owner->m_curves.GetFirst();
        while (node)
        {
            wxPlotCurve *curve = (wxPlotCurve*) node->GetData();
            if (first || curve->GetStartY() < tmp_start)
            {
              tmp_start = curve->GetStartY();
            }
            if (first || curve->GetEndY() > tmp_end)
            {
              tmp_end  = curve->GetEndY();
            }
            if (first)
            {
              first = false;
            }
            node = node->GetNext();
        }
        range = tmp_end - tmp_start;
        end = tmp_end;
    }
    //Matt's change end
    wxList::compatibility_iterator node = m_owner->m_curves.GetFirst();
    while (node)
    {
        wxPlotCurve *curve = (wxPlotCurve*)node->GetData();

        //Matt's change start
        //double double_client_height = (double)client_height;
        //double range = curve->GetEndY() - curve->GetStartY();
        //double end = curve->GetEndY();
        if (! m_owner->m_samerange )
        {
          range = curve->GetEndY() - curve->GetStartY();
          end = curve->GetEndY();
        }
        //Matt's change end
        wxCoord offset_y = curve->GetOffsetY();

        double dy = (end - curve->GetY( (wxInt32)(x/m_owner->GetZoom()) )) / range;
        wxCoord curve_y = (wxCoord)(dy * double_client_height) - offset_y - 1;

        if ((y-curve_y < 4) && (y-curve_y > -4))
        {
            wxPlotEvent event1( event.ButtonDClick() ? wxEVT_PLOT_DOUBLECLICKED : wxEVT_PLOT_CLICKED, m_owner->GetId() );
            event1.SetEventObject( m_owner );
            event1.SetZoom( m_owner->GetZoom() );
            event1.SetCurve( curve );
            event1.SetPosition( (int)floor(x/m_owner->GetZoom()) );
            m_owner->GetEventHandler()->ProcessEvent( event1 );

            if (curve != m_owner->GetCurrentCurve())
            {
                wxPlotEvent event2( wxEVT_PLOT_SEL_CHANGING, m_owner->GetId() );
                event2.SetEventObject( m_owner );
                event2.SetZoom( m_owner->GetZoom() );
                event2.SetCurve( curve );
                if (!m_owner->GetEventHandler()->ProcessEvent( event2 ) || event2.IsAllowed())
                {
                    m_owner->SetCurrentCurve( curve );
                    m_owner->MakeMark(x);
                }
            }
            return;
        }

        node = node->GetNext();
    }
}

void wxPlotArea::DeleteCurve( wxPlotCurve *curve, int from, int to )
{
    wxClientDC dc(this);
    m_owner->PrepareDC( dc );
    dc.SetPen( *wxWHITE_PEN );
    DrawCurve( &dc, curve, from, to );
}

void wxPlotArea::DrawCurve( wxDC *dc, wxPlotCurve *curve, int from, int to )
{
    int view_x=0;
    int view_y=0;
    //m_owner->GetViewStart( &view_x, &view_y );
    view_x *= wxPLOT_SCROLL_STEP;

    if (from == -1)
        from = view_x;

    int client_width;
    int client_height;
    GetClientSize( &client_width, &client_height);

    if (to == -1)
        to = view_x + client_width;

    double zoom = m_owner->GetZoom();

    int start_x = wxMax( from, (int)floor(curve->GetStartX()*zoom) );
    int end_x = wxMin( to, (int)floor(curve->GetEndX()*zoom) );

    start_x = wxMax( view_x, start_x );
    end_x = wxMin( view_x + client_width, end_x );

    end_x++;

    double double_client_height = (double)client_height;
    double range = curve->GetEndY() - curve->GetStartY();
    double end = curve->GetEndY();
    //Matt's change start
    if ( m_owner->m_samerange )
    {
        double tmp_start = curve->GetStartY();
        double tmp_end  = curve->GetEndY();
        wxList::compatibility_iterator node = m_owner->m_curves.GetFirst();
        while (node)
        {
            wxPlotCurve *curve = (wxPlotCurve*) node->GetData();
            if (curve->GetStartY() < tmp_start)
            {
              tmp_start = curve->GetStartY();
            }
            if (curve->GetEndY() > tmp_end)
            {
              tmp_end  = curve->GetEndY();
            }
            node = node->GetNext();
        }
        range = tmp_end - tmp_start;
        end = tmp_end;
    }
    //Matt's change end
    wxCoord offset_y = curve->GetOffsetY();

    wxCoord last_y=0;
    for (int x = start_x; x < end_x; x++)
    {
        double dy = (end - curve->GetY( (wxInt32)(x/zoom) )) / range;
        wxCoord y = (wxCoord)(dy * double_client_height) - offset_y - 1;

        if (x != start_x)
           dc->DrawLine( x-1, last_y, x, y );

        last_y = y;
    }
}

void wxPlotArea::DrawOnOffCurve( wxDC *dc, wxPlotOnOffCurve *curve, int from, int to )
{
    int view_x=0;
    int view_y=0;
    //m_owner->GetViewStart( &view_x, &view_y );
    view_x *= wxPLOT_SCROLL_STEP;

    if (from == -1)
        from = view_x;

    int client_width;
    int client_height;
    GetClientSize( &client_width, &client_height);

    if (to == -1)
        to = view_x + client_width;

    double zoom = m_owner->GetZoom();

    int start_x = wxMax( from, (int)floor(curve->GetStartX()*zoom) );
    int end_x = wxMin( to, (int)floor(curve->GetEndX()*zoom) );

    start_x = wxMax( view_x, start_x );
    end_x = wxMin( view_x + client_width, end_x );

    end_x++;

    wxCoord offset_y = curve->GetOffsetY();
    wxCoord last_off = -5;

    if (curve->GetCount() == 0)
        return;

    for (size_t index = 0; index < curve->GetCount(); index++)
    {
        wxPlotOnOff *p = curve->GetAt( index );

        wxCoord on = (wxCoord)(p->m_on*zoom);
        wxCoord off = (wxCoord)(p->m_off*zoom);

        if (end_x < on)
        {
            curve->DrawOffLine( *dc, client_height-offset_y, last_off, on );
            break;
        }

        if (off >= start_x)
        {
            curve->DrawOffLine( *dc, client_height-offset_y, last_off, on );
            curve->DrawOnLine( *dc, client_height-offset_y, on, off, p->m_clientData );
        }
        last_off = off;
    }

    wxPlotOnOff *p = curve->GetAt( curve->GetCount()-1 );
    wxCoord off = (wxCoord)(p->m_off*zoom);
    if (off < end_x)
        curve->DrawOffLine( *dc, client_height-offset_y, off, to );
}

void wxPlotArea::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    int view_x=0;
    int view_y=0;
    //m_owner->GetViewStart( &view_x, &view_y );
    view_x *= wxPLOT_SCROLL_STEP;
    view_y *= wxPLOT_SCROLL_STEP;

    m_owner->old_x = -1;
    m_owner->old_h = -1;
    m_owner->old_w = -1;
    wxPaintDC dc( this );
    m_owner->PrepareDC( dc );

    //wxRegionIterator upd( GetUpdateRegion() );
    int client_width;
    int client_height;
    GetClientSize( &client_width, &client_height);

    //while (upd)
    {
        int update_x = 0;
        int update_width = client_width;

/*
        if (m_owner->m_current)
        {
            dc.SetPen( *wxLIGHT_GREY_PEN );
            int base_line = client_height - m_owner->m_current->GetOffsetY();
            dc.DrawLine( update_x-1, base_line-1, update_x+update_width+2, base_line-1 );
        }
*/

        wxList::compatibility_iterator node = m_owner->m_curves.GetFirst();
        int j = 0;
        while (node)
        {
            wxPlotCurve *curve = (wxPlotCurve*) node->GetData();
            if (curve == m_owner->GetCurrentCurve())
                dc.SetPen( *wxBLACK_PEN );
            else
            {
              switch (j % 6)
              {
                case 0 :
                  dc.SetPen( *wxRED_PEN );
                  break;
                case 1 :
                  dc.SetPen( wxPen(wxColour(255, 255, 0)) );
                  break;
                case 2 :
                  dc.SetPen( *wxGREEN_PEN );
                  break;
                case 3 :
                  dc.SetPen( *wxCYAN_PEN );
                  break;
                case 4 :
                  dc.SetPen( wxPen(wxColour(0, 0, 255)) );
                  break;
                case 5 :
                  dc.SetPen( wxPen(wxColour(255, 0, 255)) );
                  break;
                default :
                  dc.SetPen( *wxLIGHT_GREY_PEN );
                  break;
              }
            }

            DrawCurve( &dc, curve, update_x-1, update_x+update_width+2 );

            node = node->GetNext();
            ++j;
        }

        dc.SetPen( *wxRED_PEN );

        node = m_owner->m_onOffCurves.GetFirst();
        while (node)
        {
            wxPlotOnOffCurve *curve = (wxPlotOnOffCurve*) node->GetData();

            DrawOnOffCurve( &dc, curve, update_x-1, update_x+update_width+2 );

            node = node->GetNext();
        }

        //upd ++;
    }
}

//-----------------------------------------------------------------------------
// wxPlotXAxisArea
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxPlotXAxisArea, wxPanel)

BEGIN_EVENT_TABLE(wxPlotXAxisArea, wxPanel)
  EVT_PAINT(        wxPlotXAxisArea::OnPaint)
  EVT_LEFT_DOWN(    wxPlotXAxisArea::OnMouse)
END_EVENT_TABLE()

wxPlotXAxisArea::wxPlotXAxisArea( wxPlotWindow *parent )
        : wxPanel( parent, wxID_ANY, wxDefaultPosition, wxSize(wxDefaultCoord,40), 0, _T("plotxaxisarea") )
{
    m_owner = parent;

    SetBackgroundColour( *wxWHITE );
    SetFont( *wxSMALL_FONT );
}

void wxPlotXAxisArea::OnMouse( wxMouseEvent &event )
{
    //Matt's change start
    wxPlotEvent event1( wxEVT_PLOT_AREA_CLICKED , m_owner->GetId() );
    m_owner->GetEventHandler()->AddPendingEvent( event1 );
    //Matt's change end
    int client_width;
    int client_height;
    GetClientSize( &client_width, &client_height);
    int view_x=0;
    int view_y=0;
    //m_owner->GetViewStart( &view_x, &view_y );
    view_x *= wxPLOT_SCROLL_STEP;
    view_y *= wxPLOT_SCROLL_STEP;

    wxCoord x = event.GetX() + view_x;
    wxCoord y = event.GetY() + view_y;

    /* TO DO: do something here */
    wxUnusedVar(x);
    wxUnusedVar(y);
}

void wxPlotXAxisArea::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    int view_x=0;
    int view_y=0;
    //m_owner->GetViewStart( &view_x, &view_y );
    view_x *= wxPLOT_SCROLL_STEP;
    view_y *= wxPLOT_SCROLL_STEP;

    double offset = 0;
    wxString x_label = wxEmptyString;
    if (wxPlotCurve *curve = m_owner->GetCurrentCurve())
    {
      offset = curve->GetStartXVal();
      x_label  = curve->GetXLabel();
    }

    wxPaintDC dc( this );

    int client_width;
    int client_height;
    GetClientSize( &client_width, &client_height);

    double zoom = m_owner->GetZoom();

    double ups = m_owner->GetUnitsPerValue() / zoom;

    double start = view_x * ups + offset;
    double end = (view_x + client_width) * ups + offset;
    double range = end - start;

    int int_log_range = (int)floor( log10( range ) );
    double step = 1.0;
    if (int_log_range > 0)
    {
        for (int i = 0; i < int_log_range; i++)
           step *= 10;
    }
    if (int_log_range < 0)
    {
        for (int i = 0; i < -int_log_range; i++)
           step /= 10;
    }
    double lower = ceil(start / step) * step;
    double upper = floor(end / step) * step;

    // if too few values, shrink size
    if ((range/step) < 8)
    {
        step /= 2;
        if (lower-step > start) lower -= step;
        if (upper+step < end) upper += step;
    }

    // if still too few, again
    if ((range/step) < 8)
    {
        step /= 2;
        if (lower-step > start) lower -= step;
        if (upper+step < end) upper += step;
    }

    dc.SetBrush( *wxWHITE_BRUSH );
    dc.SetPen( *wxTRANSPARENT_PEN );
    dc.DrawRectangle( 4, 5, client_width-14, 10 );
    dc.DrawRectangle( 0, 20, client_width, 20 );
    dc.SetPen( *wxBLACK_PEN );

    double current = lower;
    while (current < upper+(step/2))
    {
        int x = (int)ceil((current-start) / range * (double)client_width) - 1;
        if ((x > 4) && (x < client_width-75))
        {
            dc.DrawLine( x, 5, x, 15 );
            wxString label;
            if (range < 50)
            {
                label.Printf( _T("%g"), current );
                while (label.Last() == _T('0'))
                    label.RemoveLast();
                if ((label.Last() == _T('.')) || (label.Last() == _T(',')))
                    label.Append( _T('0') );
            }
            else
                label.Printf( _T("%d"), (int)floor(current) );
            dc.DrawText( label, x-4, 20 );
        }

        current += step;
    }

    dc.DrawLine( 0, 15, client_width-8, 15 );
    dc.DrawLine( client_width-4, 15, client_width-10, 10 );
    dc.DrawLine( client_width-4, 15, client_width-10, 20 );
    dc.DrawText( x_label, client_width-50, 20 );
}

//-----------------------------------------------------------------------------
// wxPlotYAxisArea
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxPlotYAxisArea, wxPanel)

BEGIN_EVENT_TABLE(wxPlotYAxisArea, wxPanel)
  EVT_PAINT(        wxPlotYAxisArea::OnPaint)
  EVT_LEFT_DOWN(    wxPlotYAxisArea::OnMouse)
END_EVENT_TABLE()

wxPlotYAxisArea::wxPlotYAxisArea( wxPlotWindow *parent )
        : wxPanel( parent, wxID_ANY, wxDefaultPosition, wxSize(100,wxDefaultCoord), 0, _T("plotyaxisarea") )
{
    m_owner = parent;

    SetBackgroundColour( *wxWHITE );
    SetFont( *wxSMALL_FONT );
}

void wxPlotYAxisArea::OnMouse( wxMouseEvent &event )
{
    //Matt's change start
    wxPlotEvent event1( wxEVT_PLOT_AREA_CLICKED , m_owner->GetId() );
    m_owner->GetEventHandler()->AddPendingEvent( event1 );
    //Matt's change end
    /* do something here */
}

void wxPlotYAxisArea::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dc( this );

    wxPlotCurve *curve = m_owner->GetCurrentCurve();

    if (!curve) return;

    int client_width;
    int client_height;
    GetClientSize( &client_width, &client_height);


    //Matt's change start
    //double range = curve->GetEndY() - curve->GetStartY();
    //double offset = ((double) curve->GetOffsetY() / (double)client_height ) * range;
    //double start = curve->GetStartY() - offset;
    //double end = curve->GetEndY() - offset;
    double range, offset, start, end;
    if ( m_owner->m_samerange )
    {
        double tmp_start = curve->GetStartY();
        double tmp_end  = curve->GetEndY();
        wxList::compatibility_iterator node = m_owner->m_curves.GetFirst();
        while (node)
        {
            wxPlotCurve *curve = (wxPlotCurve*) node->GetData();
            if (curve->GetStartY() < tmp_start)
            {
              tmp_start = curve->GetStartY();
            }
            if (curve->GetEndY() > tmp_end)
            {
              tmp_end  = curve->GetEndY();
            }
            node = node->GetNext();
        }
        range = tmp_end - tmp_start;
        offset = 0;
        start = tmp_start;
        end = tmp_end;
    }
    else
    {
      range = curve->GetEndY() - curve->GetStartY();
      offset = ((double) curve->GetOffsetY() / (double)client_height ) * range;
      start = curve->GetStartY() - offset;
      end = curve->GetEndY() - offset;
    }
    //Matt's change end

    int int_log_range = (int)floor( log10( range ) );
    double step = 1.0;
    if (int_log_range > 0)
    {
        for (int i = 0; i < int_log_range; i++)
           step *= 10;
    }
    if (int_log_range < 0)
    {
        for (int i = 0; i < -int_log_range; i++)
           step /= 10;
    }
    double lower = ceil(start / step) * step;
    double upper = floor(end / step) * step;

    // if too few values, shrink size
    if ((range/step) < 4)
    {
        step /= 2;
        if (lower-step > start) lower -= step;
        if (upper+step < end) upper += step;
    }

    // if still too few, again
    if ((range/step) < 4)
    {
        step /= 2;
        if (lower-step > start) lower -= step;
        if (upper+step < end) upper += step;
    }

    dc.SetPen( *wxBLACK_PEN );

    double current = lower;
    while (current < upper+(step/2))
    {
        int y = (int)((end-current) / range * (double)client_height) - 1;
        y -= (int)offset;
        if ((y > 20) && (y < client_height-7))
        {
            dc.DrawLine( client_width-15, y, client_width-7, y );
            wxString label;
            if (range < 50)
            {
                label.Printf( _T("%9.4f"), current );
                while (label.Last() == _T('0'))
                    label.RemoveLast();
                if ((label.Last() == _T('.')) || (label.Last() == _T(',')))
                    label.Append( _T('0') );
            }
            else
                label.Printf( _T("%d"), (int)floor(current) );
            dc.DrawText( label, 5, y-7 );
        }

        current += step;
    }

    dc.DrawLine( client_width-15, 6, client_width-15, client_height );
    dc.DrawLine( client_width-15, 2, client_width-20, 8 );
    dc.DrawLine( client_width-15, 2, client_width-10, 8 );
    
    wxString y_label = wxEmptyString;
    if (curve)
    {
      y_label  = curve->GetYLabel();
    }
    dc.DrawText( y_label, 0, 0 );
}

//-----------------------------------------------------------------------------
// wxPlotWindow
//-----------------------------------------------------------------------------

#define  ID_ENLARGE       1000
#define  ID_SHRINK        1002

#define  ID_MOVE_UP       1006
#define  ID_MOVE_DOWN     1007

#define  ID_ZOOM_IN       1010
#define  ID_ZOOM_OUT      1011


IMPLEMENT_DYNAMIC_CLASS(wxPlotWindow, wxPanel)

BEGIN_EVENT_TABLE(wxPlotWindow, wxPanel)
    EVT_BUTTON(  ID_MOVE_UP,     wxPlotWindow::OnMoveUp)
    EVT_BUTTON(  ID_MOVE_DOWN,   wxPlotWindow::OnMoveDown)

    EVT_BUTTON(  ID_ENLARGE,  wxPlotWindow::OnEnlarge)
    EVT_BUTTON(  ID_SHRINK,   wxPlotWindow::OnShrink)

    EVT_BUTTON(  ID_ZOOM_IN,     wxPlotWindow::OnZoomIn)
    EVT_BUTTON(  ID_ZOOM_OUT,    wxPlotWindow::OnZoomOut)
    EVT_PLOT_MOTION(  wxID_ANY,     wxPlotWindow::OnPlotAreaMotion)
END_EVENT_TABLE()

wxPlotWindow::wxPlotWindow( wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, int flag )
        : wxPanel( parent, id, pos, size, flag, _T("plotcanvas") )
{
    m_xUnitsPerValue = 1.0;
    m_xZoom = 1.0;
    old_x = -1;
    old_h = -1;
    old_w = -1;

    m_enlargeAroundWindowCentre = false;

    m_area = new wxPlotArea( this );
    wxBoxSizer *mainsizer = new wxBoxSizer( wxHORIZONTAL );

    if ((GetWindowStyleFlag() & wxPLOT_BUTTON_ALL) != 0)
    {
        wxBoxSizer *buttonlist = new wxBoxSizer( wxVERTICAL );
        if ((GetWindowStyleFlag() & wxPLOT_BUTTON_ENLARGE) != 0)
        {
            buttonlist->Add( new wxBitmapButton( this, ID_ENLARGE, *GetEnlargeBitmap() ), 0, wxEXPAND|wxALL, 2 );
            buttonlist->Add( new wxBitmapButton( this, ID_SHRINK, *GetShrinkBitmap() ), 0, wxEXPAND|wxALL, 2 );
            buttonlist->Add( 20,10, 0 );
        }
        if ((GetWindowStyleFlag() & wxPLOT_BUTTON_MOVE) != 0)
        {
            buttonlist->Add( new wxBitmapButton( this, ID_MOVE_UP, *GetUpBitmap() ), 0, wxEXPAND|wxALL, 2 );
            buttonlist->Add( new wxBitmapButton( this, ID_MOVE_DOWN, *GetDownBitmap() ), 0, wxEXPAND|wxALL, 2 );
            buttonlist->Add( 20,10, 0 );
        }
        if ((GetWindowStyleFlag() & wxPLOT_BUTTON_ZOOM) != 0)
        {
            buttonlist->Add( new wxBitmapButton( this, ID_ZOOM_IN, *GetZoomInBitmap() ), 0, wxEXPAND|wxALL, 2 );
            buttonlist->Add( new wxBitmapButton( this, ID_ZOOM_OUT, *GetZoomOutBitmap() ), 0, wxEXPAND|wxALL, 2 );
        }
        mainsizer->Add( buttonlist, 0, wxEXPAND|wxALL, 4 );
    }

    wxBoxSizer *plotsizer = new wxBoxSizer( wxHORIZONTAL );

    if ((GetWindowStyleFlag() & wxPLOT_Y_AXIS) != 0)
    {
        m_yaxis = new wxPlotYAxisArea( this );

        wxBoxSizer *vert1 = new wxBoxSizer( wxVERTICAL );
        plotsizer->Add( vert1, 0, wxEXPAND|wxTOP,10 );
        vert1->Add( m_yaxis, 1 );
        if ((GetWindowStyleFlag() & wxPLOT_X_AXIS) != 0)
            vert1->Add( 60, 40 );
    }
    else
    {
        m_yaxis = (wxPlotYAxisArea*) NULL;
    }

    if ((GetWindowStyleFlag() & wxPLOT_X_AXIS) != 0)
    {
        m_xaxis = new wxPlotXAxisArea( this );

        wxBoxSizer *vert2 = new wxBoxSizer( wxVERTICAL );
        plotsizer->Add( vert2, 5, wxEXPAND);
        vert2->Add( m_area, 1, wxEXPAND|wxTOP,10 );
        vert2->Add( m_xaxis, 0, wxEXPAND );
    }
    else
    {
        plotsizer->Add( m_area, 1, wxEXPAND );
        m_xaxis = (wxPlotXAxisArea*) NULL;
    }

    mainsizer->Add( plotsizer, 1, wxEXPAND );

    SetAutoLayout( true );
    SetSizer( mainsizer );
    mainsizer->Fit(this);
    mainsizer->SetSizeHints(this);
    Layout();

    //SetTargetWindow( m_area );

    SetBackgroundColour( *wxWHITE );

    m_current = (wxPlotCurve*) NULL;

    m_samerange = false;
}

wxPlotWindow::~wxPlotWindow()
{
}

void wxPlotWindow::Add( wxPlotCurve *curve )
{
    m_curves.Append( curve );
    if (!m_current) m_current = curve;

}

size_t wxPlotWindow::GetCount()
{
    return m_curves.GetCount();
}

wxPlotCurve *wxPlotWindow::GetAt( size_t n )
{
    wxList::compatibility_iterator node = m_curves.Item( n );
    if (!node)
        return (wxPlotCurve*) NULL;

    return (wxPlotCurve*) node->GetData();
}

void wxPlotWindow::SetCurrentCurve( wxPlotCurve* current )
{
    m_current = current;
    m_area->Refresh( true );
    m_area->Update();

    RedrawYAxis();

    wxPlotEvent event( wxEVT_PLOT_SEL_CHANGED, GetId() );
    event.SetEventObject( this );
    event.SetZoom( GetZoom() );
    event.SetCurve( m_current );
    GetEventHandler()->ProcessEvent( event );
}

void wxPlotWindow::Delete( wxPlotCurve* curve )
{
    wxList::compatibility_iterator node = m_curves.Find( curve );
    if (!node) return;

    m_curves.DeleteObject( curve );

    m_area->DeleteCurve( curve );
    m_area->Refresh( true );

    if (curve == m_current) m_current = (wxPlotCurve *) NULL;
}

wxPlotCurve *wxPlotWindow::GetCurrentCurve()
{
    return m_current;
}

void wxPlotWindow::Add( wxPlotOnOffCurve *curve )
{
    m_onOffCurves.Append( curve );
}

void wxPlotWindow::Delete( wxPlotOnOffCurve* curve )
{
    wxList::compatibility_iterator node = m_onOffCurves.Find( curve );
    if (!node) return;

    m_onOffCurves.DeleteObject( curve );
}

size_t wxPlotWindow::GetOnOffCurveCount()
{
    return m_onOffCurves.GetCount();
}

wxPlotOnOffCurve *wxPlotWindow::GetOnOffCurveAt( size_t n )
{
    wxList::compatibility_iterator node = m_onOffCurves.Item( n );
    if (!node)
        return (wxPlotOnOffCurve*) NULL;

    return (wxPlotOnOffCurve*) node->GetData();
}

void wxPlotWindow::Move( wxPlotCurve* curve, int pixels_up )
{
    m_area->DeleteCurve( curve );

    curve->SetOffsetY( curve->GetOffsetY() + pixels_up );

    m_area->Refresh( true );

    RedrawYAxis();
}

void wxPlotWindow::OnMoveUp( wxCommandEvent& WXUNUSED(event) )
{
    if (!m_current) return;

    Move( m_current, 25 );
}

void wxPlotWindow::OnMoveDown( wxCommandEvent& WXUNUSED(event) )
{
    if (!m_current) return;

    Move( m_current, -25 );
}

void wxPlotWindow::Enlarge( wxPlotCurve *curve, double factor )
{
    m_area->DeleteCurve( curve );

    int client_width;
    int client_height;
    m_area->GetClientSize( &client_width, &client_height);
    double offset = (double)curve->GetOffsetY() / (double)client_height;

    double range = curve->GetEndY() - curve->GetStartY();
    offset *= range;

    double new_range = range / factor;
    double new_offset = offset / factor;

    if (m_enlargeAroundWindowCentre)
    {
        double middle = curve->GetStartY() - offset + range/2;

        curve->SetStartY( middle - new_range / 2 + new_offset );
        curve->SetEndY( middle + new_range / 2 + new_offset  );
    }
    else
    {
        curve->SetStartY( (curve->GetStartY() - offset)/factor + new_offset );
        curve->SetEndY( (curve->GetEndY() - offset)/factor + new_offset );
    }

    m_area->Refresh( true );
    RedrawYAxis();
}

void wxPlotWindow::SetUnitsPerValue( double upv )
{
    m_xUnitsPerValue = upv;

    RedrawXAxis();
}

void wxPlotWindow::SetZoom( double zoom )
{
    double old_zoom = m_xZoom;
    m_xZoom = 1;  //warning

    int view_x = 0;
    int view_y = 0;
    //GetViewStart( &view_x, &view_y );

    wxInt32 max = 0;
    wxList::compatibility_iterator node = m_curves.GetFirst();
    while (node)
    {
        wxPlotCurve *curve = (wxPlotCurve*) node->GetData();
        if (curve->GetEndX() > max)
            max = curve->GetEndX();
        node = node->GetNext();
    }

    RedrawXAxis();
    m_area->Refresh( true );
}

void wxPlotWindow::RedrawXAxis()
{
    if (m_xaxis)
        m_xaxis->Refresh( true );
}

void wxPlotWindow::RedrawYAxis()
{
    if (m_yaxis)
       m_yaxis->Refresh( true );
}

void wxPlotWindow::RedrawEverything()
{
    if (m_xaxis)
        m_xaxis->Refresh( true );
    if (m_yaxis)
        m_yaxis->Refresh( true );
    m_area->Refresh( true );
}

void wxPlotWindow::OnZoomIn( wxCommandEvent& WXUNUSED(event) )
{
    SetZoom( m_xZoom * 1.5 );
}

void wxPlotWindow::OnZoomOut( wxCommandEvent& WXUNUSED(event) )
{
    SetZoom( m_xZoom * 0.6666 );
}

void wxPlotWindow::OnEnlarge( wxCommandEvent& WXUNUSED(event) )
{
    if (!m_current) return;

    Enlarge( m_current, 1.5 );
}

void wxPlotWindow::OnShrink( wxCommandEvent& WXUNUSED(event) )
{
    if (!m_current) return;

    Enlarge( m_current, 0.6666666 );
}

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

// FIXME MT-UNSAFE
static wxBitmap *GetEnlargeBitmap()
{
    static wxBitmap* s_bitmap = (wxBitmap *) NULL;
    static bool s_loaded = false;

    if ( !s_loaded )
    {
        s_loaded = true; // set it to true anyhow, we won't try again

        #if defined(__WXMSW__) || defined(__WXPM__)
            s_bitmap = new wxBitmap(_T("plot_enl_bmp"), wxBITMAP_TYPE_RESOURCE);
        #else
            s_bitmap = new wxBitmap( plot_enl_xpm );
        #endif
    }

    return s_bitmap;
}

static wxBitmap *GetShrinkBitmap()
{
    static wxBitmap* s_bitmap = (wxBitmap *) NULL;
    static bool s_loaded = false;

    if ( !s_loaded )
    {
        s_loaded = true; // set it to true anyhow, we won't try again

        #if defined(__WXMSW__) || defined(__WXPM__)
            s_bitmap = new wxBitmap(_T("plot_shr_bmp"), wxBITMAP_TYPE_RESOURCE);
        #else
            s_bitmap = new wxBitmap( plot_shr_xpm );
        #endif
    }

    return s_bitmap;
}

static wxBitmap *GetZoomInBitmap()
{
    static wxBitmap* s_bitmap = (wxBitmap *) NULL;
    static bool s_loaded = false;

    if ( !s_loaded )
    {
        s_loaded = true; // set it to true anyhow, we won't try again

        #if defined(__WXMSW__) || defined(__WXPM__)
            s_bitmap = new wxBitmap(_T("plot_zin_bmp"), wxBITMAP_TYPE_RESOURCE);
        #else
            s_bitmap = new wxBitmap( plot_zin_xpm );
        #endif
    }

    return s_bitmap;
}

static wxBitmap *GetZoomOutBitmap()
{
    static wxBitmap* s_bitmap = (wxBitmap *) NULL;
    static bool s_loaded = false;

    if ( !s_loaded )
    {
        s_loaded = true; // set it to true anyhow, we won't try again

        #if defined(__WXMSW__) || defined(__WXPM__)
            s_bitmap = new wxBitmap(_T("plot_zot_bmp"), wxBITMAP_TYPE_RESOURCE);
        #else
            s_bitmap = new wxBitmap( plot_zot_xpm );
        #endif
    }

    return s_bitmap;
}

static wxBitmap *GetUpBitmap()
{
    static wxBitmap* s_bitmap = (wxBitmap *) NULL;
    static bool s_loaded = false;

    if ( !s_loaded )
    {
        s_loaded = true; // set it to true anyhow, we won't try again

        #if defined(__WXMSW__) || defined(__WXPM__)
            s_bitmap = new wxBitmap(_T("plot_up_bmp"), wxBITMAP_TYPE_RESOURCE);
        #else
            s_bitmap = new wxBitmap( plot_up_xpm );
        #endif
    }

    return s_bitmap;
}

static wxBitmap *GetDownBitmap()
{
    static wxBitmap* s_bitmap = (wxBitmap *) NULL;
    static bool s_loaded = false;

    if ( !s_loaded )
    {
        s_loaded = true; // set it to true anyhow, we won't try again

        #if defined(__WXMSW__) || defined(__WXPM__)
            s_bitmap = new wxBitmap(_T("plot_dwn_bmp"), wxBITMAP_TYPE_RESOURCE);
        #else
            s_bitmap = new wxBitmap( plot_dwn_xpm );
        #endif
    }

    return s_bitmap;
}

