/////////////////////////////////////////////////////////////////////////////
// Name:        Commander.cpp
// Purpose:     Commander for JSBSim
// Author:      Matthew Gong
// Created:     05/03/2005
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

#include "Commander.h"
#include "AircraftDialog.h"
#include "Property_Diag.h"
#include "EngineMgr_Diag.h"

#include "initprops.h"
/*
 * A macro needed for some compilers (AIX) that need 'main' to be defined
 * in the application itself.
 */

IMPLEMENT_APP (MyApp)


MyApp::MyApp (void)
{
}

/*
 * The `main program' equivalent, creating the windows and returning the
 * main frame
 */

bool
MyApp::OnInit (void)
{
  Delimited = wxFileName::GetPathSeparator();
  if (!wxApp::OnInit ())
     return false;

  wxBitmap about;
  if ( about.LoadFile(wxT("about.bmp"), wxBITMAP_TYPE_BMP))
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
  InitDir();
  InitCfg();

  //// Create the main frame window
  dlg = new AircraftDialog (NULL, -1);

  //// Give it an icon
  dlg->SetIcon (wxICON (ogl));

  property_dialog = new PropertyDialog(dlg, -1, _("Properties"), wxDefaultPosition, wxSize(825,580));
  {
    property_dialog->Load(all_property, all_desc, all_unit, all_access, all_comment);
  }

  wxToolTip::Enable(true);
  wxToolTip::SetDelay(300);

  if (!inputfile.IsEmpty() && ! dlg->Load(inputfile))
  {
      ::wxMessageBox(wxString(_("Something is wrong! I can not load proper file(")) + wxT(")."), _("Warning"), wxOK | wxICON_INFORMATION, NULL);  
  }
  else
  {
    dlg->Maximize(true);
    if (dlg->ShowModal () == wxID_OK)
    {
      dlg->Save();
    }
    property_dialog->Destroy();
    dlg->Destroy();
  }

  return true;
}

int
MyApp::OnExit (void)
{
  return 0;
}


void
MyApp::OnInitCmdLine(wxCmdLineParser& parser)
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
        /*
        { wxCMD_LINE_OPTION, _T("o"), _T("output"),  _T("output file") },
        { wxCMD_LINE_OPTION, _T("i"), _T("input"),   _T("input dir") },
        { wxCMD_LINE_OPTION, _T("s"), _T("size"),    _T("output block size"),
            wxCMD_LINE_VAL_NUMBER },
        { wxCMD_LINE_OPTION, _T("d"), _T("date"),    _T("output file date"),
            wxCMD_LINE_VAL_DATE },
        */

        { wxCMD_LINE_PARAM,  NULL, NULL, _T("input file"),
            wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE },

        { wxCMD_LINE_NONE }
    };
    parser.SetDesc(cmdLineDesc);
    /*
    parser.AddOption(_T("project_name"),_T(""),_T("full path to project file"),
                     wxCMD_LINE_VAL_STRING,
                     wxCMD_LINE_NEEDS_SEPARATOR);
    */
}

bool MyApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
  if (wxApp::OnCmdLineParsed(parser))
  {
    size_t count = parser.GetParamCount();
    if (count > 0u)
    {
      inputfile = parser.GetParam(0u);
    }
    return true;
  }
  else
  {
    return false;
  }
}    

void
MyApp::InitDir (void)
{

  aircraft_dir = CurrentWorkDirectory + Delimited + wxT("aircraft");
  if (! ::wxDirExists(aircraft_dir))
  {
    ::wxMkdir(aircraft_dir);
  }

  engine_dir = CurrentWorkDirectory + Delimited + wxT("engine");
  if (! ::wxDirExists(engine_dir))
  {
    ::wxMkdir(engine_dir);
  }

  script_dir = CurrentWorkDirectory + Delimited + wxT("scripts");
  if (! ::wxDirExists(script_dir))
  {
    ::wxMkdir(script_dir);
  }

  result_dir = CurrentWorkDirectory + Delimited + wxT("results");
  if (! ::wxDirExists(result_dir))
  {
    ::wxMkdir(result_dir);
  }

}


void
MyApp::InitCfg (void)
{
  wxString ini = CurrentWorkDirectory + Delimited + wxT("EngineMgr.ini");
  EngineMgrDialog::InitEngineCfg(); 
}

AircraftDialog *
GetMainDlg (void)
{
  return wxGetApp().dlg;
}

PropertyDialog *
GetPropertyDialog (void)
{
  return wxGetApp().property_dialog;
}

