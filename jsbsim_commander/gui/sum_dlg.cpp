/////////////////////////////////////////////////////////////////////////////
// Name:        sum_dlg.cpp
// Purpose:     dialog for FGSummer
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
#include "sum.h"
#include "sum_dlg.h"


BEGIN_EVENT_TABLE (SummerPropertyDialog, wxDialog)
  EVT_CHECKBOX(ID_CLIPABLE, SummerPropertyDialog::OnClipable)
  EVT_BUTTON  (wxID_OK, SummerPropertyDialog::OnOK)
  EVT_LISTBOX (ID_LISTBOX, SummerPropertyDialog::OnListBox)
  EVT_RADIOBOX (ID_RADIO_SET, SummerPropertyDialog::OnRadioSet)
  EVT_BUTTON  (ID_INPUT_ADD, SummerPropertyDialog::OnInputAdd)
  EVT_BUTTON  (ID_INPUT_REMOVE, SummerPropertyDialog::OnInputRemove)
END_EVENT_TABLE ()


SummerPropertyDialog::SummerPropertyDialog(Summer * sum, wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxTHICK_FRAME)
{
    // begin wxGlade: SumPropertyDialog::SumPropertyDialog
    notebook_Main_pane = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, 0);
    notebook_Main_pane_input = new wxPanel(notebook_Main_pane, -1);
    notebook_Main_pane_basic = new wxPanel(notebook_Main_pane, -1);
    sizer_7_staticbox = new wxStaticBox(notebook_Main_pane_basic, -1, _("cliper"));
    label_name = new wxStaticText(notebook_Main_pane_basic, -1, _("Name:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    label_Type = new wxStaticText(notebook_Main_pane_basic, -1, _("Type:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    label_order = new wxStaticText(notebook_Main_pane_basic, -1, _("Order:"));
    text_ctrl_name = new wxTextCtrl(notebook_Main_pane_basic, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NONE, &name) );
    label_type = new wxStaticText(notebook_Main_pane_basic, -1, wxT("SUMMER"));
    text_ctrl_order = new wxTextCtrl(notebook_Main_pane_basic, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &order));
    checkbox_clipable = new wxCheckBox(notebook_Main_pane_basic, ID_CLIPABLE, _("clipable"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&clipable));
    label_max = new wxStaticText(notebook_Main_pane_basic, -1, _("Max:"));
    text_ctrl_clipmax = new wxTextCtrl(notebook_Main_pane_basic, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NONE, &clipmax));
    label_min = new wxStaticText(notebook_Main_pane_basic, -1, _("Min:"));
    text_ctrl_clipmin = new wxTextCtrl(notebook_Main_pane_basic, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NONE, &clipmin));
    label_bias = new wxStaticText(notebook_Main_pane_basic, -1, _("Bias:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    text_ctrl_bias = new wxTextCtrl(notebook_Main_pane_basic, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &bias));
    const wxString list_box_input_choices[] = {
        
    };
    list_box_input = new wxListBox(notebook_Main_pane_input, ID_LISTBOX, wxDefaultPosition, wxDefaultSize, 0, list_box_input_choices, wxLB_SINGLE|wxLB_NEEDED_SB);
    const wxString radio_box_set_choices[] = {
        wxT("Positive"),
        wxT("Negative")
    };
    radio_box_set = new wxRadioBox(notebook_Main_pane_input, ID_RADIO_SET, _("set"), wxDefaultPosition, wxDefaultSize, 2, radio_box_set_choices, 1, wxRA_SPECIFY_ROWS);
    button_add = new wxButton(notebook_Main_pane_input, ID_INPUT_ADD, _("Add"));
    button_remove = new wxButton(notebook_Main_pane_input, ID_INPUT_REMOVE, _("Remove"));
    button_OK = new wxButton(this, wxID_OK, _("OK"));
    button_Cancel = new wxButton(this, wxID_CANCEL, _("Cancel"));

    set_properties();
    do_layout();
    // end wxGlade

    GetDataIn(sum);
}


void SummerPropertyDialog::set_properties()
{
    // begin wxGlade: SummerPropertyDialog::set_properties
    button_Cancel->SetFocus();
    // end wxGlade
}


void SummerPropertyDialog::do_layout()
{
    // begin wxGlade: SummerPropertyDialog::do_layout
    wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_13 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_14 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_3 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_8 = new wxBoxSizer(wxHORIZONTAL);
    //wxBoxSizer* sizer_12 = new wxBoxSizer(wxHORIZONTAL);
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
    sizer_8->Add(label_bias, 1, wxALL, 7);
    sizer_8->Add(text_ctrl_bias, 2, wxALL, 3);
    sizer_3->Add(sizer_8, 0, wxALL|wxEXPAND, 4);
    notebook_Main_pane_basic->SetAutoLayout(true);
    notebook_Main_pane_basic->SetSizer(sizer_3);
    sizer_3->Fit(notebook_Main_pane_basic);
    sizer_3->SetSizeHints(notebook_Main_pane_basic);
    sizer_13->Add(list_box_input, 2, wxALL|wxEXPAND|wxFIXED_MINSIZE, 5);
    sizer_13->Add(radio_box_set, 0, wxALL|wxEXPAND|wxFIXED_MINSIZE, 5);
    sizer_14->Add(button_add, 1, wxALL, 10);
    sizer_14->Add(button_remove, 1, wxALL, 10);
    sizer_13->Add(sizer_14, 1, wxBOTTOM|wxEXPAND, 10);
    notebook_Main_pane_input->SetAutoLayout(true);
    notebook_Main_pane_input->SetSizer(sizer_13);
    sizer_13->Fit(notebook_Main_pane_input);
    sizer_13->SetSizeHints(notebook_Main_pane_input);
    notebook_Main_pane->AddPage(notebook_Main_pane_basic, _("Basic"));
    notebook_Main_pane->AddPage(notebook_Main_pane_input, _("Input"));
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
SummerPropertyDialog::Show( bool show)
{
  bool tmp = wxDialog::Show(show);

  if (show)
    {
      notebook_Main_pane_basic->InitDialog();
      notebook_Main_pane_input->InitDialog();
    }

  return tmp;
}

void
SummerPropertyDialog::OnOK(wxCommandEvent & event )
{
  if (notebook_Main_pane_basic->Validate())
    notebook_Main_pane_basic->TransferDataFromWindow();
  if (notebook_Main_pane_input->Validate())
    notebook_Main_pane_input->TransferDataFromWindow();

  event.Skip();
}


void
SummerPropertyDialog::OnClipable(wxCommandEvent & WXUNUSED(event) )
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
SummerPropertyDialog::GetDataIn(Summer * g)
{
  name       = g->GetName();
  order      = wxString::Format("%ld",g->GetOrder());
  clipable   = g->IsClipable();
  clipmax    = g->GetClipMax();
  clipmin    = g->GetClipMin();
  
  bias       = wxString::Format("%g",g->GetBias());

  list_box_input->Clear();
  wxMyBoolListNode * node = g->GetInputSignList().GetFirst();
  while (node)
    {
      bool * value = node->GetData();
      if (*value)
	{
	  list_box_input->Append(wxT("negative"));
	}
      else
	{
	  list_box_input->Append(wxT("positive"));
	}
      node = node->GetNext();
    }

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
  list_box_input->SetSelection(0);
  if ( list_box_input->GetString(0) == wxT("positive") )
    radio_box_set->SetSelection(0);
  else
    radio_box_set->SetSelection(1);

  if ( g->GetLines().GetCount()>0 )
    button_remove->Enable(false);
}


void
SummerPropertyDialog::SetDataOut(Summer * g)
{
  g->SetName(name);
  long int tmpl;
  order.ToLong(&tmpl);
  g->SetOrder(tmpl);
  g->SetClipable(clipable);
  double tmpd;
  g->SetClipMax(clipmax);
  g->SetClipMin(clipmin);

  bias.ToDouble(&tmpd);
  g->SetBias(tmpd);
  
  MyBoolList & list = g->GetInputSignList();
  list.Clear();

  double w = g->GetWidth();
  double h = g->GetHeight();
  g->ClearAttachments ();
  g->GetAttachments ().Append (new wxAttachmentPoint (0,  w * 0.5, 0.0));

  int s = list_box_input->GetCount();
  for ( int i = 0; i < s; ++i)
    {
      wxString str = list_box_input->GetString(i);
      if ( str == wxT("positive") )
	list.Append(new bool(false));
      else
	list.Append(new bool(true));
      g->GetAttachments ().Append (new wxAttachmentPoint (i+1, -w * 0.5, 0.0));
    }

  g->SetAttachmentSize(w,h);
  g->NormalizeLines();
}


void 
SummerPropertyDialog::OnListBox (wxCommandEvent & WXUNUSED(event))
{
  wxString str = list_box_input->GetStringSelection();
  if ( str == wxT("negative") )
    radio_box_set->SetSelection(1);
  else
    radio_box_set->SetSelection(0);

}


void 
SummerPropertyDialog::OnRadioSet (wxCommandEvent & WXUNUSED(event))
{
  int pos =  list_box_input->GetSelection();
  if ( radio_box_set->GetSelection() == 0 )
    list_box_input->SetString(pos,wxT("positive"));
  else
    list_box_input->SetString(pos,wxT("negative"));
}


void 
SummerPropertyDialog::OnInputAdd (wxCommandEvent & WXUNUSED(event))
{
  list_box_input->Append(wxT("positive"));
}


void 
SummerPropertyDialog::OnInputRemove (wxCommandEvent & WXUNUSED(event))
{
  if ( list_box_input->GetCount() <= 2)
    return;

  int pos =  list_box_input->GetSelection();
  list_box_input->Delete(pos);
}
