/////////////////////////////////////////////////////////////////////////////
// Name:        deadband_dlg.h
// Purpose:     dialog for FGDeadBand
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
// end wxGlade


#ifndef DEADBAND_DLG_H
#define DEADBAND_DLG_H

class DeadBand;
class DeadBandPropertyDialog: public wxDialog 
{
public:
    // begin wxGlade: DeadBandPropertyDialog::ids
    enum {
      ID_CLIPABLE = 700,
    };
    // end wxGlade

    DeadBandPropertyDialog(DeadBand * shape, wxWindow* parent, int id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxSize(240, 390), long style=wxDEFAULT_DIALOG_STYLE);

private:
    // begin wxGlade: DeadBandPropertyDialog::methods
    void set_properties();
    void do_layout();
    // end wxGlade

  wxString name;
  wxString order;
  bool clipable;
  wxString clipmax;
  wxString clipmin;
  int input1;

  wxString width;
  wxString gain;
  
protected:
    // begin wxGlade: DeadBandPropertyDialog::attributes
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
    wxStaticText* label_width;
    wxTextCtrl* text_ctrl_width;
    wxStaticText* label_gain;
    wxTextCtrl* text_ctrl_gain;
    wxPanel* notebook_Main_pane_special;
    wxNotebook* notebook_Main_pane;
    wxButton* button_OK;
    wxButton* button_Cancel;
    // end wxGlade

 public:
  void OnClipable (wxCommandEvent & event);
  void OnType (wxCommandEvent & event);

  bool Show( bool show = TRUE );
  void OnOK( wxCommandEvent &event );

  void GetDataIn(DeadBand * shape);
  void SetDataOut(DeadBand * shape);

  DECLARE_EVENT_TABLE ()

};


#endif // DEADBAND_DLG_H
