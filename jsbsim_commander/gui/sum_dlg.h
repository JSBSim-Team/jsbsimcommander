/////////////////////////////////////////////////////////////////////////////
// Name:        sum_dlg.h
// Purpose:     dialog for FGSummer
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


#ifndef SUMDIALOG_H
#define SUMDIALOG_H

class Summer;
class SummerPropertyDialog : public wxDialog 
{
 public:
  // begin wxGlade: SumPropertyDialog::ids
  enum
    {
      ID_TYPE = 700,
      ID_CLIPABLE,
      ID_LISTBOX,
      ID_RADIO_SET,
      ID_INPUT_ADD,
      ID_INPUT_REMOVE
    };
  // end wxGlade

  SummerPropertyDialog(Summer * sum, wxWindow* parent, int id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxSize(240, 350), long style=wxDEFAULT_DIALOG_STYLE);

 private:
  // begin wxGlade: SumPropertyDialog::methods
  void set_properties();
  void do_layout();
  // end wxGlade

 protected:
  // begin wxGlade: SumPropertyDialog::attributes
  wxStaticBox* sizer_11_staticbox;
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
  wxStaticText* label_bias;
  wxTextCtrl* text_ctrl_bias;
  wxPanel* notebook_Main_pane_basic;
  wxListBox* list_box_input;
  wxRadioBox* radio_box_set;
  wxButton* button_add;
  wxButton* button_remove;
  wxPanel* notebook_Main_pane_input;
  wxNotebook* notebook_Main_pane;
  wxButton* button_OK;
  wxButton* button_Cancel;
  // end wxGlade

 private:
  wxString name;
  wxString order;
  bool clipable;
  wxString clipmax;
  wxString clipmin;

  wxString bias;

 public:
  void OnClipable (wxCommandEvent & event);

  bool Show( bool show = TRUE );
  void OnOK( wxCommandEvent &event );

  void GetDataIn(Summer * g);
  void SetDataOut(Summer * g);

  void OnListBox (wxCommandEvent & event);
  void OnRadioSet (wxCommandEvent & event);
  void OnInputAdd (wxCommandEvent & event);
  void OnInputRemove (wxCommandEvent & event);

  DECLARE_EVENT_TABLE ()
};


#endif // SUMDIALOG_H
