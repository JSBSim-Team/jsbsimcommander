/////////////////////////////////////////////////////////////////////////////
// Name:        func_dlg.h
// Purpose:     dialog for FGFCSFunction
// Author:      Matthew Gong
// Created:     07/24/2005
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

#include <wx/treectrl.h>

#ifndef FCSFUNCTION_DLG_H
#define FCSFUNCTION_DLG_H

#include "func.h"

class FCSFunctionDialog: public wxDialog {
public:
    // begin wxGlade: FCSFunctionDialog::ids
  enum
    {
      ID_TYPE = 700,
      ID_CLIPABLE,
      ID_INPUT_LISTBOX,
      ID_INPUT_RADIO_SET,
      ID_INPUT_ADD,
      ID_INPUT_REMOVE,
    };
    // end wxGlade

    FCSFunctionDialog(FCSFunction * componment, wxWindow* parent, int id, const wxString& title=wxT("FCS Function Property Dialog"), const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxSize(320, 380), long style=wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

private:
    // begin wxGlade: FCSFunctionDialog::methods
    void set_properties();
    void do_layout();
    // end wxGlade

protected:
    // begin wxGlade: FCSFunctionDialog::attributes
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

    wxTreeCtrl* tree_func;
    
    wxPanel* notebook_Main_pane_tree;
    TreeItemData_function * function_data;
    wxTreeItemId root;

 private:
  wxString name;
  wxString order;
  bool clipable;
  wxString clipmax;
  wxString clipmin;

 public:
  void OnClipable (wxCommandEvent & event);

  bool Show( bool show = TRUE );
  void OnOK( wxCommandEvent &event );

  void GetDataIn(FCSFunction * s);
  void SetDataOut(FCSFunction * s);

  void OnInputListBox (wxCommandEvent & event);
  void OnInputRadioSet (wxCommandEvent & event);
  void OnInputAdd (wxCommandEvent & event);
  void OnInputRemove (wxCommandEvent & event);

  DECLARE_EVENT_TABLE ()
};

#endif // FCSFUNCTION_DLG_H
