/////////////////////////////////////////////////////////////////////////////
// Name:        kinemat_dlg.cpp
// Purpose:     dialog for FGKinemat
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
#include "kinemat.h"
#include "kinemat_dlg.h"

BEGIN_EVENT_TABLE (KinematPropertyDialog, wxDialog)
  EVT_CHECKBOX(ID_CLIPABLE, KinematPropertyDialog::OnClipable)
  EVT_BUTTON  (wxID_OK, KinematPropertyDialog::OnOK)
END_EVENT_TABLE ()


KinematPropertyDialog::KinematPropertyDialog(Kinemat * componment, wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxTHICK_FRAME)
{
    // begin wxGlade: KinematPropertyDialog::KinematPropertyDialog
    notebook_Main_pane = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, 0);
    notebook_Main_pane_kinemat = new wxPanel(notebook_Main_pane, -1);
    notebook_Main_pane_basic = new wxPanel(notebook_Main_pane, -1);
    sizer_8_staticbox = new wxStaticBox(notebook_Main_pane_basic, -1, _("inputs"));
    sizer_7_staticbox = new wxStaticBox(notebook_Main_pane_basic, -1, _("cliper"));
    label_name = new wxStaticText(notebook_Main_pane_basic, -1, _("Name:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    label_Type = new wxStaticText(notebook_Main_pane_basic, -1, _("Type:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    label_order = new wxStaticText(notebook_Main_pane_basic, -1, _("Order:"));
    text_ctrl_name = new wxTextCtrl(notebook_Main_pane_basic, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NONE, &name) );
    label_type = new wxStaticText(notebook_Main_pane_basic, -1, wxT("KINEMAT"));
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
    grid_kinemat = new wxGrid(notebook_Main_pane_kinemat, -1);
    checkbox_isscale = new wxCheckBox(notebook_Main_pane_kinemat, -1, _("input scale"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&isscale));
    button_OK = new wxButton(this, wxID_OK, _("OK"));
    button_Cancel = new wxButton(this, wxID_CANCEL, _("Cancel"));

    set_properties();
    do_layout();
    // end wxGlade

    GetDataIn(componment);
}


void KinematPropertyDialog::set_properties()
{
    // begin wxGlade: KinematPropertyDialog::set_properties
    radio_box_input1->SetSelection(0);
    grid_kinemat->CreateGrid(40, 2);
    grid_kinemat->SetRowLabelSize(30);
    grid_kinemat->SetColLabelSize(30);
    grid_kinemat->SetSelectionMode(wxGrid::wxGridSelectRows);
    grid_kinemat->SetColLabelValue(0, _("position"));
    grid_kinemat->SetColLabelValue(1, _("time"));
    checkbox_isscale->SetValue(1);
    button_Cancel->SetFocus();
    // end wxGlade
}


void KinematPropertyDialog::do_layout()
{
    // begin wxGlade: KinematPropertyDialog::do_layout
    wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_11 = new wxBoxSizer(wxVERTICAL);
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
    sizer_11->Add(grid_kinemat, 1, wxEXPAND, 0);
    sizer_11->Add(checkbox_isscale, 0, wxALL, 3);
    notebook_Main_pane_kinemat->SetAutoLayout(true);
    notebook_Main_pane_kinemat->SetSizer(sizer_11);
    sizer_11->Fit(notebook_Main_pane_kinemat);
    sizer_11->SetSizeHints(notebook_Main_pane_kinemat);
    notebook_Main_pane->AddPage(notebook_Main_pane_basic, _("Basic"));
    notebook_Main_pane->AddPage(notebook_Main_pane_kinemat, _("Kinemat"));
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
KinematPropertyDialog::Show( bool show)
{
  bool tmp = wxDialog::Show(show);

  if (show)
    {
      notebook_Main_pane_basic->InitDialog();
      notebook_Main_pane_kinemat->InitDialog();
    }

  return tmp;
}

void
KinematPropertyDialog::OnOK(wxCommandEvent & event )
{
  if (notebook_Main_pane_basic->Validate())
    notebook_Main_pane_basic->TransferDataFromWindow();
  if (notebook_Main_pane_kinemat->Validate())
    notebook_Main_pane_kinemat->TransferDataFromWindow();

  event.Skip();
}


void
KinematPropertyDialog::OnClipable(wxCommandEvent & WXUNUSED(event) )
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
KinematPropertyDialog::GetDataIn(Kinemat * k)
{
  name       = k->GetName();
  order      = wxString::Format("%ld",k->GetOrder());
  clipable   = k->IsClipable();
  clipmax    = k->GetClipMax();
  clipmin    = k->GetClipMin();

  input1     = *(k->GetInputSignList().GetFirst()->GetData())?1:0;

  isscale   = k->IsScale();

  MySettingPairList & list = k->GetPairList();
  wxMySettingPairListNode * node = list.GetFirst();
  int i = 0;
  while (node)
    {
      MySettingPair * value = node->GetData();
      grid_kinemat->SetCellValue(i,0,wxString::Format("%g",value->pos));
      grid_kinemat->SetCellValue(i,1,wxString::Format("%g",value->time));
      node = node->GetNext();
      ++i;
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

}


void
KinematPropertyDialog::SetDataOut(Kinemat * k)
{
  k->SetName(name);
  long int tmpl;
  order.ToLong(&tmpl);
  k->SetOrder(tmpl);
  k->SetClipable(clipable);
  double tmpd;
  k->SetClipMax(clipmax);
  k->SetClipMin(clipmin);
  *(k->GetInputSignList().GetFirst()->GetData()) = input1==0?false:true;

  k->SetScale(isscale);
  
  MySettingPairList & list = k->GetPairList();
  list.Clear();
  for ( int i = 0; i < 40; ++i)
    {
      MySettingPair * p = new MySettingPair;
      bool f1 = grid_kinemat->GetCellValue(i,0).ToDouble(&(p->pos));
      bool f2 = grid_kinemat->GetCellValue(i,1).ToDouble(&(p->time));
      if (!f1 || !f2)
	{
	  delete p;
	  break;
	}
      list.Append(p);
    }
}
