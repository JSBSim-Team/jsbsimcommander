/////////////////////////////////////////////////////////////////////////////
// Name:        When_Diag.cpp
// Purpose:     Manager of the OptScript
// Author:      Matthew Gong
// Created:     06/23/2005
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

// -*- C++ -*- generated by wxGlade 0.4cvs on Thu Jun 23 17:12:14 2005

#include <wx/txtstrm.h>

#include "FGXMLElement.h"
#include "MyApp.h"
#include "When_Diag.h"
#include "Property_Diag.h"

wxString Parameter::GetText() const
{
  wxString rslt = name;
  switch (comparison)
  {
    case FG_LT:
      rslt += wxT(" < ");
      break;
    case FG_LE:
      rslt += wxT(" <= ");
      break;
    case FG_EQ:
      rslt += wxT(" = ");
      break;
    case FG_GE:
      rslt += wxT(" >= ");
      break;
    case FG_GT:
      rslt += wxT(" > ");
      break;
    case FG_NE:
      rslt += wxT(" <> ");
      break;
  }
  rslt += value;
  return rslt;
}

void Parameter::Load(JSBSim::Element * el)
{
  name = std2wxstr(el->GetAttributeValue("name"));
  wxString comp = std2wxstr(el->GetAttributeValue("comparison"));
  if (comp == wxT("lt"))
    comparison = FG_LT;
  else if (comp == wxT("le"))
    comparison = FG_LE;
  else if (comp == wxT("eq"))
    comparison = FG_EQ;
  else if (comp == wxT("ge"))
    comparison = FG_GE;
  else if (comp == wxT("gt"))
    comparison = FG_GT;
  else if (comp == wxT("ne"))
    comparison = FG_NE;
  value = std2wxstr(el->GetAttributeValue("value"));
}

void Parameter::Save(wxTextOutputStream & out, const wxString & prefix)
{
  wxString comp;
  switch (comparison)
  {
    case FG_LT:
      comp = wxT("lt");
      break;
    case FG_LE:
      comp = wxT("le");
      break;
    case FG_EQ:
      comp = wxT("eq");
      break;
    case FG_GE:
      comp = wxT("ge");
      break;
    case FG_GT:
      comp = wxT("gt");
      break;
    case FG_NE:
      comp = wxT("ne");
      break;
  }
  out << prefix << wxT("<parameter name=\"") << name << wxT("\" comparison=\"") << comp << wxT("\" value=\"") << value << wxT("\"/>\n");
}

wxString Set::GetText() const
{
  wxString rslt;
  if (type == FG_DELTA)
  {
    rslt = wxT("delta ");
  }
  rslt += name + wxT(" = ") + value;
  rslt += wxT(" (");
  rslt += wxT("Form = ");
  switch (action)
  {
    case FG_STEP:
      rslt += wxT("Step");
      break;
    case FG_RAMP:
      rslt += wxString::Format(wxT("Ramp, tc = %s sec"), tc.c_str());
      break;
    case FG_EXP:
      rslt += wxString::Format(wxT("Exp, tc = %s sec"), tc.c_str());
      break;
  }
  if (persistent)
    rslt += wxT(", persist");
  rslt += wxT("); ");
  return rslt;
}

void Set::Load(JSBSim::Element * el)
{
  wxString tmp;
  name = std2wxstr(el->GetAttributeValue("name"));
  
  tmp = std2wxstr(el->GetAttributeValue("type"));
  if (tmp == wxT("FG_DELTA"))
   type = FG_DELTA;
  else
    type = FG_VALUE;
  
  value = std2wxstr(el->GetAttributeValue("value"));
  
  tmp = std2wxstr(el->GetAttributeValue("action"));
  if (tmp == wxT("FG_STEP"))
    action = FG_STEP;
  else if (tmp == wxT("FG_EXP"))
    action = FG_EXP;
  else
    action = FG_RAMP;
  
  tmp = std2wxstr(el->GetAttributeValue("persistent"));
  if (tmp == wxT("true"))
    persistent = true;
  else
    persistent = false;
  
  if (action != FG_STEP)
    tc = std2wxstr(el->GetAttributeValue("tc"));
  else
    tc = wxT("1.0");
}

void Set::Save(wxTextOutputStream & out, const wxString & prefix)
{
  wxString type_str, action_str, persist_str;
  switch (type)
  {
    case FG_DELTA :
      type_str = wxT("FG_DELTA");
      break;
    default :
      type_str = wxT("FG_VALUE");
  }
  switch (action)
  {
    case FG_RAMP :
      action_str = wxT("FG_RAMP");
      break;
    case FG_EXP :
      action_str = wxT("FG_EXP");
      break;
    default :
      action_str = wxT("FG_STEP");
  }

  if (persistent)
  {
    persist_str = wxT("true");
  }
  else
  {
    persist_str = wxT("false");
  }

  out << prefix << wxT("<set name=\"") << name << wxT("\" type=\"") << type_str << wxT("\" value=\"") << value << wxT("\" action=\"") << action_str << wxT("\" persistent=\"") << persist_str;
  if (action != FG_STEP && !tc.IsEmpty())
    out << wxT("\" tc =\"") << tc << wxT("\"/>\n");
  else
    out << wxT("\"/>\n");
}

wxString WhenData::GetText()
{
  wxString rslt = wxT("if ");
  bool first = true;
  for (std::vector<Parameter>::iterator i = parameters.begin(); i != parameters.end(); ++i)
  {
    if (!first)
      rslt += wxT(" and ");
    
    rslt += wxT("(");
    rslt += (*i).GetText();
    rslt += wxT(")");
    
    first = false;
  }
  rslt += wxT("   then ");
  for (std::vector<Set>::iterator i = sets.begin(); i != sets.end(); ++i)
  {
    rslt += (*i).GetText();    
  }
  return rslt;
}

void WhenData::Load(JSBSim::Element * el)
{
  JSBSim::Element * p = el->FindElement("parameter");
  while (p)
  {
    Parameter tmp;
    tmp.Load(p);
    parameters.push_back(tmp);
  
    p = el->FindNextElement("parameter");
  }
  p = el->FindElement("set");
  while (p)
  {
    Set tmp;
    tmp.Load(p);
    sets.push_back(tmp);
    
    p = el->FindNextElement("set");
  }
}

void WhenData::Save(wxTextOutputStream & out, const wxString & prefix)
{
  out << prefix << wxT("<when>\n");
  wxString pre = prefix + wxT("  ");
  for (std::vector<Parameter>::iterator i = parameters.begin(); i != parameters.end(); ++i)
  {
    (*i).Save(out, pre);
  }
  for (std::vector<Set>::iterator i = sets.begin(); i != sets.end(); ++i)
  {
    (*i).Save(out, pre);
  }
  out << prefix << wxT("</when>\n");
}


WhenDialog::WhenDialog(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, style)
{
    // begin wxGlade: WhenDialog::WhenDialog
    notebook_main = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, 0);
    notebook_main_pane_2 = new wxPanel(notebook_main, -1);
    notebook_main_pane_1 = new wxPanel(notebook_main, -1);
    const wxString list_box_para_choices[] = {
        
    };
    list_box_para = new wxListBox(notebook_main_pane_1, ID_PARA_LIST, wxDefaultPosition, wxDefaultSize, 0, list_box_para_choices, 0);
    button_para_append = new wxButton(notebook_main_pane_1, ID_PARA_APPEND, _("Append"));
    button_para_del = new wxButton(notebook_main_pane_1, ID_PARA_DEL, _("Delete"));
    button_para_apply = new wxButton(notebook_main_pane_1, ID_PARA_APPLY, _("Apply"));
    text_ctrl_para_name = new wxTextCtrl(notebook_main_pane_1, ID_PARA_TEXT, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_PROCESS_ENTER);
    button_para_name = new wxButton(notebook_main_pane_1, ID_PARA_NAME, wxT("..."));
    const wxString combo_box_para_comp_choices[] = {
        wxT("<"),
        wxT("<="),
        wxT("="),
        wxT(">="),
        wxT(">"),
        wxT("<>")
    };
    combo_box_para_comp = new wxComboBox(notebook_main_pane_1, ID_PARA_TEXT, wxT(""), wxDefaultPosition, wxDefaultSize, 6, combo_box_para_comp_choices, wxCB_DROPDOWN|wxCB_READONLY);
    text_ctrl_para_value = new wxTextCtrl(notebook_main_pane_1, ID_PARA_TEXT, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    button_para_value = new wxButton(notebook_main_pane_1, ID_PARA_VALUE, wxT("..."));
    const wxString list_box_set_choices[] = {
        
    };
    list_box_set = new wxListBox(notebook_main_pane_2, ID_SET_LIST, wxDefaultPosition, wxDefaultSize, 0, list_box_set_choices, 0);
    button_set_append = new wxButton(notebook_main_pane_2, ID_SET_APPEND, _("Append"));
    button_set_del = new wxButton(notebook_main_pane_2, ID_SET_DEL, _("Delete"));
    button_set_apply = new wxButton(notebook_main_pane_2, ID_SET_APPLY, _("Apply"));
    text_ctrl_set_name = new wxTextCtrl(notebook_main_pane_2, ID_SET_TEXT, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_PROCESS_ENTER);
    button_set_name = new wxButton(notebook_main_pane_2, ID_SET_NAME, wxT("..."));
    label_set_eq = new wxStaticText(notebook_main_pane_2, -1, wxT("="));
    const wxString text_ctrl_set_value_choices[] = {
        
    };
    text_ctrl_set_value = new wxTextCtrl(notebook_main_pane_2, ID_SET_TEXT, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    button_set_value = new wxButton(notebook_main_pane_2, ID_SET_VALUE, wxT("..."));
    label_set_type = new wxStaticText(notebook_main_pane_2, -1, _("Type :"));
    const wxString combo_box_set_type_choices[] = {
        wxT("FG_VALUE"),
        wxT("FG_DELTA")
    };
    combo_box_set_type = new wxComboBox(notebook_main_pane_2, ID_SET_TEXT, wxT(""), wxDefaultPosition, wxDefaultSize, 2, combo_box_set_type_choices, wxCB_DROPDOWN|wxCB_READONLY);
    label_set_action = new wxStaticText(notebook_main_pane_2, -1, _("Action :"));
    const wxString combo_box_set_action_choices[] = {
        wxT("FG_RAMP"),
        wxT("FG_STEP"),
        wxT("FG_EXP")
    };
    combo_box_set_action = new wxComboBox(notebook_main_pane_2, ID_SET_TEXT, wxT(""), wxDefaultPosition, wxDefaultSize, 3, combo_box_set_action_choices, wxCB_DROPDOWN|wxCB_READONLY);
    label_set_tc = new wxStaticText(notebook_main_pane_2, -1, _("TC = "));
    text_ctrl_set_tc = new wxTextCtrl(notebook_main_pane_2, ID_SET_TEXT, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    button_set_tc = new wxButton(notebook_main_pane_2, ID_SET_TC, wxT("..."));
    checkbox_set_persist = new wxCheckBox(notebook_main_pane_2, ID_SET_TEXT, _("persistent"));
    button_ok = new wxButton(this, wxID_OK, _("OK"));
    button_cancel = new wxButton(this, wxID_CANCEL, _("Cancel"));

    set_properties();
    do_layout();
    // end wxGlade
}


BEGIN_EVENT_TABLE(WhenDialog, wxDialog)
    // begin wxGlade: WhenDialog::event_table
    EVT_LISTBOX(ID_PARA_LIST, WhenDialog::OnSelectPara)
    EVT_BUTTON(ID_PARA_APPEND, WhenDialog::OnAppendPara)
    EVT_BUTTON(ID_PARA_DEL, WhenDialog::OnDelPara)
    EVT_BUTTON(ID_PARA_APPLY, WhenDialog::OnApplyPara)
    EVT_LISTBOX(ID_SET_LIST, WhenDialog::OnSelectSet)
    EVT_BUTTON(ID_SET_APPEND, WhenDialog::OnAppendSet)
    EVT_BUTTON(ID_SET_DEL, WhenDialog::OnDelSet)
    EVT_BUTTON(ID_SET_APPLY, WhenDialog::OnApplySet)
    // end wxGlade
    EVT_TEXT_ENTER(ID_PARA_TEXT, WhenDialog::OnApplyPara)
    EVT_COMBOBOX(ID_PARA_TEXT, WhenDialog::OnApplyPara)
    EVT_TEXT_ENTER(ID_SET_TEXT, WhenDialog::OnApplySet)
    EVT_COMBOBOX(ID_SET_TEXT, WhenDialog::OnApplySet)
    EVT_CHECKBOX(ID_SET_TEXT, WhenDialog::OnApplySet)
    EVT_BUTTON(ID_PARA_NAME, WhenDialog::OnParaName)
    EVT_BUTTON(ID_PARA_VALUE, WhenDialog::OnParaValue)
    EVT_BUTTON(ID_SET_NAME, WhenDialog::OnSetName)
    EVT_BUTTON(ID_SET_VALUE, WhenDialog::OnSetValue)
    EVT_BUTTON(ID_SET_TC, WhenDialog::OnSetTC)
END_EVENT_TABLE();


void WhenDialog::OnSelectPara(wxCommandEvent &event)
{
  update_para();
}


void WhenDialog::OnAppendPara(wxCommandEvent &event)
{
  Parameter * data = new Parameter;
  data->name = wxT("sim-time-sec");
  data->comparison = FG_GE;
  data->value = wxT("10");
  int i = list_box_para->Append(data->GetText(), data);
  list_box_para->SetSelection(i);
  if (text_ctrl_para_name->GetValue().IsEmpty() || text_ctrl_para_value->GetValue().IsEmpty() || combo_box_para_comp->GetSelection() == wxNOT_FOUND)
  {
    update_para();
  }
  else
  {
    apply_para();
  }
}


void WhenDialog::OnDelPara(wxCommandEvent &event)
{
  int i = list_box_para->GetSelection();
  if (i != wxNOT_FOUND)
  {
      if ( ::wxMessageBox(_("Do you really want to delete the item?"), _("Confirm"), wxYES_NO | wxICON_QUESTION, this) == wxYES )
      {
        list_box_para->Delete(i);
        update_para();
      }
  }
}


void WhenDialog::OnApplyPara(wxCommandEvent &event)
{
  apply_para();
}


void WhenDialog::OnSelectSet(wxCommandEvent &event)
{
  update_set();
}


void WhenDialog::OnAppendSet(wxCommandEvent &event)
{
  Set * data = new Set;
  data->name = wxT("fcs/elevator-cmd-norm");
  data->type = FG_VALUE;
  data->value = wxT("-0.5");
  data->action = FG_RAMP;
  data->persistent = false;
  data->tc = wxT("0.2");
  int i = list_box_set->Append(data->GetText(), data);
  list_box_set->SetSelection(i);
  if (text_ctrl_set_name->GetValue().IsEmpty() || text_ctrl_set_value->GetValue().IsEmpty() )
  {
    update_set();
  }
  else
  {
    apply_set();
  }
}


void WhenDialog::OnDelSet(wxCommandEvent &event)
{
  int i = list_box_set->GetSelection();
  if (i != wxNOT_FOUND)
  {
      if ( ::wxMessageBox(_("Do you really want to delete the item?"), _("Confirm"), wxYES_NO | wxICON_QUESTION, this) == wxYES )
      {
        list_box_set->Delete(i);
        update_set();
      }
  }
}


void WhenDialog::OnApplySet(wxCommandEvent &event)
{
  apply_set();
}



// wxGlade: add WhenDialog event handlers
void WhenDialog::OnParaName(wxCommandEvent &event)
{
  wxString tmp = text_ctrl_para_name->GetValue();
  if (!tmp.IsEmpty())
    GetPropertyDialog()->Select(tmp);
  if (GetPropertyDialog()->ShowModal() == wxID_OK)
  {
    text_ctrl_para_name->SetValue(GetPropertyDialog()->GetResult());
    apply_para();
  }
}

void WhenDialog::OnParaValue(wxCommandEvent &event)
{
  wxString tmp = text_ctrl_para_value->GetValue();
  if (!tmp.IsEmpty())
    GetPropertyDialog()->Select(tmp);
  if (GetPropertyDialog()->ShowModal() == wxID_OK)
  {
    text_ctrl_para_value->SetValue(GetPropertyDialog()->GetResult());
    apply_para();
  }
}

void WhenDialog::OnSetName(wxCommandEvent &event)
{
  wxString tmp = text_ctrl_set_name->GetValue();
  if (!tmp.IsEmpty())
    GetPropertyDialog()->Select(tmp);
  if (GetPropertyDialog()->ShowModal() == wxID_OK)
  {
    text_ctrl_set_name->SetValue(GetPropertyDialog()->GetResult());
    apply_set();
  }
}

void WhenDialog::OnSetValue(wxCommandEvent &event)
{
  wxString tmp = text_ctrl_set_value->GetValue();
  if (!tmp.IsEmpty())
    GetPropertyDialog()->Select(tmp);
  if (GetPropertyDialog()->ShowModal() == wxID_OK)
  {
    text_ctrl_set_value->SetValue(GetPropertyDialog()->GetResult());
    apply_set();
  }
}

void WhenDialog::OnSetTC(wxCommandEvent &event)
{
  wxString tmp = text_ctrl_set_tc->GetValue();
  if (!tmp.IsEmpty())
    GetPropertyDialog()->Select(tmp);
  if (GetPropertyDialog()->ShowModal() == wxID_OK)
  {
    text_ctrl_set_tc->SetValue(GetPropertyDialog()->GetResult());
    apply_set();
  }
}


void WhenDialog::set_properties()
{
    // begin wxGlade: WhenDialog::set_properties
    combo_box_para_comp->SetMinSize(wxSize(50, 29));
    combo_box_para_comp->SetSelection(0);
    combo_box_set_type->SetSelection(0);
    combo_box_set_action->SetSelection(0);
    button_para_name->SetMinSize(wxSize(50, 29));
    button_para_value->SetMinSize(wxSize(50, 29));
    button_set_name->SetMinSize(wxSize(50, 29));
    button_set_value->SetMinSize(wxSize(50, 29));
    button_set_tc->SetMinSize(wxSize(50, 29));
    list_box_set->SetMinSize(wxSize(-1, 200));
    // end wxGlade
}


void WhenDialog::do_layout()
{
    // begin wxGlade: WhenDialog::do_layout
    wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_7 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_6 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_9 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_8 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_3 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_5 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_4 = new wxBoxSizer(wxHORIZONTAL);
    sizer_3->Add(list_box_para, 1, wxALL|wxEXPAND, 5);
    sizer_4->Add(button_para_append, 1, 0, 0);
    sizer_4->Add(button_para_del, 1, 0, 0);
    sizer_4->Add(button_para_apply, 1, 0, 0);
    sizer_3->Add(sizer_4, 0, wxALL|wxEXPAND, 5);
    sizer_5->Add(text_ctrl_para_name, 1, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL, 3);
    sizer_5->Add(button_para_name, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL, 3);
    sizer_5->Add(combo_box_para_comp, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL, 3);
    sizer_5->Add(text_ctrl_para_value, 1, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL, 3);
    sizer_5->Add(button_para_value, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL, 3);
    sizer_3->Add(sizer_5, 0, wxALL|wxEXPAND, 5);
    notebook_main_pane_1->SetAutoLayout(true);
    notebook_main_pane_1->SetSizer(sizer_3);
    sizer_3->Fit(notebook_main_pane_1);
    sizer_3->SetSizeHints(notebook_main_pane_1);
    sizer_7->Add(list_box_set, 1, wxALL|wxEXPAND, 5);
    sizer_8->Add(button_set_append, 1, 0, 0);
    sizer_8->Add(button_set_del, 1, 0, 0);
    sizer_8->Add(button_set_apply, 1, 0, 0);
    sizer_7->Add(sizer_8, 0, wxALL|wxEXPAND, 5);
    sizer_9->Add(text_ctrl_set_name, 1, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL, 3);
    sizer_9->Add(button_set_name, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL, 3);
    sizer_9->Add(label_set_eq, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
    sizer_9->Add(text_ctrl_set_value, 1, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL, 3);
    sizer_9->Add(button_set_value, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL, 3);
    sizer_7->Add(sizer_9, 0, wxALL|wxEXPAND, 5);
    sizer_6->Add(label_set_type, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
    sizer_6->Add(combo_box_set_type, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 10);
    sizer_6->Add(label_set_action, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
    sizer_6->Add(combo_box_set_action, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 10);
    sizer_6->Add(label_set_tc, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
    sizer_6->Add(text_ctrl_set_tc, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
    sizer_6->Add(button_set_tc, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
    sizer_6->Add(checkbox_set_persist, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
    sizer_7->Add(sizer_6, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
    notebook_main_pane_2->SetAutoLayout(true);
    notebook_main_pane_2->SetSizer(sizer_7);
    sizer_7->Fit(notebook_main_pane_2);
    sizer_7->SetSizeHints(notebook_main_pane_2);
    notebook_main->AddPage(notebook_main_pane_1, _("Condition"));
    notebook_main->AddPage(notebook_main_pane_2, _("Action"));
    sizer_1->Add(notebook_main, 1, wxALL|wxEXPAND, 5);
    sizer_2->Add(button_ok, 1, wxLEFT|wxRIGHT, 10);
    sizer_2->Add(button_cancel, 1, wxLEFT|wxRIGHT, 10);
    sizer_1->Add(sizer_2, 0, wxTOP|wxBOTTOM|wxEXPAND, 10);
    SetAutoLayout(true);
    SetSizer(sizer_1);
    sizer_1->Fit(this);
    sizer_1->SetSizeHints(this);
    Layout();
    // end wxGlade
}


void WhenDialog::Load(WhenData * data)
{
  GetPropertyDialog()->Update();

  if (data)
  {
    for (std::vector<Parameter>::iterator i = data->parameters.begin(); i != data->parameters.end(); ++i)
    {
      list_box_para->Append((*i).GetText(), new Parameter(*i));
    }
    list_box_para->SetSelection(0);
    update_para();

    for (std::vector<Set>::iterator i = data->sets.begin(); i != data->sets.end(); ++i)
    {
      list_box_set->Append((*i).GetText(), new Set(*i));
    }
    list_box_set->SetSelection(0);
    update_set();
  }
}

WhenData * WhenDialog::Save()
{
  WhenData * data = new WhenData;

  for (int i=0; i < list_box_para->GetCount(); ++i)
  {
    data->parameters.push_back(*dynamic_cast<Parameter *>(list_box_para->GetClientObject(i)));
  }

  for (int i=0; i < list_box_set->GetCount(); ++i)
  {
    data->sets.push_back(*dynamic_cast<Set *>(list_box_set->GetClientObject(i)));
  }
  
  return data;
}


void WhenDialog::update_para()
{
  int i = list_box_para->GetSelection();
  if (i == wxNOT_FOUND)
  {
    text_ctrl_para_name->SetValue(wxEmptyString);
    combo_box_para_comp->SetSelection(0);
    text_ctrl_para_value->SetValue(wxEmptyString);
  }
  else
  {
    Parameter * data = dynamic_cast<Parameter *>(list_box_para->GetClientObject(i));
    text_ctrl_para_name->SetValue(data->name);
    combo_box_para_comp->SetSelection((int)data->comparison);
    text_ctrl_para_value->SetValue(data->value);
  }
}

void WhenDialog::apply_para()
{
  int i = list_box_para->GetSelection();
  if (i != wxNOT_FOUND)
  {
    Parameter * data = dynamic_cast<Parameter *>(list_box_para->GetClientObject(i));
    data->name = text_ctrl_para_name->GetValue();
    data->comparison = (eComparison)combo_box_para_comp->GetSelection();
    data->value = text_ctrl_para_value->GetValue();
    list_box_para->SetString(i, data->GetText());
  }
}


void WhenDialog::update_set()
{
  int i = list_box_set->GetSelection();
  if (i == wxNOT_FOUND)
  {
    text_ctrl_set_name->SetValue(wxEmptyString);
    combo_box_set_type->SetSelection(0);
    combo_box_set_action->SetSelection(0);
    text_ctrl_set_value->SetValue(wxEmptyString);
    text_ctrl_set_tc->SetValue(wxEmptyString);
    checkbox_set_persist->SetValue(false);
  }
  else
  {
    Set * data = dynamic_cast<Set *>(list_box_set->GetClientObject(i));
    text_ctrl_set_name->SetValue(data->name);
    combo_box_set_type->SetSelection((int)data->type);
    combo_box_set_action->SetSelection((int)data->action);
    text_ctrl_set_value->SetValue(data->value);
    text_ctrl_set_tc->SetValue(data->tc);
    checkbox_set_persist->SetValue(data->persistent);
  }
}

void WhenDialog::apply_set()
{
  int i = list_box_set->GetSelection();
  if (i != wxNOT_FOUND)
  {
    Set * data = dynamic_cast<Set *>(list_box_set->GetClientObject(i));
    data->name = text_ctrl_set_name->GetValue();
    data->type = (eType)combo_box_set_type->GetSelection();
    data->action = (eAction)combo_box_set_action->GetSelection();
    data->value = text_ctrl_set_value->GetValue();
    if (!text_ctrl_set_tc->GetValue().IsEmpty())
    {
      data->tc = text_ctrl_set_tc->GetValue();
    }
    else
    {
      text_ctrl_set_tc->SetValue(data->tc);
    }
    data->persistent = checkbox_set_persist->GetValue();
    list_box_set->SetString(i, data->GetText());
  }
}


