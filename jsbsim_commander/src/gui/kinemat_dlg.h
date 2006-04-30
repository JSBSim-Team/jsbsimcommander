/////////////////////////////////////////////////////////////////////////////
// Name:        kinemat_dlg.h
// Purpose:     dialog for FGKinemat
// Author:      Matthew Gong
// Created:     02/05/2005
// Copyright:   (c) Matthew Gong
// Licence:     GPL licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
// #pragma interface
#endif

#include <wx/wx.h>
#include <wx/image.h>
// begin wxGlade: ::dependencies
#include <wx/notebook.h>
#include <wx/grid.h>
// end wxGlade


#ifndef KINEMAT_DLG_H
#define KINEMAT_DLG_H

class Kinemat;
class KinematPropertyDialog: public wxDialog {
public:
  // begin wxGlade: KinematPropertyDialog::ids
  // begin wxGlade: GainPropertyDialog::ids
  enum
    {
      ID_CLIPABLE = 700,
    };
  // end wxGlade
  // end wxGlade

  KinematPropertyDialog(Kinemat * componment, wxWindow* parent, int id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxSize(250, 510), long style=wxDEFAULT_DIALOG_STYLE);

private:
  // begin wxGlade: KinematPropertyDialog::methods
  void set_properties();
  void do_layout();
  // end wxGlade

  wxString name;
  wxString order;
  bool clipable;
  wxString clipmax;
  wxString clipmin;
  int input1;
  
  bool isscale;

protected:
  // begin wxGlade: KinematPropertyDialog::attributes
  wxStaticBox* sizer_8_staticbox;
  wxStaticBox* sizer_7_staticbox;
  wxStaticText* label_name;
  wxStaticText* label_Type;
  wxStaticText* label_order;
  wxTextCtrl* text_ctrl_name;
  wxStaticText* label_type;
  wxTextCtrl* text_ctrl_order;
  wxCheckBox* checkbox_clipable;
  wxStaticText* label_max;
  wxTextCtrl* text_ctrl_clipmax;
  wxStaticText* label_min;
  wxTextCtrl* text_ctrl_clipmin;
  wxRadioBox* radio_box_input1;
  wxPanel* notebook_Main_pane_basic;
  wxGrid* grid_kinemat;
  wxCheckBox* checkbox_isscale;
  wxPanel* notebook_Main_pane_kinemat;
  wxNotebook* notebook_Main_pane;
  wxButton* button_OK;
  wxButton* button_Cancel;
  // end wxGlade

 public:
  void OnClipable (wxCommandEvent & event);

  bool Show( bool show = TRUE );
  void OnOK( wxCommandEvent &event );

  void GetDataIn(Kinemat * k);
  void SetDataOut(Kinemat * k);

  DECLARE_EVENT_TABLE ()

};


#endif // KINEMAT_DLG_H
