/////////////////////////////////////////////////////////////////////////////
// Name:        switch_dlg.h
// Purpose:     dialog for FGSwitch
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

#include <wx/treectrl.h>
#include "switch.h"

#ifndef SWITCH_DLG_H
#define SWITCH_DLG_H

class SwitchPropertyDialog: public wxDialog {
public:
    // begin wxGlade: SwitchPropertyDialog::ids
  enum
    {
      ID_TYPE = 700,
      ID_CLIPABLE,
      ID_INPUT_LISTBOX,
      ID_INPUT_RADIO_SET,
      ID_INPUT_ADD,
      ID_INPUT_REMOVE,
      ID_TREE,
      ID_APPLY_COND,
      ID_APPLY_OUTPUT,
      MENUITEM_DEL,
      MENUITEM_ADD_SCOND,
      MENUITEM_ADD_CCOND_AND,
      MENUITEM_ADD_CCOND_OR,
      MENUITEM_ADD_TEST_AND,
      MENUITEM_ADD_TEST_OR
    };
    // end wxGlade

    SwitchPropertyDialog(Switch* componment, wxWindow* parent, int id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxSize(410, 330), long style=wxDEFAULT_DIALOG_STYLE);

private:
    // begin wxGlade: SwitchPropertyDialog::methods
    void set_properties();
    void do_layout();
    // end wxGlade

    wxTreeItemId AddTest(Test * test, wxTreeItemId & root);
    wxTreeItemId AddCondition(Condition * cond, wxTreeItemId & parent);
    Test * OutputTest(wxTreeItemId id);
    Condition * OutputCond(wxTreeItemId id);

protected:
    // begin wxGlade: SwitchPropertyDialog::attributes
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

    wxTreeCtrl* tree_test;
    
    wxStaticText* label_output;
    wxComboBox* combo_box_output;
    wxButton* button_output_apply;
    wxPanel* pane_output;
    
    wxComboBox* combo_box_para1;
    wxComboBox* combo_box_compare;
    wxComboBox* combo_box_para2;
    wxButton* button_apply;
    wxPanel* pane_condition;
    
    wxPanel* notebook_Main_pane_tree;

 private:
  wxString name;
  wxString order;
  bool clipable;
  wxString clipmax;
  wxString clipmin;
  TestList testlist;

 public:
  void OnClipable (wxCommandEvent & event);

  bool Show( bool show = TRUE );
  void OnOK( wxCommandEvent &event );

  void GetDataIn(Switch * s);
  void SetDataOut(Switch * s);

  void OnInputListBox (wxCommandEvent & event);
  void OnInputRadioSet (wxCommandEvent & event);
  void OnInputAdd (wxCommandEvent & event);
  void OnInputRemove (wxCommandEvent & event);
  void OnTreeSelChanging (wxTreeEvent & event);
  void OnTreeRightClick (wxTreeEvent & event);
  void OnApplyOutput (wxCommandEvent & event);
  void OnApplyCond (wxCommandEvent & event);
  void OnUpdateMenu(wxUpdateUIEvent & event);
  void OnMenuDel (wxCommandEvent & event);
  void OnMenuAddSCond (wxCommandEvent & event);
  void OnAddCCondAND (wxCommandEvent & event);
  void OnMenuCCondOR (wxCommandEvent & event);
  void OnMenuTestAND (wxCommandEvent & event);
  void OnMenuTestOR (wxCommandEvent & event);

 private:
  wxBoxSizer * sizer_12;
  wxMenu menu;

  DECLARE_EVENT_TABLE ()
};

#endif // SWITCH_DLG_H
