/////////////////////////////////////////////////////////////////////////////
// Name:        gain_dlg.h
// Purpose:     dialog for FGGain
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


#ifndef GAIN_DLG_H
#define GAIN_DLG_H

class Gain;
class GainPropertyDialog: public wxDialog 
{
 public:
  // begin wxGlade: GainPropertyDialog::ids
  enum
    {
      ID_TYPE = 700,
      ID_CLIPABLE,
    };
  // end wxGlade

  GainPropertyDialog(Gain * componment, const wxArrayString & source, wxWindow* parent, int id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxSize(380, 550), long style=wxDEFAULT_DIALOG_STYLE);

 private:
  // begin wxGlade: GainPropertyDialog::methods
  void set_properties();
  void do_layout();
  // end wxGlade

  wxString name;
  wxString type;
  wxString order;
  bool clipable;
  wxString clipmax;
  wxString clipmin;
  int input1;
  
  wxString gain;

  wxString as_max;
  wxString as_min;

  wxString indep;

 protected:
  // begin wxGlade: GainPropertyDialog::attributes
  wxStaticBox* sizer_8_staticbox;
  wxStaticBox* sizer_11_staticbox;
  wxStaticBox* sizer_7_staticbox;
  wxStaticText* label_name;
  wxStaticText* label_Type;
  wxStaticText* label_order;
  wxTextCtrl* text_ctrl_name;
  wxComboBox* combo_box_type;
  wxTextCtrl* text_ctrl_order;
  wxCheckBox* checkbox_clipable;
  wxStaticText* label_clipmax;
  wxTextCtrl* text_ctrl_clipmax;
  wxStaticText* label_clipmin;
  wxTextCtrl* text_ctrl_clipmin;
  wxRadioBox* radio_box_input1;
  wxPanel* notebook_Main_pane_basic;
  wxStaticText* label_gain;
  wxTextCtrl* text_ctrl_gain;
  wxStaticText* label_as_max;
  wxTextCtrl* text_ctrl_as_max;
  wxStaticText* label_as_min;
  wxTextCtrl* text_ctrl_as_min;
  wxPanel* notebook_Main_pane_aerosurface;
  wxStaticText* label_indep;
  wxComboBox* combo_box_indep;
  wxGrid* grid_table;
  wxPanel* notebook_Main_pane_table;
  wxNotebook* notebook_Main_pane;
  wxButton* button_OK;
  wxButton* button_Cancel;
  // end wxGlade

 public:
  void OnClipable (wxCommandEvent & event);
  void OnType (wxCommandEvent & event);

  bool Show( bool show = TRUE );
  void OnOK( wxCommandEvent &event );

  void GetDataIn(Gain * g);
  void SetDataOut(Gain * g);

  DECLARE_EVENT_TABLE ()

};


#endif // GAIN_DLG_H
