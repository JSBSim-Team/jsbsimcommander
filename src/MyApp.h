/////////////////////////////////////////////////////////////////////////////
// Name:        MyApp.h
// Purpose:     The App for JSBSim Commander
// Author:      Matthew Gong
// Created:     05/25/2006
// Copyright:   (c) Matthew Gong
// Licence:     GPL licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
// #pragma interface
#endif

#ifndef _MYAPP_H_
#define _MYAPP_H_


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include <wx/app.h>             // wxApp
#include <wx/dir.h>
#include <string>

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
DEFINITIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
FORWARD DECLARATIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

class MyFrame;
class AircraftDialog;
class PropertyDialog;

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DOCUMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/** The MyApp class represents the application itself.
    It is used to:
    set and get application-wide properties; 
    implement the windowing system message or event loop; 
    initiate application processing via wxApp::OnInit; 
    allow default processing of events not handled by other objects in the application. 

    @author Matthew Gong
*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DECLARATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

class MyApp:public wxApp
{
public:
   /**
    * A constructor.
    * A more elaborate description of the constructor.
    */
  MyApp (void);

  /** Programe starts from here.
   * it will usually create the application's main window, 
   * optionally calling wxApp::SetTopWindow. 
   * You may use OnExit to clean up anything initialized here, 
   * provided that the function returns true.
   * @return return true if initialize itself successful
   */
  bool OnInit (void);

  /** Command Line Parser starts from here.
    * Called from OnInit and may be used to initialize the parser 
    * with the command line options for this application. 
    *
    * @see OnInit(void)
    * @param parser The default command line parser.
    */
  void OnInitCmdLine(wxCmdLineParser& parser);

  /** Called after the command line had been successfully parsed.
    *
    * @see OnInitCmdLine(wxCmdLineParser& parser)
    * @param parser The initialized command line parser.
    * @return Return true to continue normal execution 
    * or false to return false from OnInit thus terminating the program.
    */
  bool OnCmdLineParsed(wxCmdLineParser& parser);

  /** Programe ends from here.
   * OnExit is called after destroying all application windows and controls, 
   * but before wxWidgets cleanup. 
   * Note that it is not called at all if OnInit failed.
   *
   * @see OnInit()
   * @return return true if initialize itself successful
   */
  int OnExit (void);

  MyFrame * frame;
  AircraftDialog * dlg;

  PropertyDialog * property_dialog;

  wxString CurrentWorkDirectory;
  wxString AppDirectory;
  wxString aircraft_dir;
  wxString engine_dir;
  wxString script_dir;
  wxString result_dir;
  wxString prop_file;

  wxString Delimited;

protected:
  wxLocale m_locale; // locale we'll be using
  void InitDir();
  void InitCfg();
  wxString inputfile;
  wxString about_img_file;
  wxDir rc_dir;
};

DECLARE_APP (MyApp)

extern MyFrame *GetMainFrame (void);
extern PropertyDialog *GetPropertyDialog (void);

inline wxString std2wxstr(const std::string &str){return wxString(str.c_str(), wxConvUTF8);}

inline wxString c2wxstr(const char *str){return wxString(str, wxConvUTF8);}

inline std::string wx2stdstr(const wxString &str){ return (const char *)(str.mb_str(wxConvUTF8)); }

#endif  // _MYAPP_H_
