/////////////////////////////////////////////////////////////////////////////
// Name:        filter_dlg.cpp
// Purpose:     dialog for FGFilter
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
#include "filter.h"
#include "filter_dlg.h"

#include "integrator.xpm"
#include "lag.xpm"
#include "ledlag.xpm"
#include "scdorder.xpm"
#include "washout.xpm"



BEGIN_EVENT_TABLE (FilterPropertyDialog, wxDialog)
  EVT_CHECKBOX(ID_CLIPABLE, FilterPropertyDialog::OnClipable)
  EVT_TEXT(ID_TYPE, FilterPropertyDialog::OnType)
  EVT_BUTTON  (wxID_OK, FilterPropertyDialog::OnOK)
END_EVENT_TABLE ()


FilterPropertyDialog::FilterPropertyDialog(Filter * shape, wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxTHICK_FRAME)
{
    // begin wxGlade: FilterPropertyDialog::FilterPropertyDialog
    notebook_Main_pane = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, 0);
    notebook_Main_pane_filter = new wxPanel(notebook_Main_pane, -1);
    notebook_Main_pane_basic = new wxPanel(notebook_Main_pane, -1);
    sizer_8_staticbox = new wxStaticBox(notebook_Main_pane_basic, -1, _("inputs"));
    sizer_7_staticbox = new wxStaticBox(notebook_Main_pane_basic, -1, _("cliper"));
    label_name = new wxStaticText(notebook_Main_pane_basic, -1, _("Name:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    label_Type = new wxStaticText(notebook_Main_pane_basic, -1, _("Type:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    label_order = new wxStaticText(notebook_Main_pane_basic, -1, _("Order:"));
    text_ctrl_name = new wxTextCtrl(notebook_Main_pane_basic, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NONE, &name) );
    const wxString combo_box_type_choices[] = {
        wxT("LAG_FILTER"),
        wxT("LEAD_LAG_FILTER"),
        wxT("SECOND_ORDER_FILTER"),
        wxT("WASHOUT_FILTER"),
        wxT("INTEGRATOR")
    };
    combo_box_type = new wxComboBox(notebook_Main_pane_basic, ID_TYPE, wxT(""), wxDefaultPosition, wxDefaultSize, 5, combo_box_type_choices, wxCB_READONLY,wxGenericValidator(&type));
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
    bitmap_show = new wxStaticBitmap(notebook_Main_pane_filter, -1, wxBITMAP(lag));
    label_c1 = new wxStaticText(notebook_Main_pane_filter, -1, _("c1:"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
    label_c2 = new wxStaticText(notebook_Main_pane_filter, -1, _("c2:"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
    label_c3 = new wxStaticText(notebook_Main_pane_filter, -1, _("c3:"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
    label_c4 = new wxStaticText(notebook_Main_pane_filter, -1, _("c4:"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
    label_c5 = new wxStaticText(notebook_Main_pane_filter, -1, _("c5:"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
    label_c6 = new wxStaticText(notebook_Main_pane_filter, -1, _("c6:"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
    label_trigger = new wxStaticText(notebook_Main_pane_filter, -1, _("trigger:"));
    text_ctrl_c1 = new wxTextCtrl(notebook_Main_pane_filter, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NONE, &c1));
    text_ctrl_c2 = new wxTextCtrl(notebook_Main_pane_filter, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NONE, &c2));
    text_ctrl_c3 = new wxTextCtrl(notebook_Main_pane_filter, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NONE, &c3));
    text_ctrl_c4 = new wxTextCtrl(notebook_Main_pane_filter, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NONE, &c4));
    text_ctrl_c5 = new wxTextCtrl(notebook_Main_pane_filter, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NONE, &c5));
    text_ctrl_c6 = new wxTextCtrl(notebook_Main_pane_filter, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NONE, &c6));
    text_ctrl_trigger = new wxTextCtrl(notebook_Main_pane_filter, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NONE, &trigger));
    button_OK = new wxButton(this, wxID_OK, _("OK"));
    button_Cancel = new wxButton(this, wxID_CANCEL, _("Cancel"));

    set_properties();
    do_layout();
    // end wxGlade

    GetDataIn(shape);
}


void FilterPropertyDialog::set_properties()
{
    // begin wxGlade: FilterPropertyDialog::set_properties
    text_ctrl_trigger->Enable(false);
    button_OK->SetFocus();
    // end wxGlade
}


void FilterPropertyDialog::do_layout()
{
    // begin wxGlade: FilterPropertyDialog::do_layout
    wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_13 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_14 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_16 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_15 = new wxBoxSizer(wxVERTICAL);
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
    sizer_6->Add(combo_box_type, 1, wxALL|wxALIGN_CENTER_VERTICAL, 3);
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
    sizer_13->Add(bitmap_show, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 10);
    sizer_15->Add(label_c1, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 7);
    sizer_15->Add(label_c2, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 7);
    sizer_15->Add(label_c3, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 7);
    sizer_15->Add(label_c4, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 7);
    sizer_15->Add(label_c5, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 7);
    sizer_15->Add(label_c6, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 7);
    sizer_15->Add(label_trigger, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 3);
    sizer_14->Add(sizer_15, 1, wxEXPAND, 0);
    sizer_16->Add(text_ctrl_c1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 3);
    sizer_16->Add(text_ctrl_c2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 3);
    sizer_16->Add(text_ctrl_c3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 3);
    sizer_16->Add(text_ctrl_c4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 3);
    sizer_16->Add(text_ctrl_c5, 0, wxALL|wxALIGN_CENTER_VERTICAL, 3);
    sizer_16->Add(text_ctrl_c6, 0, wxALL|wxALIGN_CENTER_VERTICAL, 3);
    sizer_16->Add(text_ctrl_trigger, 0, wxALL|wxALIGN_CENTER_VERTICAL, 3);
    sizer_14->Add(sizer_16, 2, wxEXPAND, 0);
    sizer_13->Add(sizer_14, 1, wxEXPAND, 0);
    sizer_13->Add(20, 20, 0, wxFIXED_MINSIZE, 0);
    notebook_Main_pane_filter->SetAutoLayout(true);
    notebook_Main_pane_filter->SetSizer(sizer_13);
    sizer_13->Fit(notebook_Main_pane_filter);
    sizer_13->SetSizeHints(notebook_Main_pane_filter);
    notebook_Main_pane->AddPage(notebook_Main_pane_basic, _("Basic"));
    notebook_Main_pane->AddPage(notebook_Main_pane_filter, _("Filter"));
    sizer_1->Add(notebook_Main_pane, 2, wxALL|wxEXPAND, 5);
    sizer_2->Add(button_OK, 1, wxALL, 5);
    sizer_2->Add(button_Cancel, 1, wxALL, 5);
    sizer_1->Add(sizer_2, 0, wxEXPAND, 0);
    SetAutoLayout(true);
    SetSizer(sizer_1);
    sizer_1->Fit(this);
    sizer_1->SetSizeHints(this);
    Layout();
    Centre();
    // end wxGlade
}


bool
FilterPropertyDialog::Show( bool show)
{
  bool tmp = wxDialog::Show(show);

  if (show)
    {
      notebook_Main_pane_basic->InitDialog();
      notebook_Main_pane_filter->InitDialog();
    }

  return tmp;
}


void
FilterPropertyDialog::OnOK(wxCommandEvent & event )
{
  if (notebook_Main_pane_basic->Validate())
    notebook_Main_pane_basic->TransferDataFromWindow();
  if (notebook_Main_pane_filter->Validate())
    notebook_Main_pane_filter->TransferDataFromWindow();

  event.Skip();
}


void
FilterPropertyDialog::OnClipable(wxCommandEvent & WXUNUSED(event) )
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
FilterPropertyDialog::aftertypechange()
{
  if ( combo_box_type->GetValue() == wxT("LAG_FILTER") )
  {
    text_ctrl_c1->Enable(true);
    text_ctrl_c2->Enable(false);
    text_ctrl_c3->Enable(false);
    text_ctrl_c4->Enable(false);
    text_ctrl_c5->Enable(false);
    text_ctrl_c6->Enable(false);
    bitmap_show->SetBitmap(wxBITMAP(lag));
  }
  else if (combo_box_type->GetValue() == wxT("LEAD_LAG_FILTER") )
  {
    text_ctrl_c1->Enable(true);
    text_ctrl_c2->Enable(true);
    text_ctrl_c3->Enable(true);
    text_ctrl_c4->Enable(true);
    text_ctrl_c5->Enable(false);
    text_ctrl_c6->Enable(false);
    bitmap_show->SetBitmap(wxBITMAP(ledlag));
  }
  else if (combo_box_type->GetValue() == wxT("SECOND_ORDER_FILTER") )
  {
    text_ctrl_c1->Enable(true);
    text_ctrl_c2->Enable(true);
    text_ctrl_c3->Enable(true);
    text_ctrl_c4->Enable(true);
    text_ctrl_c5->Enable(true);
    text_ctrl_c6->Enable(true);
    bitmap_show->SetBitmap(wxBITMAP(scdorder));
  }
  else if (combo_box_type->GetValue() == wxT("WASHOUT_FILTER") )
  {
    text_ctrl_c1->Enable(true);
    text_ctrl_c2->Enable(false);
    text_ctrl_c3->Enable(false);
    text_ctrl_c4->Enable(false);
    text_ctrl_c5->Enable(false);
    text_ctrl_c6->Enable(false);
    bitmap_show->SetBitmap(wxBITMAP(washout));
  }
  else if (combo_box_type->GetValue() == wxT("INTEGRATOR") )
  {
    text_ctrl_c1->Enable(true);
    text_ctrl_c2->Enable(false);
    text_ctrl_c3->Enable(false);
    text_ctrl_c4->Enable(false);
    text_ctrl_c5->Enable(false);
    text_ctrl_c6->Enable(false);
    bitmap_show->SetBitmap(wxBITMAP(integrator));
  }
  else
  {
    text_ctrl_c1->Enable(false);
    text_ctrl_c2->Enable(false);
    text_ctrl_c3->Enable(false);
    text_ctrl_c4->Enable(false);
    text_ctrl_c5->Enable(false);
    text_ctrl_c6->Enable(false);
  }
}


void
FilterPropertyDialog::OnType(wxCommandEvent & WXUNUSED(event) )
{
  aftertypechange();
}


void
FilterPropertyDialog::GetDataIn(Filter * shape)
{
  name       = shape->GetName();
  type       = shape->GetType();
  combo_box_type->SetValue(type);
  order      = wxString::Format("%ld",shape->GetOrder());
  clipable   = shape->IsClipable();
  clipmax    = shape->GetClipMax();
  clipmin    = shape->GetClipMin();

  input1     = *(shape->GetInputSignList().GetFirst()->GetData())?1:0;

  c1         = shape->GetC1();
  c2         = shape->GetC2();
  c3         = shape->GetC3();
  c4         = shape->GetC4();
  c5         = shape->GetC5();
  c6         = shape->GetC6();

  trigger    = shape->GetTrigger();

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

  aftertypechange();
}


void
FilterPropertyDialog::SetDataOut(Filter * shape)
{
  shape->SetName(name);
  shape->SetType(type);
  long int tmpl;
  order.ToLong(&tmpl);
  shape->SetOrder(tmpl);
  shape->SetClipable(clipable);
  shape->SetClipMax(clipmax);
  shape->SetClipMin(clipmin);
  *(shape->GetInputSignList().GetFirst()->GetData()) = input1==0?false:true;

  shape->SetC1(c1);
  shape->SetC2(c2);
  shape->SetC3(c3);
  shape->SetC4(c4);
  shape->SetC5(c5);
  shape->SetC6(c6);

  shape->SetTrigger(trigger);
}



