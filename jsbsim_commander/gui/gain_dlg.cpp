/////////////////////////////////////////////////////////////////////////////
// Name:        gain_dlg.cpp
// Purpose:     dialog for FGGain
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
#include "gain.h"
#include "gain_dlg.h"

BEGIN_EVENT_TABLE (GainPropertyDialog, wxDialog)
  EVT_CHECKBOX(ID_CLIPABLE, GainPropertyDialog::OnClipable)
  EVT_TEXT(ID_TYPE, GainPropertyDialog::OnType)
  EVT_BUTTON  (wxID_OK, GainPropertyDialog::OnOK)
END_EVENT_TABLE ()


GainPropertyDialog::GainPropertyDialog(Gain * componment, const wxArrayString & source, wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxTHICK_FRAME)
{
    // begin wxGlade: GainPropertyDialog::GainPropertyDialog
    notebook_Main_pane = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, 0);
    notebook_Main_pane_table = new wxPanel(notebook_Main_pane, -1);
    notebook_Main_pane_aerosurface = new wxPanel(notebook_Main_pane, -1);
    notebook_Main_pane_basic = new wxPanel(notebook_Main_pane, -1);
    sizer_8_staticbox = new wxStaticBox(notebook_Main_pane_basic, -1, _("inputs"));
    sizer_7_staticbox = new wxStaticBox(notebook_Main_pane_basic, -1, _("cliper"));
    label_name = new wxStaticText(notebook_Main_pane_basic, -1, _("Name:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    label_Type = new wxStaticText(notebook_Main_pane_basic, -1, _("Type:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    label_order = new wxStaticText(notebook_Main_pane_basic, -1, _("Order:"));
    text_ctrl_name = new wxTextCtrl(notebook_Main_pane_basic, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NONE, &name) );
    const wxString combo_box_type_choices[] = {
        wxT("PURE_GAIN"),
        wxT("AEROSURFACE_SCALE"),
        wxT("SCHEDULED_GAIN")
    };
    combo_box_type = new wxComboBox(notebook_Main_pane_basic, ID_TYPE, wxT(""), wxDefaultPosition, wxDefaultSize, 3, combo_box_type_choices, wxCB_READONLY,wxGenericValidator(&type));
    text_ctrl_order = new wxTextCtrl(notebook_Main_pane_basic, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &order));
    checkbox_clipable = new wxCheckBox(notebook_Main_pane_basic, ID_CLIPABLE, _("clipable"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&clipable));
    label_clipmax = new wxStaticText(notebook_Main_pane_basic, -1, _("Max:"));
    text_ctrl_clipmax = new wxTextCtrl(notebook_Main_pane_basic, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NONE, &clipmax));
    label_clipmin = new wxStaticText(notebook_Main_pane_basic, -1, _("Min:"));
    text_ctrl_clipmin = new wxTextCtrl(notebook_Main_pane_basic, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NONE, &clipmin));
    const wxString radio_box_input1_choices[] = {
        wxT("positive"),
        wxT("negative")
    };
    radio_box_input1 = new wxRadioBox(notebook_Main_pane_basic, -1, _("input1"), wxDefaultPosition, wxDefaultSize, 2, radio_box_input1_choices, 1, wxRA_SPECIFY_ROWS,  wxGenericValidator(&input1));
    label_gain = new wxStaticText(notebook_Main_pane_basic, -1, _("Gain:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    text_ctrl_gain = new wxTextCtrl(notebook_Main_pane_basic, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &gain));
    label_as_max = new wxStaticText(notebook_Main_pane_aerosurface, -1, _("Max:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    text_ctrl_as_max = new wxTextCtrl(notebook_Main_pane_aerosurface, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &as_max));
    label_as_min = new wxStaticText(notebook_Main_pane_aerosurface, -1, _("Min:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    text_ctrl_as_min = new wxTextCtrl(notebook_Main_pane_aerosurface, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &as_min));
    label_indep = new wxStaticText(notebook_Main_pane_table, -1, _("independentVar:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    combo_box_indep = new wxComboBox(notebook_Main_pane_table, -1, wxT(""), wxDefaultPosition, wxDefaultSize, source, 0, wxGenericValidator(&indep));
    grid_table = new wxGrid(notebook_Main_pane_table, -1);
    button_OK = new wxButton(this, wxID_OK, _("OK"));
    button_Cancel = new wxButton(this, wxID_CANCEL, _("Cancel"));

    set_properties();
    do_layout();
    // end wxGlade

    GetDataIn(componment);
}


void GainPropertyDialog::set_properties()
{
    // begin wxGlade: GainPropertyDialog::set_properties
    combo_box_type->SetSelection(-1);
    radio_box_input1->SetSelection(0);
    combo_box_indep->SetSelection(-1);
    grid_table->CreateGrid(99, 2);
    grid_table->SetColLabelValue(0, _("independentVar"));
    grid_table->SetColSize(0, 130);
    grid_table->SetColLabelValue(1, _("Value"));
    grid_table->SetColSize(1, 70);
    button_Cancel->SetFocus();
    // end wxGlade
}


void GainPropertyDialog::do_layout()
{
    // begin wxGlade: GainPropertyDialog::do_layout
    wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_17 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_18 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_14 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_16 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_15 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_13 = new wxBoxSizer(wxHORIZONTAL);
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
    sizer_9->Add(label_clipmax, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
    sizer_9->Add(text_ctrl_clipmax, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
    sizer_7->Add(sizer_9, 1, wxTOP|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL, 5);
    sizer_10->Add(label_clipmin, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
    sizer_10->Add(text_ctrl_clipmin, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
    sizer_7->Add(sizer_10, 1, wxTOP|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL, 5);
    sizer_3->Add(sizer_7, 0, wxEXPAND, 0);
    sizer_13->Add(label_gain, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    sizer_13->Add(text_ctrl_gain, 2, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    sizer_3->Add(sizer_13, 0, wxEXPAND, 0);
    sizer_8->Add(radio_box_input1, 0, wxALL|wxEXPAND, 5);
    sizer_3->Add(sizer_8, 1, wxEXPAND, 0);
    notebook_Main_pane_basic->SetAutoLayout(true);
    notebook_Main_pane_basic->SetSizer(sizer_3);
    sizer_3->Fit(notebook_Main_pane_basic);
    sizer_3->SetSizeHints(notebook_Main_pane_basic);
    sizer_14->Add(20, 20, 1, wxFIXED_MINSIZE, 0);
    sizer_15->Add(label_as_max, 1, wxALL|wxALIGN_CENTER_VERTICAL, 3);
    sizer_15->Add(text_ctrl_as_max, 2, wxALL, 10);
    sizer_14->Add(sizer_15, 0, wxEXPAND, 0);
    sizer_16->Add(label_as_min, 1, wxALL|wxALIGN_CENTER_VERTICAL, 3);
    sizer_16->Add(text_ctrl_as_min, 2, wxALL, 10);
    sizer_14->Add(sizer_16, 0, wxEXPAND, 0);
    sizer_14->Add(20, 20, 2, wxFIXED_MINSIZE, 0);
    notebook_Main_pane_aerosurface->SetAutoLayout(true);
    notebook_Main_pane_aerosurface->SetSizer(sizer_14);
    sizer_14->Fit(notebook_Main_pane_aerosurface);
    sizer_14->SetSizeHints(notebook_Main_pane_aerosurface);
    sizer_18->Add(label_indep, 0, wxALL|wxALIGN_CENTER_VERTICAL, 3);
    sizer_18->Add(combo_box_indep, 1, wxALL, 3);
    sizer_17->Add(sizer_18, 0, wxALL|wxEXPAND, 10);
    sizer_17->Add(grid_table, 1, wxALL|wxEXPAND, 3);
    notebook_Main_pane_table->SetAutoLayout(true);
    notebook_Main_pane_table->SetSizer(sizer_17);
    sizer_17->Fit(notebook_Main_pane_table);
    sizer_17->SetSizeHints(notebook_Main_pane_table);
    notebook_Main_pane->AddPage(notebook_Main_pane_basic, _("Basic"));
    notebook_Main_pane->AddPage(notebook_Main_pane_aerosurface, _("AeroSurface"));
    notebook_Main_pane->AddPage(notebook_Main_pane_table, _("Scheduled"));
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
GainPropertyDialog::Show( bool show)
{
  bool tmp = wxDialog::Show(show);

  if (show)
    {
      notebook_Main_pane_basic->InitDialog();
      notebook_Main_pane_aerosurface->InitDialog();
      notebook_Main_pane_table->InitDialog();
    }

  return tmp;
}

void
GainPropertyDialog::OnOK(wxCommandEvent & event )
{
  if (notebook_Main_pane_basic->Validate())
    notebook_Main_pane_basic->TransferDataFromWindow();
  if (notebook_Main_pane_aerosurface->Validate())
    notebook_Main_pane_aerosurface->TransferDataFromWindow();
  if (notebook_Main_pane_table->Validate())
    notebook_Main_pane_table->TransferDataFromWindow();

  event.Skip();
}


void
GainPropertyDialog::OnClipable(wxCommandEvent & WXUNUSED(event) )
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
GainPropertyDialog::OnType(wxCommandEvent & WXUNUSED(event) )
{
  if ( combo_box_type->GetValue() == wxT("PURE_GAIN") )
  {
    notebook_Main_pane_aerosurface->Enable(false);
    notebook_Main_pane_table->Enable(false);
  }
  else if (combo_box_type->GetValue() == wxT("AEROSURFACE_SCALE") )
  {
    notebook_Main_pane_aerosurface->Enable(true);
    notebook_Main_pane_table->Enable(false);
  }
  else if (combo_box_type->GetValue() == wxT("SCHEDULED_GAIN") )
  {
    notebook_Main_pane_aerosurface->Enable(false);
    notebook_Main_pane_table->Enable(true);
  }
  else
  {
    notebook_Main_pane_aerosurface->Enable(true);
    notebook_Main_pane_table->Enable(true);
  }
  
}


void
GainPropertyDialog::GetDataIn(Gain * g)
{
  name       = g->GetName();
  type       = g->GetType();
  order      = wxString::Format("%ld",g->GetOrder());
  clipable   = g->IsClipable();
  clipmax    = g->GetClipMax();
  clipmin    = g->GetClipMin();

  input1     = *(g->GetInputSignList().GetFirst()->GetData())?1:0;

  gain       = wxString::Format("%g",g->GetGain());

  as_max     = wxString::Format("%g",g->GetAeroSurfaceMax());
  as_min     = wxString::Format("%g",g->GetAeroSurfaceMin());
  
  indep      = g->GetIndependVar();

  MyPairList & list = g->GetTable();
  wxMyPairListNode * node = list.GetFirst();
  int i = 0;
  while (node)
    {
      MyPair * value = node->GetData();
      grid_table->SetCellValue(i,0,wxString::Format("%g",value->x));
      grid_table->SetCellValue(i,1,wxString::Format("%g",value->y));
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

  if ( type == wxT("PURE_GAIN") )
  {
    notebook_Main_pane_aerosurface->Enable(false);
    notebook_Main_pane_table->Enable(false);
  }
  else if ( type == wxT("AEROSURFACE_SCALE") )
  {
    notebook_Main_pane_aerosurface->Enable(true);
    notebook_Main_pane_table->Enable(false);
  }
  else if ( type == wxT("SCHEDULED_GAIN") )
  {
    notebook_Main_pane_aerosurface->Enable(false);
    notebook_Main_pane_table->Enable(true);
  }
  else
  {
    notebook_Main_pane_aerosurface->Enable(true);
    notebook_Main_pane_table->Enable(true);
  }

}


void
GainPropertyDialog::SetDataOut(Gain * g)
{
  g->SetName(name);
  if ( type.Length() > 2 )
     g->SetType(type);
  long int tmpl;
  order.ToLong(&tmpl);
  g->SetOrder(tmpl);
  g->SetClipable(clipable);
  double tmpd;
  g->SetClipMax(clipmax);
  g->SetClipMin(clipmin);
  *(g->GetInputSignList().GetFirst()->GetData()) = input1==0?false:true;

  double tmp;
  gain.ToDouble(&tmp);
  g->SetGain(tmp);

  as_max.ToDouble(&tmp);
  g->SetAeroSurfaceMax(tmp);
  as_min.ToDouble(&tmp);
  g->SetAeroSurfaceMin(tmp);

  if (indep.Length()>0)
    g->SetIndepentVar(indep);
  
  MyPairList & list = g->GetTable();
  list.Clear();
  for ( int i = 0; i < 99; ++i)
    {
      MyPair * p = new MyPair;
      bool f1 = grid_table->GetCellValue(i,0).ToDouble(&(p->x));
      bool f2 = grid_table->GetCellValue(i,1).ToDouble(&(p->y));
      if (!f1 || !f2)
	{
	  delete p;
	  break;
	}
      list.Append(p);
    }
}
