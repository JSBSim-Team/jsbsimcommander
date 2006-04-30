/////////////////////////////////////////////////////////////////////////////
// Name:        filter_dlg.h
// Purpose:     dialog for FGFilter
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


#ifndef FILTER_DLG_H
#define FILTER_DLG_H


class Filter;
class FilterPropertyDialog: public wxDialog {
public:
  // begin wxGlade: FilterPropertyDialog::ids
  enum
    {
      ID_TYPE = 700,
      ID_CLIPABLE,
    };
  // end wxGlade

  FilterPropertyDialog(Filter * shape, wxWindow* parent, int id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxSize(320, 440), long style=wxDEFAULT_DIALOG_STYLE);

private:
  // begin wxGlade: FilterPropertyDialog::methods
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
  
  wxString c1;
  wxString c2;
  wxString c3;
  wxString c4;
  wxString c5;
  wxString c6;

  wxString trigger;

  void aftertypechange();

protected:
  // begin wxGlade: FilterPropertyDialog::attributes
  wxStaticBox* sizer_8_staticbox;
  wxStaticBox* sizer_7_staticbox;
  wxStaticText* label_name;
  wxStaticText* label_Type;
  wxStaticText* label_order;
  wxTextCtrl* text_ctrl_name;
  wxComboBox* combo_box_type;
  wxTextCtrl* text_ctrl_order;
  wxCheckBox* checkbox_clipable;
  wxStaticText* label_max;
  wxTextCtrl* text_ctrl_clipmax;
  wxStaticText* label_min;
  wxTextCtrl* text_ctrl_clipmin;
  wxRadioBox* radio_box_input1;
  wxPanel* notebook_Main_pane_basic;
  wxStaticBitmap* bitmap_show;
  wxStaticText* label_c1;
  wxStaticText* label_c2;
  wxStaticText* label_c3;
  wxStaticText* label_c4;
  wxStaticText* label_c5;
  wxStaticText* label_c6;
  wxStaticText* label_trigger;
  wxTextCtrl* text_ctrl_c1;
  wxTextCtrl* text_ctrl_c2;
  wxTextCtrl* text_ctrl_c3;
  wxTextCtrl* text_ctrl_c4;
  wxTextCtrl* text_ctrl_c5;
  wxTextCtrl* text_ctrl_c6;
  wxTextCtrl* text_ctrl_trigger;
  wxPanel* notebook_Main_pane_filter;
  wxNotebook* notebook_Main_pane;
  wxButton* button_OK;
  wxButton* button_Cancel;
  // end wxGlade

 public:
  void OnClipable (wxCommandEvent & event);
  void OnType (wxCommandEvent & event);

  bool Show( bool show = TRUE );
  void OnOK( wxCommandEvent &event );

  void GetDataIn(Filter * shape);
  void SetDataOut(Filter * shape);

  DECLARE_EVENT_TABLE ()

};

#endif // FILTER_DLG_H
