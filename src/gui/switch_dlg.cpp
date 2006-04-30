/////////////////////////////////////////////////////////////////////////////
// Name:        switch_dlg.cpp
// Purpose:     dialog for FGSwitch
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

#include "view.h"
#include "doc.h"

#include "shape.h"
#include "switch.h"
#include "switch_dlg.h"

#include <typeinfo>

class TreeItemTest : public wxTreeItemData
{
public:
  Test::eLogic logic;
  wxString output;
};

class TreeItemCCond : public wxTreeItemData
{
public:
  Condition::eLogic Logic;
};

class TreeItemSCond : public wxTreeItemData
{
public:
  wxString TestParam1, TestParam2;
  Condition::eComparison Comparison;
};

BEGIN_EVENT_TABLE (SwitchPropertyDialog, wxDialog)
  EVT_CHECKBOX(ID_CLIPABLE, SwitchPropertyDialog::OnClipable)
  EVT_BUTTON  (wxID_OK, SwitchPropertyDialog::OnOK)
  EVT_LISTBOX (ID_INPUT_LISTBOX, SwitchPropertyDialog::OnInputListBox)
  EVT_RADIOBOX (ID_INPUT_RADIO_SET, SwitchPropertyDialog::OnInputRadioSet)
  EVT_BUTTON  (ID_INPUT_ADD, SwitchPropertyDialog::OnInputAdd)
  EVT_BUTTON  (ID_INPUT_REMOVE, SwitchPropertyDialog::OnInputRemove)
  EVT_TREE_SEL_CHANGING ( ID_TREE, SwitchPropertyDialog::OnTreeSelChanging)
  EVT_TREE_ITEM_RIGHT_CLICK(ID_TREE, SwitchPropertyDialog::OnTreeRightClick) 
  EVT_BUTTON  (ID_APPLY_COND, SwitchPropertyDialog::OnApplyCond)
  EVT_BUTTON  (ID_APPLY_OUTPUT, SwitchPropertyDialog::OnApplyOutput)
  EVT_UPDATE_UI_RANGE (MENUITEM_DEL, MENUITEM_ADD_TEST_OR, SwitchPropertyDialog::OnUpdateMenu)
  EVT_MENU(    MENUITEM_DEL, SwitchPropertyDialog::OnMenuDel)
  EVT_MENU(    MENUITEM_ADD_SCOND, SwitchPropertyDialog::OnMenuAddSCond)
  EVT_MENU(    MENUITEM_ADD_CCOND_AND, SwitchPropertyDialog::OnAddCCondAND)
  EVT_MENU(    MENUITEM_ADD_CCOND_OR, SwitchPropertyDialog::OnMenuCCondOR)
  EVT_MENU(    MENUITEM_ADD_TEST_AND, SwitchPropertyDialog::OnMenuTestAND)
  EVT_MENU(    MENUITEM_ADD_TEST_OR, SwitchPropertyDialog::OnMenuTestOR)
END_EVENT_TABLE ()

SwitchPropertyDialog::SwitchPropertyDialog(Switch* componment, wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxTHICK_FRAME)
{
    // begin wxGlade: SwitchPropertyDialog::SwitchPropertyDialog
    notebook_Main_pane = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, 0);
    notebook_Main_pane_input = new wxPanel(notebook_Main_pane, -1);
    notebook_Main_pane_basic = new wxPanel(notebook_Main_pane, -1);
    notebook_Main_pane_tree = new wxPanel(notebook_Main_pane, -1);
    sizer_7_staticbox = new wxStaticBox(notebook_Main_pane_basic, -1, _("cliper"));
    label_name = new wxStaticText(notebook_Main_pane_basic, -1, _("Name:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    label_Type = new wxStaticText(notebook_Main_pane_basic, -1, _("Type:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    label_order = new wxStaticText(notebook_Main_pane_basic, -1, _("Order:"));
    text_ctrl_name = new wxTextCtrl(notebook_Main_pane_basic, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NONE, &name) );
    label_type = new wxStaticText(notebook_Main_pane_basic, -1, wxT("SWITCH"));
    text_ctrl_order = new wxTextCtrl(notebook_Main_pane_basic, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &order));
    checkbox_clipable = new wxCheckBox(notebook_Main_pane_basic, ID_CLIPABLE, _("clipable"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&clipable));
    label_max = new wxStaticText(notebook_Main_pane_basic, -1, _("Max:"));
    text_ctrl_clipmax = new wxTextCtrl(notebook_Main_pane_basic, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NONE, &clipmax));
    label_min = new wxStaticText(notebook_Main_pane_basic, -1, _("Min:"));
    text_ctrl_clipmin = new wxTextCtrl(notebook_Main_pane_basic, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NONE, &clipmin));
    const wxString list_box_input_choices[] = {
        
    };
    list_box_input = new wxListBox(notebook_Main_pane_input, ID_INPUT_LISTBOX, wxDefaultPosition, wxDefaultSize, 0, list_box_input_choices, wxLB_SINGLE|wxLB_NEEDED_SB);
    const wxString radio_box_set_choices[] = {
        wxT("Positive"),
        wxT("Negative")
    };
    radio_box_set = new wxRadioBox(notebook_Main_pane_input, ID_INPUT_RADIO_SET, _("set"), wxDefaultPosition, wxDefaultSize, 2, radio_box_set_choices, 1, wxRA_SPECIFY_ROWS);
    button_add = new wxButton(notebook_Main_pane_input, ID_INPUT_ADD, _("Add"));
    button_remove = new wxButton(notebook_Main_pane_input, ID_INPUT_REMOVE, _("Remove"));

    tree_test = new wxTreeCtrl(notebook_Main_pane_tree, ID_TREE, wxDefaultPosition, wxDefaultSize,  wxTR_HAS_BUTTONS|wxTR_DEFAULT_STYLE);

    pane_output = new wxPanel(notebook_Main_pane_tree, -1);
    label_output = new wxStaticText(pane_output, -1, _("Output:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    wxArrayString inputs;
    combo_box_output = new wxComboBox(pane_output, -1, wxT(""), wxDefaultPosition, wxSize(110,-1), inputs, wxCB_DROPDOWN|wxCB_SORT);
    button_output_apply = new wxButton(pane_output, ID_APPLY_OUTPUT, _("Apply"));
    
    pane_condition = new wxPanel(notebook_Main_pane_tree, -1);
    combo_box_para1 = new wxComboBox(pane_condition, -1, wxT(""), wxDefaultPosition, wxSize(90,-1), inputs, wxCB_DROPDOWN|wxCB_SORT);
    const wxString combo_box_compare_choices[] = {
        wxT("=="),
        wxT("!="),
        wxT(">"),
        wxT(">="),
        wxT("<"),
        wxT("<=")
    };
    combo_box_compare = new wxComboBox(pane_condition, -1, wxT(""), wxDefaultPosition, wxSize(50,-1), 6, combo_box_compare_choices, wxCB_READONLY);
    combo_box_para2 = new wxComboBox(pane_condition, -1, wxT(""), wxDefaultPosition, wxSize(90,-1), inputs, wxCB_DROPDOWN|wxCB_SORT);
    button_apply = new wxButton(pane_condition, ID_APPLY_COND, _("Apply"));

    button_OK = new wxButton(this, wxID_OK, _("OK"));
    button_Cancel = new wxButton(this, wxID_CANCEL, _("Cancel"));

    set_properties();
    do_layout();
    // end wxGlade

    menu.Append (MENUITEM_DEL, _("&Delete"));
    menu.AppendSeparator ();
    menu.Append (MENUITEM_ADD_SCOND, _("Add &Simple Condition"));
    menu.Append (MENUITEM_ADD_CCOND_AND, _("Add Compound Condition(&AND)"));
    menu.Append (MENUITEM_ADD_CCOND_OR, _("Add Compound Condition(&OR)"));
    menu.Append (MENUITEM_ADD_TEST_AND, _("Add Test(&AND)"));
    menu.Append (MENUITEM_ADD_TEST_OR, _("Add Test(&OR)"));

    GetDataIn(componment);
}


void SwitchPropertyDialog::set_properties()
{
    // begin wxGlade: SwitchPropertyDialog::set_properties
    button_Cancel->SetFocus();
    // end wxGlade
    pane_output->Hide();
    pane_condition->Hide();
}


void SwitchPropertyDialog::do_layout()
{
    // begin wxGlade: SwitchPropertyDialog::do_layout
    wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_14 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_3 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_8 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_11 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_13 = new wxBoxSizer(wxVERTICAL);
    sizer_12 = new wxBoxSizer(wxVERTICAL);
    
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
       
    sizer_8->Add(label_output, 0, wxALL|wxFIXED_MINSIZE, 8);
    sizer_8->Add(combo_box_output, 1, wxALL|wxFIXED_MINSIZE, 5);
    sizer_8->Add(button_output_apply, 0, wxALL|wxFIXED_MINSIZE, 5);
    pane_output->SetAutoLayout(true);
    pane_output->SetSizer(sizer_8);
    sizer_8->Fit(pane_output);
    sizer_8->SetSizeHints(pane_output);
    
    sizer_11->Add(combo_box_para1, 0, wxALL|wxFIXED_MINSIZE, 5);
    sizer_11->Add(combo_box_compare, 0, wxALL|wxFIXED_MINSIZE, 5);
    sizer_11->Add(combo_box_para2, 0, wxALL|wxFIXED_MINSIZE, 5);
    sizer_11->Add(button_apply, 0, wxALL|wxFIXED_MINSIZE, 5);
    pane_condition->SetAutoLayout(true);
    pane_condition->SetSizer(sizer_11);
    sizer_11->Fit(pane_condition);
    sizer_11->SetSizeHints(pane_condition);
    
    sizer_12->Add(tree_test, 1, wxALL|wxEXPAND, 5);
    sizer_12->Add(pane_output, 0, wxALL|wxFIXED_MINSIZE, 5);
    sizer_12->Add(pane_condition, 0, wxALL|wxFIXED_MINSIZE, 5);
    sizer_12->SetMinSize(380,0);
    notebook_Main_pane_tree->SetAutoLayout(true);
    notebook_Main_pane_tree->SetSizer(sizer_12);
    sizer_12->Fit(notebook_Main_pane_tree);
    sizer_12->SetSizeHints(notebook_Main_pane_tree);
    notebook_Main_pane->AddPage(notebook_Main_pane_basic, _("Basic"));
    notebook_Main_pane->AddPage(notebook_Main_pane_input, _("Input"));
    notebook_Main_pane->AddPage(notebook_Main_pane_tree, _("Tree"));
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
SwitchPropertyDialog::Show( bool show)
{
  bool tmp = wxDialog::Show(show);

  if (show)
    {
      notebook_Main_pane_basic->InitDialog();
      notebook_Main_pane_input->InitDialog();
      notebook_Main_pane_tree->InitDialog();
    }

  return tmp;
}

void
SwitchPropertyDialog::OnOK(wxCommandEvent & event )
{
  if (notebook_Main_pane_basic->Validate())
    notebook_Main_pane_basic->TransferDataFromWindow();
  if (notebook_Main_pane_input->Validate())
    notebook_Main_pane_input->TransferDataFromWindow();
  if (notebook_Main_pane_tree->Validate())
    notebook_Main_pane_tree->TransferDataFromWindow();

  event.Skip();
}


void
SwitchPropertyDialog::OnClipable(wxCommandEvent & WXUNUSED(event) )
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
SwitchPropertyDialog::GetDataIn(Switch * s)
{
  name       = s->GetName();
  order      = wxString::Format("%ld",s->GetOrder());
  clipable   = s->IsClipable();
  clipmax    = s->GetClipMax();
  clipmin    = s->GetClipMin();
  
  {
    list_box_input->Clear();
    wxMyBoolListNode * node = s->GetInputSignList().GetFirst();
    int i = 1;
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
      wxString tmp = wxT("$INPUT") + wxString::Format("%d",i);
      combo_box_output->Append(tmp);
      combo_box_para1->Append(tmp);
      combo_box_para2->Append(tmp);
      ++i;
      node = node->GetNext();
    }
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

  if ( s->GetLines().GetCount()>0 )
    button_remove->Enable(false);

  {
    wxTreeItemId root = tree_test->AddRoot(wxT("Test"));
    wxTestListNode * node = s->GetTestList().GetFirst();
    while (node)
    {
      Test * data = node->GetData();
      AddTest(data, root);
      node = node->GetNext();
    }
    tree_test->Expand(root);
  }
}

wxTreeItemId
SwitchPropertyDialog::AddTest(Test * test, wxTreeItemId & root)
{
  TreeItemTest* data = new TreeItemTest;
  data->logic = test->Logic;
  data->output = test->Output;
  wxString txt;
  switch (test->Logic)
  {
  case Test::eAND :
     txt = wxT("AND");
     break;
  case Test::eOR :
     txt = wxT("OR");
     break;
  case Test::eDefault :
     txt = wxT("Default");
     break;
  default:
     txt = wxT("Undefined");     
  }
  txt += wxT("(") + test->Output + wxT(")");
  wxTreeItemId branch = tree_test->AppendItem(root, txt, -1, -1, data);
  data->SetId(branch);
  
  wxConditionListNode * node = test->conditions.GetFirst();
  while (node)
  {
      Condition * data = node->GetData();
      AddCondition(data, branch);
      node = node->GetNext();
  }

  return branch;
}

wxTreeItemId
SwitchPropertyDialog::AddCondition(Condition * cond, wxTreeItemId & parent)
{
  wxTreeItemId branch;
  if (cond->isGroup)
  {
    TreeItemCCond * cdata = new TreeItemCCond;
    cdata->Logic = cond->Logic;
    wxString txt;
    switch (cond->Logic)
    {
    case Condition::eAND :
       txt = wxT("AND");
       break;
    case Condition::eOR :
       txt = wxT("OR");
       break;
    default:
       txt = wxT("Undefined");     
    }
    branch = tree_test->AppendItem(parent, txt, -1, -1, cdata);
    cdata->SetId(branch);
    wxConditionListNode * node = cond->conditions.GetFirst();
    while (node)
    {
      Condition * data = node->GetData();
      AddCondition(data, branch);
      node = node->GetNext();
    }
  }
  else
  {
    TreeItemSCond * sdata = new TreeItemSCond;
    sdata->TestParam1 = cond->TestParam1;
    sdata->TestParam2 = cond->TestParam2;
    sdata->Comparison = cond->Comparison;
    wxString t = cond->TestParam1;
    switch (cond->Comparison)
    {
	case Condition::eEQ :
	  t += wxT(" == ");
	  break;
	case Condition::eNE :
	  t += wxT(" != ");
	  break;
	case Condition::eGT :
	  t += wxT(" > ");
	  break;
	case Condition::eGE :
	  t += wxT(" >= ");
	  break;
	case Condition::eLT :
	  t += wxT(" < ");
	  break;
	case Condition::eLE :
	  t += wxT(" <= ");
	  break;
	default :
	  t += wxT(" (Undef) ");
    }
    t += cond->TestParam2;
    branch = tree_test->AppendItem(parent, t, -1, -1, sdata);
    sdata->SetId(branch);
  }

  return branch;
}

void
SwitchPropertyDialog::SetDataOut(Switch * s)
{
  s->SetName(name);
  long int tmpl;
  order.ToLong(&tmpl);
  s->SetOrder(tmpl);
  s->SetClipable(clipable);
  double tmpd;
  s->SetClipMax(clipmax);
  s->SetClipMin(clipmin);

  MyBoolList & list = s->GetInputSignList();
  list.Clear();

  double w = s->GetWidth();
  double h = s->GetHeight();
  s->ClearAttachments ();
  s->GetAttachments ().Append (new wxAttachmentPoint (0,  w * 0.5, 0.0));

  int c = list_box_input->GetCount();
  for ( int i = 0; i < c; ++i)
    {
      wxString str = list_box_input->GetString(i);
      if ( str == wxT("positive") )
	list.Append(new bool(false));
      else
	list.Append(new bool(true));
      s->GetAttachments ().Append (new wxAttachmentPoint (i+1, -w * 0.5, 0.0));
    }

  s->SetAttachmentSize(w,h);
  s->NormalizeLines();

  TestList & tl = s->GetTestList();
  tl.DeleteContents(true);
  tl.Clear();
  wxTreeItemId root = tree_test->GetRootItem();
  wxTreeItemIdValue tmp = (wxTreeItemIdValue)0401276;
  for (wxTreeItemId node = tree_test->GetFirstChild(root,tmp); node.IsOk(); node = tree_test->GetNextChild(root,tmp) )
  {
    Test * t = OutputTest(node);
    if (t)
    {
      tl.Append(t);
    }
  }
}

Test * 
SwitchPropertyDialog::OutputTest(wxTreeItemId id)
{
  Test * test = new Test;
  TreeItemTest * data = (TreeItemTest *)tree_test->GetItemData(id);
  test->Logic = data->logic;
  test->Output = data->output;
  
  wxTreeItemIdValue tmp = (wxTreeItemIdValue)0401277;
  for (wxTreeItemId node = tree_test->GetFirstChild(id,tmp); node.IsOk(); node = tree_test->GetNextChild(id,tmp) )
  {
    Condition * c = OutputCond(node);
    if (c)
    {
      test->conditions.Append(c);
    }
  }
  
  bool flag = true;
  if (test->Logic == Test::elUndef)
    flag = false;
  if (test->Output.Length()==0)
    flag = false;
  if (test->conditions.GetCount()==0 && test->Logic != Test::eDefault)
    flag = false;

  if (flag)
  {
    return test;
  }
  else
  {
    delete test;
    return (Test *)NULL;
  }
}

Condition * 
SwitchPropertyDialog::OutputCond(wxTreeItemId id)
{
  Condition * cond = new Condition;
  bool flag = true;
  {
    TreeItemCCond * data = dynamic_cast<TreeItemCCond *>(tree_test->GetItemData(id));
    if (data)
    {
      cond->isGroup = true;
      cond->Logic = data->Logic;
      wxTreeItemIdValue tmp = (wxTreeItemIdValue)040130;
      for (wxTreeItemId node = tree_test->GetFirstChild(id,tmp); node.IsOk(); node = tree_test->GetNextChild(id,tmp) )
      {
	Condition * c = OutputCond(node);
	if (c)
          cond->conditions.Append(c);
      }
      if (cond->Logic == Condition::elUndef)
	flag = false;
      if (cond->conditions.GetCount()==0)
	flag = false;
    }
  }
  {
    TreeItemSCond * data = dynamic_cast<TreeItemSCond *>(tree_test->GetItemData(id));
    if (data)
    {
      cond->isGroup = false;
      cond->TestParam1 = data->TestParam1;
      cond->TestParam2 = data->TestParam2;
      cond->Comparison = data->Comparison;
      if (cond->Comparison == Condition::ecUndef)
	flag = false;
      if (cond->TestParam1 == wxT("unknown"))
	flag = false;
      if (cond->TestParam2 == wxT("unknown"))
	flag = false;
    }
  }

  if (flag)
  {
    return cond;
  }
  else
  {
    delete cond;
    return (Condition *)NULL;
  }
}

void 
SwitchPropertyDialog::OnInputListBox (wxCommandEvent & WXUNUSED(event))
{
  wxString str = list_box_input->GetStringSelection();
  if ( str == wxT("negative") )
    radio_box_set->SetSelection(1);
  else
    radio_box_set->SetSelection(0);
}


void 
SwitchPropertyDialog::OnInputRadioSet (wxCommandEvent & WXUNUSED(event))
{
  int pos =  list_box_input->GetSelection();
  if ( radio_box_set->GetSelection() == 0 )
    list_box_input->SetString(pos,wxT("positive"));
  else
    list_box_input->SetString(pos,wxT("negative"));
}


void 
SwitchPropertyDialog::OnInputAdd (wxCommandEvent & WXUNUSED(event))
{
  list_box_input->Append(wxT("positive"));
  wxString tmp = wxT("$INPUT") + wxString::Format("%d",list_box_input->GetCount());
  combo_box_output->Append(tmp);
  combo_box_para1->Append(tmp);
  combo_box_para2->Append(tmp);
}


void 
SwitchPropertyDialog::OnInputRemove (wxCommandEvent & WXUNUSED(event))
{
  if ( list_box_input->GetCount() <= 1)
    return;

  wxString tmp = wxT("$INPUT") + wxString::Format("%d",list_box_input->GetCount());
  int i = combo_box_output->FindString(tmp);
  combo_box_output->Delete(i);
  combo_box_para1->Delete(i);
  combo_box_para2->Delete(i);
  
  int pos =  list_box_input->GetSelection();
  list_box_input->Delete(pos);
}

void
SwitchPropertyDialog::OnTreeSelChanging (wxTreeEvent & event)
{
  wxTreeItemId id = event.GetItem();
  {
    TreeItemTest * data = dynamic_cast<TreeItemTest *>(tree_test->GetItemData(id));
    if (data)
    {
      pane_condition->Hide();
      pane_output->Show();
      sizer_12->Layout();
      combo_box_output->SetValue(data->output);
      return;
    }
  }
  {
    TreeItemSCond * data = dynamic_cast<TreeItemSCond *>(tree_test->GetItemData(id));
    if (data)
    {
      pane_output->Hide();
      pane_condition->Show();
      sizer_12->Layout();
      combo_box_para1->SetValue(data->TestParam1);
      wxString t;
      switch (data->Comparison)
      {
	case Condition::eEQ :
	  t = wxT("==");
	  break;
	case Condition::eNE :
	  t = wxT("!=");
	  break;
	case Condition::eGT :
	  t = wxT(">");
	  break;
	case Condition::eGE :
	  t = wxT(">=");
	  break;
	case Condition::eLT :
	  t = wxT("<");
	  break;
	case Condition::eLE :
	  t = wxT("<=");
	  break;
	default :
	  t = wxT("(Undef)");
      }
      combo_box_compare->SetValue(t);
      combo_box_para2->SetValue(data->TestParam2);
      return;
    }
  }
  pane_output->Hide();
  pane_condition->Hide();
  sizer_12->Layout();
}

void 
SwitchPropertyDialog::OnTreeRightClick (wxTreeEvent & event)
{
  wxTreeItemId id = event.GetItem();
  tree_test->SelectItem(id);
  tree_test->PopupMenu(&menu);
}

void
SwitchPropertyDialog::OnApplyOutput (wxCommandEvent & event)
{
  wxTreeItemId id = tree_test->GetSelection();
  if ( id.IsOk() )
  {
    TreeItemTest * test = dynamic_cast<TreeItemTest *>(tree_test->GetItemData(id));
    test->output = combo_box_output->GetValue();
    wxString txt;
    switch (test->logic)
    {
    case Test::eAND :
       txt = wxT("AND");
       break;
    case Test::eOR :
       txt = wxT("OR");
       break;
    case Test::eDefault :
       txt = wxT("Default");
       break;
    default:
       txt = wxT("Undefined");     
    }
    txt += wxT("(") + test->output + wxT(")");
    tree_test->SetItemText(id, txt);
  }
}

void
SwitchPropertyDialog::OnApplyCond (wxCommandEvent & event)
{
  wxTreeItemId id = tree_test->GetSelection();
  if ( id.IsOk() )
  {
    TreeItemSCond * cond = dynamic_cast<TreeItemSCond *>(tree_test->GetItemData(id));
    cond->TestParam1 = combo_box_para1->GetValue();
    cond->TestParam2 = combo_box_para2->GetValue();
    wxString t = combo_box_compare->GetValue();
    if ( t == wxT("==") )
      cond->Comparison = Condition::eEQ;
    else if ( t == wxT("!=") )
      cond->Comparison = Condition::eNE;
    else if ( t == wxT(">") )
      cond->Comparison = Condition::eGT;
    else if ( t == wxT(">=") )
      cond->Comparison = Condition::eGE;
    else if ( t == wxT("<") )
      cond->Comparison = Condition::eLT;
    else if ( t == wxT("<=") )
      cond->Comparison = Condition::eLE;
    else
      cond->Comparison = Condition::ecUndef;      
    wxString txt;
    txt = cond->TestParam1 + wxT(" ") + t + wxT(" ") + cond->TestParam2;
    tree_test->SetItemText(id, txt);
  }
}

void
SwitchPropertyDialog::OnUpdateMenu(wxUpdateUIEvent & event)
{
  wxTreeItemId id = tree_test->GetSelection();
  wxTreeItemId root = tree_test->GetRootItem();
  switch (event.GetId())
  {
  case    MENUITEM_DEL :
    if (id == root)
    {
      event.Enable(false);
    }
    else if ( TreeItemTest * data = dynamic_cast<TreeItemTest *>(tree_test->GetItemData(id)) )
    {
      if (data->logic == Test::eDefault)
        event.Enable(false);
      else
	event.Enable(true);
    }
    else
    {
      event.Enable(true);    
    }
    break;
  case    MENUITEM_ADD_SCOND :
  case    MENUITEM_ADD_CCOND_AND :
  case    MENUITEM_ADD_CCOND_OR :
    if (id == root)
    {
      event.Enable(false);
    }
    else if ( TreeItemTest * data = dynamic_cast<TreeItemTest *>(tree_test->GetItemData(id)) )
    {
      if (data->logic == Test::eDefault)
        event.Enable(false);
      else
	event.Enable(true);
    }
    else if ( TreeItemCCond * data = dynamic_cast<TreeItemCCond *>(tree_test->GetItemData(id)) )
    {
	event.Enable(true);
    }
    else
    {
      event.Enable(false);    
    }
    break;
  case    MENUITEM_ADD_TEST_AND :
  case    MENUITEM_ADD_TEST_OR :
    if (id == root)
    {
      event.Enable(true);
    }
    else
    {
      event.Enable(false);    
    }
    break;
  default:
    event.Skip();
    break;
  }
}

void
SwitchPropertyDialog::OnMenuDel (wxCommandEvent & event)
{
  wxTreeItemId id = tree_test->GetSelection();
  tree_test->Delete(id);
}

void
SwitchPropertyDialog::OnMenuAddSCond (wxCommandEvent & event)
{
  wxTreeItemId parent = tree_test->GetSelection();
  wxTreeItemId child;
  TreeItemSCond * sdata = new TreeItemSCond;
  sdata->TestParam1 = wxT("unknown");
  sdata->TestParam2 = wxT("unknown");
  sdata->Comparison = Condition::ecUndef ;
  wxString t = sdata->TestParam1;
  t += wxT(" (Undef) ");
  t += sdata->TestParam2;
  child = tree_test->AppendItem(parent, t, -1, -1, sdata);
  sdata->SetId(child);
  tree_test->Refresh();
  tree_test->Update();
}

void
SwitchPropertyDialog::OnAddCCondAND (wxCommandEvent & event)
{
  wxTreeItemId parent = tree_test->GetSelection();
  wxTreeItemId child;
  TreeItemCCond * cdata = new TreeItemCCond;
  cdata->Logic = Condition::eAND;
  wxString txt = wxT("AND");
  child = tree_test->AppendItem(parent, txt, -1, -1, cdata);
  cdata->SetId(child);
  tree_test->Refresh();
  tree_test->Update();
}

void
SwitchPropertyDialog::OnMenuCCondOR (wxCommandEvent & event)
{
  wxTreeItemId parent = tree_test->GetSelection();
  wxTreeItemId child;
  TreeItemCCond * cdata = new TreeItemCCond;
  cdata->Logic = Condition::eOR;
  wxString txt = wxT("OR");
  child = tree_test->AppendItem(parent, txt, -1, -1, cdata);
  cdata->SetId(child);
  tree_test->Refresh();
  tree_test->Update();
}

void
SwitchPropertyDialog::OnMenuTestAND (wxCommandEvent & event)
{
  wxTreeItemId parent = tree_test->GetSelection();
  wxTreeItemId child;
  TreeItemTest * data = new TreeItemTest;
  data->logic = Test::eAND;
  data->output = wxT("0.0");
  wxString txt = wxT("AND(0.0)");
  child = tree_test->AppendItem(parent, txt, -1, -1, data);
  data->SetId(child);
  tree_test->Refresh();
  tree_test->Update();
}

void
SwitchPropertyDialog::OnMenuTestOR (wxCommandEvent & event)
{
  wxTreeItemId parent = tree_test->GetSelection();
  wxTreeItemId child;
  TreeItemTest * data = new TreeItemTest;
  data->logic = Test::eOR;
  wxString txt = wxT("OR(0.0)");
  data->output = wxT("0.0");
  child = tree_test->AppendItem(parent, txt, -1, -1, data);
  data->SetId(child);
  tree_test->Refresh();
  tree_test->Update();
}


