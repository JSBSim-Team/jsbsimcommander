/////////////////////////////////////////////////////////////////////////////
// Name:        MyApp.cpp
// Purpose:     The App of Commander for JSBSim
// Author:      Matthew Gong
// Created:     05/25/2006
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

#include <wx/tooltip.h>
#include <wx/splash.h>
#include <wx/cmdline.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/xrc/xmlres.h>          // XRC XML resouces
#include <wx/image.h>               // wxImage

#include "MyApp.h"
#include "MyFrame.h"
#include "Property_Diag.h"
#include "EngineMgr_Diag.h"

#include "initprops.h"

/*
 * A macro needed for some compilers (AIX) that need 'main' to be defined
 * in the application itself.
 */

IMPLEMENT_APP (MyApp)

/**
* MyApp ========================================================================
*/

MyApp::MyApp (void)
{
}

/**
 * OnInit ======================================================================
 * The `main program' equivalent, creating the main frame
 */

bool MyApp::OnInit (void)
{
  Delimited = wxFileName::GetPathSeparator();

  if (!wxApp::OnInit ())
     return false;

  wxBitmap about;
  if ( about.LoadFile(about_img_file, wxBITMAP_TYPE_BMP))
  {
    wxSplashScreen* splash = new wxSplashScreen(about,
          wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
          3000, NULL, -1, wxDefaultPosition, wxDefaultSize,
          wxSIMPLE_BORDER|wxSTAY_ON_TOP);
  }

  m_locale.Init(wxLANGUAGE_DEFAULT);
  // Initialize the catalogs we'll be using
  m_locale.AddCatalog(wxT("Commander"));

  InitProps(source_property, source_desc, source_unit, source_access, source_comment, destination_property, destination_desc, destination_unit, destination_access, destination_comment, all_property, all_desc, all_unit, all_access, all_comment);

  // If there is any of a certain format of image in the xrcs, then first
  // load a handler for that image type. This example uses XPMs, but if
  // you want PNGs, then add a PNG handler, etc. See wxImage::AddHandler()
  // documentation for the types of image handlers available.
  //wxImage::AddHandler(new wxXPMHandler);
  ::wxInitAllImageHandlers();

  // Initialize all the XRC handlers. Always required (unless you feel like
  // going through and initializing a handler of each control type you will
  // be using (ie initialize the spinctrl handler, initialize the textctrl
  // handler). However, if you are only using a few control types, it will
  // save some space to only initialize the ones you will be using. See
  // wxXRC docs for details.
  wxXmlResource::Get()->InitAllHandlers();

  // Load all of the XRC files that will be used. You can put everything
  // into one giant XRC file if you wanted, but then they become more
  // diffcult to manage, and harder to reuse in later projects.
  // The menubar
  wxString filename;
  bool cont = rc_dir.GetFirst(&filename, wxT("*.xrc"), wxDIR_FILES);
  if (!cont)
  {
    wxMessageBox(_("Fail to load XRC files. Please use --resource=<str> or -r <str> to specify the resource directory."), _("Error"), wxOK | wxICON_ERROR);
    return false;
  }
  while ( cont )
  {
	wxXmlResource::Get()->Load(
	  rc_dir.GetName()+wxFileName::GetPathSeparator()+filename);
      cont = rc_dir.GetNext(&filename);
  }

  InitDir();
  InitCfg();

  // Make an instance of your derived frame. Passing NULL (the default value
  // of MyFrame's constructor is NULL) as the frame doesn't have a parent
  // since it is the main application window.
  frame = new MyFrame();

  property_dialog = new PropertyDialog(frame, -1, _("Properties"), wxDefaultPosition, wxSize(825,580));
  {
    property_dialog->Load(all_property, all_desc, all_unit, all_access, all_comment);
  }

  wxToolTip::Enable(true);
  wxToolTip::SetDelay(300);

  SetTopWindow(frame);
  frame->Centre (wxBOTH);
  frame->Maximize(true);

  // Show the frame as it's created initially hidden.
  frame->Show(true);

  // Return true to tell program to continue (false would terminate).
  return true;
}

/**
* OnExit =======================================================================
*/

int MyApp::OnExit (void)
{
  return 0;
}

/**
* OnInitCmdLine ================================================================
*/

void MyApp::OnInitCmdLine(wxCmdLineParser& parser)
{
  CurrentWorkDirectory = argv[0];
  CurrentWorkDirectory = CurrentWorkDirectory.BeforeLast(wxFileName::GetPathSeparator());
  wxSetWorkingDirectory(CurrentWorkDirectory);
    static const wxCmdLineEntryDesc cmdLineDesc[] =
    {
        { wxCMD_LINE_SWITCH, _T("h"), _T("help"), _T("show this help message"),
            wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
        { wxCMD_LINE_SWITCH, _T("v"), _T("verbose"), _T("be verbose") },
        { wxCMD_LINE_SWITCH, _T("q"), _T("quiet"),   _T("be quiet") },
        { wxCMD_LINE_OPTION, _T("r"), _T("resource"), _T("resource dir") },
	{ wxCMD_LINE_OPTION, _T("a"), _T("about"), _T("about image file") },

        { wxCMD_LINE_NONE }
    };
    parser.SetDesc(cmdLineDesc);
}

/**
* OnCmdLineParsed ==============================================================
*/

bool MyApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
  if (wxApp::OnCmdLineParsed(parser))
  {
    wxString dir;
    if (parser.Found("resource", &dir))
    {
      rc_dir.Open(dir);
    }
    else
    {
      rc_dir.Open(CurrentWorkDirectory+wxFileName::GetPathSeparator()+wxT("rc"));
    }
    if (parser.Found("about", &dir))
    {
      about_img_file = dir;
    }
    else
    {
      about_img_file = CurrentWorkDirectory+wxFileName::GetPathSeparator()+wxT("about.bmp");
    }
    return true;
  }
  else
  {
    return false;
  }
}

/**
* InitDir ======================================================================
*/

void MyApp::InitDir (void)
{
  engine_dir = CurrentWorkDirectory + Delimited + wxT("engine");
  if (! ::wxDirExists(engine_dir))
  {
    ::wxMkdir(engine_dir);
  }
}

/**
* InitCfg ======================================================================
*/

void MyApp::InitCfg (void)
{
  wxString ini = CurrentWorkDirectory + Delimited + wxT("EngineMgr.ini");
  EngineMgrDialog::InitEngineCfg(); 
}

/**
* GetMainFrame =================================================================
*/

MyFrame *GetMainFrame (void)
{
  return wxGetApp().frame;
}

/**
* GetPropertyDialog ============================================================
*/

PropertyDialog *GetPropertyDialog (void)
{
  return wxGetApp().property_dialog;
}

