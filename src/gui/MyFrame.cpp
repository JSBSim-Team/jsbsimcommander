//-----------------------------------------------------------------------------
// Name:        MyFrame.cpp
// Purpose:     A derived frame, called MyFrame
// Author:      Matthew Gong
// Licence:     wxWindows licence
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// GCC implementation
//-----------------------------------------------------------------------------

#ifdef __GNUG__
//    #pragma implementation "MyFrame.h"
#endif

//-----------------------------------------------------------------------------
// Standard wxWidgets headers
//-----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// For all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

//-----------------------------------------------------------------------------
// Header of this .cpp file
//-----------------------------------------------------------------------------

#include "MyFrame.h"

//-----------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//-----------------------------------------------------------------------------

// Since setting an icon
#include <wx/image.h>

//-----------------------------------------------------------------------------

#include <wx/xrc/xmlres.h>              // XRC XML resouces

//-----------------------------------------------------------------------------

#include "MyApp.h"
#include "Property_Diag.h"
#include "AircraftDialog.h"
#include "ResultShow_Diag.h"

//-----------------------------------------------------------------------------
// Regular resources (the non-XRC kind).
//-----------------------------------------------------------------------------

// The application icon
// All non-MSW platforms use an xpm. MSW uses an .ico file
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__)
    #include "bitmaps/ogl.xpm"
#endif

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
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(XRCID("aircraft_tool_or_menuitem"), MyFrame::OnAircraftManagerToolOrMenuCommand)
    EVT_MENU(XRCID("result_tool_or_menuitem"), MyFrame::OnResultManagerToolOrMenuCommand)
    EVT_MENU(XRCID("property_tool_or_menuitem"), MyFrame::OnPropertyManagerToolOrMenuCommand)
    EVT_MENU(wxID_EXIT,  MyFrame::OnExitToolOrMenuCommand)
END_EVENT_TABLE()

//-----------------------------------------------------------------------------
// Public methods
//-----------------------------------------------------------------------------

/**
* MyFrame ======================================================================
* Constructor
*/

MyFrame::MyFrame(wxWindow* parent)
{
    // Load up this frame from XRC. [Note, instead of making a class's
    // constructor take a wxWindow* parent with a default value of NULL,
    // we could have just had designed MyFrame class with an empty
    // constructor and then written here:
    // wxXmlResource::Get()->LoadFrame(this, (wxWindow* )NULL, "main_frame");
    // since this frame will always be the top window, and thus parentless.
    // However, the current approach has source code that can be recycled
    // for other frames that aren't the top level window.]
    wxXmlResource::Get()->LoadFrame(this, parent, wxT("main_frame"));

    // Set the icon for the frame.
    SetIcon(wxICON(ogl));

    // Load the menubar from XRC and set this frame's menubar to it.
    SetMenuBar(wxXmlResource::Get()->LoadMenuBar(wxT("main_menu")));
    // Load the toolbar from XRC and set this frame's toolbar to it.
    // NOTE: For toolbars you currently should do it exactly like this.
    // With toolbars, you currently can't create one, and set it later. It
    // needs to be all in one step.
    SetToolBar(wxXmlResource::Get()->LoadToolBar(this, wxT("main_toolbar")));

#if wxUSE_STATUSBAR
    // Give the frame an optional statusbar. The '1' just means one field.
    // A gripsizer will automatically get put on into the corner, if that
    // is the normal OS behaviour for frames on that platform. Helptext
    // for menu items and toolbar tools will automatically get displayed
    // here.
    CreateStatusBar( 1 );
#endif // wxUSE_STATUSBAR
}

//-----------------------------------------------------------------------------
// Private methods
//-----------------------------------------------------------------------------


/**
* OnExitToolOrMenuCommand ======================================================
*/

void MyFrame::OnExitToolOrMenuCommand(wxCommandEvent& WXUNUSED(event))
{
  // true is to force the frame to close.
  Close(true);
}

/**
* OnAboutToolOrMenuCommand =====================================================
*/

void MyFrame::OnAboutToolOrMenuCommand(wxCommandEvent& WXUNUSED(event))
{
/*******************************************************************************
    wxDialog dlg;
    wxXmlResource::Get()->LoadDialog(&dlg, this, wxT("about_dialog"));
    XRCCTRL(dlg, "package_string", wxStaticText)->SetLabel(wxT(PACKAGE_STRING));
    XRCCTRL(dlg, "package_bugreport", wxStaticText)->SetLabel(wxT(PACKAGE_BUGREPORT));
    dlg.ShowModal();
*******************************************************************************/
}

/**
* OnAircraftManagerToolOrMenuCommand ===========================================
*/

void MyFrame::OnAircraftManagerToolOrMenuCommand(wxCommandEvent& event)
{
  wxString filename = wxFileSelector(_("Choose a file to open"), wxEmptyString, wxEmptyString, wxT("xml"), _("XML files (*.xml)|*.xml"), wxOPEN|wxFILE_MUST_EXIST, this);
  if ( !filename.empty())
  {
    AircraftDialog * dlg = new AircraftDialog (this, -1);
    if (dlg->Load(filename))
    {
      if (dlg->ShowModal () == wxID_OK)
      {
        dlg->Save();
      }
    }
    else
    {
      ::wxMessageBox(wxString(_("Something is wrong! I can not load proper file(")) + filename + wxT(")."), _("Warning"), wxOK | wxICON_INFORMATION, NULL);    
    }
    dlg->Destroy();
  }
}

/**
* OnResultManagerToolOrMenuCommand =============================================
* Show the curves from a CSV file.
*/

void MyFrame::OnResultManagerToolOrMenuCommand(wxCommandEvent& event)
{
  wxString filename = wxFileSelector(_("Choose a file to open"), wxEmptyString, wxEmptyString, wxT("csv"), _("CSV files (*.csv)|*.csv"), wxOPEN|wxFILE_MUST_EXIST, this);
  if ( !filename.empty())
  {
    ResultShowDialog * dlg = new ResultShowDialog (this, -1);
    if (dlg->Load(filename))
    {
      dlg->ShowModal();
    }
    else
    {
      ::wxMessageBox(wxString(_("Something is wrong! I can not load proper file(")) + filename + wxT(")."), _("Warning"), wxOK | wxICON_INFORMATION, NULL);    
    }
    dlg->Destroy();
  }
}

/**
* OnRunScriptToolOrMenuCommand =================================================
*/

void MyFrame::OnRunScriptToolOrMenuCommand(wxCommandEvent& event)
{
}

/**
* OnPropertyManagerToolOrMenuCommand ===========================================
*/

void MyFrame::OnPropertyManagerToolOrMenuCommand(wxCommandEvent& event)
{
    GetPropertyDialog()->ShowModal();
}


