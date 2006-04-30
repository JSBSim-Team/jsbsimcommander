/////////////////////////////////////////////////////////////////////////////
// Name:        fcsprop_dlg.cpp
// Purpose:     FCS Builder Property Dialogs
// Author:      Matthew Gong
// Modified by:
// Created:     02/02/2005
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

#include <wx/valtext.h>
#include <wx/valgen.h>

#include "fcsprop_dlg.h"

BEGIN_EVENT_TABLE (FCSPropDialog, wxDialog)
  EVT_LISTBOX (ID_BOX, FCSPropDialog::OnSelect)
  EVT_BUTTON  (wxID_OK, FCSPropDialog::OnOK)
  EVT_BUTTON  (ID_ADD, FCSPropDialog::OnAdd)
  EVT_BUTTON  (ID_REMOVE, FCSPropDialog::OnRemove)
  EVT_TEXT_ENTER(ID_TEXT, FCSPropDialog::OnModify)
  EVT_BUTTON  (ID_MODIFY, FCSPropDialog::OnModify)
END_EVENT_TABLE ()


FCSPropDialog::FCSPropDialog(const wxString & t, const wxString & n, const wxArrayString & as, wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, style)
{
    // begin wxGlade: FCSPropDialog::FCSPropDialog
    label_type = new wxStaticText(this, -1, _("Type:"));
    const wxString combo_box_type_choices[] = {
        wxT("flight_control"),
        wxT("autopilot")
    };
    combo_box_type = new wxComboBox(this, -1, t, wxDefaultPosition, wxDefaultSize, 2, combo_box_type_choices, wxCB_DROPDOWN|wxCB_READONLY, wxGenericValidator(&type));
    type = t;
    label_name = new wxStaticText(this, -1, _("Name:"));
    text_ctrl_name = new wxTextCtrl(this, -1, n, wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NONE, &name) );
    name = n;
    list_box = new wxListBox(this, ID_BOX, wxDefaultPosition, wxDefaultSize, as, 0);
    text_ctrl = new wxTextCtrl(this, ID_TEXT, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    button_add = new wxButton(this, ID_ADD, _("Add"));
    button_modify = new wxButton(this, ID_MODIFY, _("Modify"));
    button_remove = new wxButton(this, ID_REMOVE, _("Remove"));
    button_OK = new wxButton(this, wxID_OK, _("OK"));
    button_cancel = new wxButton(this, wxID_CANCEL, _("Cancel"));

    set_properties();
    do_layout();
    // end wxGlade
}


void FCSPropDialog::set_properties()
{
    // begin wxGlade: FCSPropDialog::set_properties
    combo_box_type->SetSelection(0);
    list_box->SetMinSize(wxSize(300, 300));
    text_ctrl->SetMinSize(wxSize(300, 24));
    // end wxGlade
}


void FCSPropDialog::do_layout()
{
    // begin wxGlade: FCSPropDialog::do_layout
    wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_4 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_3 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_5 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_6 = new wxBoxSizer(wxHORIZONTAL);
    sizer_6->Add(label_type, 0, wxALL, 5);
    sizer_6->Add(combo_box_type, 1, wxALL, 5);
    sizer_1->Add(sizer_6, 0, wxALL|wxEXPAND, 5);
    sizer_5->Add(label_name, 0, wxALL, 5);
    sizer_5->Add(text_ctrl_name, 1, wxALL, 5);
    sizer_1->Add(sizer_5, 0, wxALL|wxEXPAND, 5);
    sizer_1->Add(list_box, 1, wxALL, 10);
    sizer_1->Add(text_ctrl, 0, wxALL, 10);
    sizer_3->Add(button_add, 1, wxALL, 3);
    sizer_3->Add(button_modify, 1, wxALL, 3);
    sizer_3->Add(button_remove, 1, wxALL, 3);
    sizer_1->Add(sizer_3, 0, wxEXPAND, 0);
    sizer_4->Add(button_OK, 1, wxALL, 10);
    sizer_4->Add(button_cancel, 1, wxALL, 10);
    sizer_1->Add(sizer_4, 0, wxEXPAND, 0);
    SetAutoLayout(true);
    SetSizer(sizer_1);
    sizer_1->Fit(this);
    sizer_1->SetSizeHints(this);
    Layout();
    // end wxGlade
}


void
FCSPropDialog::OnOK(wxCommandEvent & event )
{
  int n = list_box->GetCount();
  data.Clear();
  for (int i=0; i<n; ++i)
  {
    data.Add(list_box->GetString(i));
  }
  
  event.Skip();
}


void
FCSPropDialog::OnAdd(wxCommandEvent & WXUNUSED(event) )
{
  list_box->Append(text_ctrl->GetValue());
}

void
FCSPropDialog::OnModify(wxCommandEvent & WXUNUSED(event) )
{
  int i = list_box->GetSelection();
  if (i == wxNOT_FOUND)
    return;
  if (text_ctrl->GetValue().IsNull())
    return;
  list_box->SetString(i,text_ctrl->GetValue());
}

void
FCSPropDialog::OnRemove(wxCommandEvent & WXUNUSED(event) )
{
  int i = list_box->GetSelection();
  if (i == wxNOT_FOUND)
    return;
  list_box->Delete(i);
}

void
FCSPropDialog::OnSelect(wxCommandEvent & WXUNUSED(event) )
{
  int i = list_box->GetSelection();
  if (i == wxNOT_FOUND)
    return;
  text_ctrl->SetValue(list_box->GetString(i));
}
  
