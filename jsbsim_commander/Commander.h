/////////////////////////////////////////////////////////////////////////////
// Name:        Commander.h
// Purpose:     JSBSim Commander
// Author:      Matthew Gong
// Created:     05/03/2005
// Copyright:   (c) Matthew Gong
// Licence:     GPL licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
// #pragma interface
#endif

#ifndef _COMMANDER_H_
#define _COMMANDER_H_

// Define a new application
class AircraftDialog;
class PropertyDialog;
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

  AircraftDialog * dlg;

  wxArrayString source_property;
  wxArrayString source_desc;
  wxArrayString source_unit;
  wxArrayString source_access;
  wxArrayString source_comment;
  
  wxArrayString destination_property;
  wxArrayString destination_desc;
  wxArrayString destination_unit;
  wxArrayString destination_access;
  wxArrayString destination_comment;

  wxArrayString all_property;
  wxArrayString all_desc;
  wxArrayString all_unit;
  wxArrayString all_access;
  wxArrayString all_comment;

  PropertyDialog * property_dialog;

  wxString CurrentWorkDirectory;
  wxString aircraft_dir;
  wxString engine_dir;
  wxString script_dir;
  wxString result_dir;

  wxString Delimited;

protected:
  wxLocale m_locale; // locale we'll be using
  void InitDir();
  void InitCfg();
  wxString inputfile;
};

DECLARE_APP (MyApp)

extern AircraftDialog *GetMainDlg (void);
extern PropertyDialog *GetPropertyDialog (void);

#endif  // _COMMANDER_H_
