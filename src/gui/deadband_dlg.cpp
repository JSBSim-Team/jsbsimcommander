/////////////////////////////////////////////////////////////////////////////
// Name:        deadband_dlg.cpp
// Purpose:     dialog for FGDeadBand
// Author:      Matthew Gong
// Created:     02/05/2005
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

#include "shape.h"
#include "deadband.h"
#include "deadband_dlg.h"


BEGIN_EVENT_TABLE (DeadBandPropertyDialog, wxDialog)
  EVT_CHECKBOX(ID_CLIPABLE, DeadBandPropertyDialog::OnClipable)
  EVT_BUTTON  (wxID_OK, DeadBandPropertyDialog::OnOK)
END_EVENT_TABLE ()


DeadBandPropertyDialog::DeadBandPropertyDialog(DeadBand * shape, wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxTHICK_FRAME)
{
    // begin wxGlade: DeadBandPropertyDialog::DeadBandPropertyDialog
    notebook_Main_pane = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, 0);
    notebook_Main_pane_special = new wxPanel(notebook_Main_pane, -1);
    notebook_Main_pane_basic = new wxPanel(notebook_Main_pane, -1);
    sizer_8_staticbox = new wxStaticBox(notebook_Main_pane_basic, -1, _("inputs"));
    sizer_7_staticbox = new wxStaticBox(notebook_Main_pane_basic, -1, _("cliper"));
    label_name = new wxStaticText(notebook_Main_pane_basic, -1, _("Name:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    label_Type = new wxStaticText(notebook_Main_pane_basic, -1, _("Type:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    label_order = new wxStaticText(notebook_Main_pane_basic, -1, _("Order:"));
    text_ctrl_name = new wxTextCtrl(notebook_Main_pane_basic, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NONE, &name) );
    label_type = new wxStaticText(notebook_Main_pane_basic, -1, wxT("DEADBAND"));
    text_ctrl_order = new wxTextCtrl(notebook_Main_pane_basic, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &order));
    checkbox_clipable = new wxCheckBox(notebook_Main_pane_basic, ID_CLIPABLE, _("clipable"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&clipable));
    label_max = new wxStaticText(notebook_Main_pane_basic, -1, _("Max:"));
    text_ctrl_clipmax = new wxTextCtrl(notebook_Main_pane_basic, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NONE, &clipmax));
    label_min = new wxStaticText(notebook_Main_pane_basic, -1, _("Min:"));
    text_ctrl_clipmin = new wxTextCtrl(notebook_Main_pane_basic, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NONE, &clipmin));
    const wxString radio_box_input1_choices[] = {
        wxT("positive"),
        wxT("negative")
    };
    radio_box_input1 = new wxRadioBox(notebook_Main_pane_basic, -1, _("input1"), wxDefaultPosition, wxDefaultSize, 2, radio_box_input1_choices, 1, wxRA_SPECIFY_ROWS,  wxGenericValidator(&input1));
    label_width = new wxStaticText(notebook_Main_pane_special, -1, _("width:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    text_ctrl_width = new wxTextCtrl(notebook_Main_pane_special, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &width));
    label_gain = new wxStaticText(notebook_Main_pane_special, -1, _("gain:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    text_ctrl_gain = new wxTextCtrl(notebook_Main_pane_special, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &gain));
    button_OK = new wxButton(this, wxID_OK, _("OK"));
    button_Cancel = new wxButton(this, wxID_CANCEL, _("Cancel"));

    set_properties();
    do_layout();
    // end wxGlade

    GetDataIn(shape);
}


void DeadBandPropertyDialog::set_properties()
{
    // begin wxGlade: DeadBandPropertyDialog::set_properties
    radio_box_input1->SetSelection(0);
    button_OK->SetFocus();
    // end wxGlade
}


void DeadBandPropertyDialog::do_layout()
{
    // begin wxGlade: DeadBandPropertyDialog::do_layout
    wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_11 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_13 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_12 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_3 = new wxBoxSizer(wxVERTICAL);
    wxStaticBoxSizer* sizer_8 = new wxStaticBoxSizer(sizer_8_staticbox, wxVERTICAL);
    wxStaticBoxSizer* sizer_7 = new wxStaticBoxSizer(sizer_7_staticbox, wxVERTICAL);
    wxBoxSizer* sizer_10 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_9 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_4 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_6 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_5 = new wxBoxSizer(wxVERTICAL);
    sizer_5->Add(label_name, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    sizer_5->Add(label_Type, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    sizer_5->Add(label_order, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    sizer_4->Add(sizer_5, 1, wxLEFT|wxTOP|wxBOTTOM|wxEXPAND, 10);
    sizer_6->Add(text_ctrl_name, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 3);
    sizer_6->Add(label_type, 1, wxALL|wxALIGN_CENTER_VERTICAL, 3);
    sizer_6->Add(text_ctrl_order, 1, wxALL|wxALIGN_CENTER_VERTICAL, 3);
    sizer_4->Add(sizer_6, 2, wxRIGHT|wxTOP|wxBOTTOM|wxEXPAND, 10);
    sizer_3->Add(sizer_4, 0, wxEXPAND, 0);
    sizer_7->Add(checkbox_clipable, 0, wxLEFT, 5);
    sizer_9->Add(label_max, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
    sizer_9->Add(text_ctrl_clipmax, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
    sizer_7->Add(sizer_9, 1, wxTOP|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL, 5);
    sizer_10->Add(label_min, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
    sizer_10->Add(text_ctrl_clipmin, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
    sizer_7->Add(sizer_10, 1, wxTOP|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL, 5);
    sizer_3->Add(sizer_7, 0, wxEXPAND, 0);
    sizer_8->Add(radio_box_input1, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL, 5);
    sizer_3->Add(sizer_8, 1, wxEXPAND, 0);
    notebook_Main_pane_basic->SetAutoLayout(true);
    notebook_Main_pane_basic->SetSizer(sizer_3);
    sizer_3->Fit(notebook_Main_pane_basic);
    sizer_3->SetSizeHints(notebook_Main_pane_basic);
    sizer_12->Add(label_width, 1, wxALL|wxALIGN_CENTER_VERTICAL, 3);
    sizer_12->Add(text_ctrl_width, 2, wxALL|wxALIGN_CENTER_VERTICAL, 3);
    sizer_11->Add(sizer_12, 1, wxEXPAND, 0);
    sizer_13->Add(label_gain, 1, wxALL|wxALIGN_CENTER_VERTICAL, 3);
    sizer_13->Add(text_ctrl_gain, 2, wxALL|wxALIGN_CENTER_VERTICAL, 3);
    sizer_11->Add(sizer_13, 1, wxEXPAND, 0);
    notebook_Main_pane_special->SetAutoLayout(true);
    notebook_Main_pane_special->SetSizer(sizer_11);
    sizer_11->Fit(notebook_Main_pane_special);
    sizer_11->SetSizeHints(notebook_Main_pane_special);
    notebook_Main_pane->AddPage(notebook_Main_pane_basic, _("Basic"));
    notebook_Main_pane->AddPage(notebook_Main_pane_special, _("Deadband"));
    sizer_1->Add(notebook_Main_pane, 2, wxALL|wxEXPAND, 5);
    sizer_2->Add(button_OK, 1, wxALL, 5);
    sizer_2->Add(button_Cancel, 1, wxALL, 5);
    sizer_1->Add(sizer_2, 0, wxEXPAND, 0);
    SetAutoLayout(true);
    SetSizer(sizer_1);
    sizer_1->Fit(this);
    sizer_1->SetSizeHints(this);
    Layout();
    // end wxGlade
}



bool
DeadBandPropertyDialog::Show( bool show)
{
  bool tmp = wxDialog::Show(show);

  if (show)
    {
      notebook_Main_pane_basic->InitDialog();
      notebook_Main_pane_special->InitDialog();
    }

  return tmp;
}


void
DeadBandPropertyDialog::OnOK(wxCommandEvent & event )
{
  if (notebook_Main_pane_basic->Validate())
    notebook_Main_pane_basic->TransferDataFromWindow();
  if (notebook_Main_pane_special->Validate())
    notebook_Main_pane_special->TransferDataFromWindow();

  event.Skip();
}


void
DeadBandPropertyDialog::OnClipable(wxCommandEvent & WXUNUSED(event) )
{
  if ( checkbox_clipable->IsChecked() )
    {
      text_ctrl_clipmax->Enable(true);
      text_ctrl_clipmin->Enable(true);
    }
  else
    {
      text_ctrl_clipmax->Enable(false);
      text_ctrl_clipmin->Enable(false);
    }
}


void
DeadBandPropertyDialog::GetDataIn(DeadBand * shape)
{
  name       = shape->GetName();
  order      = wxString::Format("%ld",shape->GetOrder());
  clipable   = shape->IsClipable();
  clipmax    = shape->GetClipMax();
  clipmin    = shape->GetClipMin();

  input1     = *(shape->GetInputSignList().GetFirst()->GetData())?1:0;

  width      = wxString::Format("%g",shape->GetDeadBandWidth());
  gain       = wxString::Format("%g",shape->GetGain());

  if (clipable)
    {
      text_ctrl_clipmax->Enable(true);
      text_ctrl_clipmin->Enable(true);
    }
  else
    {
      text_ctrl_clipmax->Enable(false);
      text_ctrl_clipmin->Enable(false);
    }
}


void
DeadBandPropertyDialog::SetDataOut(DeadBand * shape)
{
  shape->SetName(name);
  long int tmpl;
  order.ToLong(&tmpl);
  shape->SetOrder(tmpl);
  shape->SetClipable(clipable);
  double tmpd;
  shape->SetClipMax(clipmax);
  shape->SetClipMin(clipmin);
  *(shape->GetInputSignList().GetFirst()->GetData()) = input1==0?false:true;

  width.ToDouble(&tmpd);
  shape->SetDeadBandWidth(tmpd);
  gain.ToDouble(&tmpd);
  shape->SetGain(tmpd);
}


