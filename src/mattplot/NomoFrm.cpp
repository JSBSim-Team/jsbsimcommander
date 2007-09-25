/////////////////////////////////////////////////////////////////////////////
// Name:        PropDlg.cpp
// Purpose:     list of properties
// Author:      Matthew Gong
// Created:     02/26/2006
// Copyright:   (c) Matthew Gong
// Licence:     GPL licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
 #pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/xrc/xmlres.h>

#include "NomoFrm.h"
#include "MyApp.h"
#include "mattplot/plot.h"

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------

// The event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
// The reason why the menuitems and tools are given the same name in the
// XRC file, is that both a tool (a toolbar item) and a menuitem are designed
// to fire the same kind of event (an EVT_MENU) and thus I give them the same
// ID name to help new users emphasize this point which is often overlooked
// when starting out with wxWidgets.
BEGIN_EVENT_TABLE(NomographFrame, wxFrame)
  EVT_NOTEBOOK_PAGE_CHANGING(XRCID("notebook"), NomographFrame::OnPageChanging) 
  EVT_MENU(wxID_SAVE, NomographFrame::OnSave)
  EVT_MENU(wxID_SAVEAS, NomographFrame::OnSaveAs)
END_EVENT_TABLE()

//-----------------------------------------------------------------------------
// Class implementation
//-----------------------------------------------------------------------------

NomographFrame::NomographFrame(wxWindow* parent, const wxString & inputfile)
{
  wxXmlResource::Get()->LoadFrame(this, parent, wxT("plot_frame"));
    text_ctrl_xml = XRCCTRL(*this, "text_xml", wxTextCtrl);
    notbook = XRCCTRL(*this, "notebook", wxNotebook); 


    // Load the menubar from XRC and set this frame's menubar to it.
    SetMenuBar(wxXmlResource::Get()->LoadMenuBar(wxT("plot_main_menu")));
    // Load the toolbar from XRC and set this frame's toolbar to it.
    // NOTE: For toolbars you currently should do it exactly like this.
    // With toolbars, you currently can't create one, and set it later. It
    // needs to be all in one step.
    //frame->SetToolBar(wxXmlResource::Get()->LoadToolBar(frame, wxT("plot_main_toolbar")));

#if wxUSE_STATUSBAR
    // Give the frame an optional statusbar. The '1' just means one field.
    // A gripsizer will automatically get put on into the corner, if that
    // is the normal OS behaviour for frames on that platform. Helptext
    // for menu items and toolbar tools will automatically get displayed
    // here.
    CreateStatusBar( 1 );
#endif // wxUSE_STATUSBAR

  //push event handler
  handler = new PlotHandler (this);
  PushEventHandler (handler);

  canvas = new PlotCanvas(this, wxID_ANY, handler, wxDefaultPosition, wxSize(800,600));
  wxXmlResource::Get()->AttachUnknownControl(wxT("canvas"), canvas, this);

  if (!inputfile.IsEmpty())
  {
    handler->Load(inputfile);
  }
}

void NomographFrame::OnPageChanging(wxNotebookEvent &event)
{
  if (event.GetOldSelection() == 1)
  {
    if (!text_ctrl_xml->IsModified())
      return;
    if (!text_ctrl_xml->SaveFile(handler->GetFileName()))
    {
      event.Veto();
    }
    if (!handler->Load(handler->GetFileName()))
    {
      event.Veto();
    }
  }
  else if (event.GetOldSelection() == 0)
  {
    if (buff != handler->GetFileName())
    {
      buff = handler->GetFileName();
      text_ctrl_xml->Clear();
      if (!text_ctrl_xml->LoadFile(buff))
      {
        event.Veto();
      }
    }
  }
}

void NomographFrame::OnSave(wxCommandEvent& event)
{
  buff.Empty();
  event.Skip();
  if (notbook->GetSelection() == 1)
  {
    if (!text_ctrl_xml->IsModified())
      return;
    if (!text_ctrl_xml->SaveFile(handler->GetFileName()))
    {
    }
    if (!handler->Load(handler->GetFileName()))
    {
    }
    buff = handler->GetFileName();
  }
}

void NomographFrame::OnSaveAs(wxCommandEvent& event)
{
  buff.Empty();
  event.Skip();
}

