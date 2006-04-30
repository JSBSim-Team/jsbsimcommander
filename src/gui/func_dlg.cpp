/////////////////////////////////////////////////////////////////////////////
// Name:        func_dlg.cpp
// Purpose:     dialog for FGFCSFunction
// Author:      Matthew Gong
// Created:     07/24/2005
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
#include <wx/treectrl.h>
#include <wx/txtstrm.h>
#include <wx/sstream.h>
#include <wx/datetime.h>

#include "TreeItem.h"
#include "FGXMLParse.h"

#include "shape.h"
#include "func.h"
#include "func_dlg.h"

#ifndef __WXMSW__
#include "aero_icon.xpm"
#include "fcs_icon.xpm"
#include "mass_icon.xpm"
#include "ap_icon.xpm"
#include "fileheader_icon.xpm"
#include "metrics_icon.xpm"
#include "engine_icon.xpm"
#include "gear_icon.xpm"
#include "output_icon.xpm"
#include "axis.xpm"
#include "operator_abs.xpm"
#include "operator_derivative.xpm"
#include "operator_product.xpm"
#include "operator_acos.xpm"
#include "operator_difference.xpm"
#include "operator_quotient.xpm"
#include "operator_asin.xpm"
#include "operator_func.xpm"
#include "operator_sin.xpm"
#include "operator_atan.xpm"
#include "operator_integral.xpm"
#include "operator_sum.xpm"
#include "operator_cos.xpm"
#include "operator_pow.xpm"
#include "operator_tan.xpm"
#include "property_icon.xpm"
#include "table_icon.xpm"
#include "value_icon.xpm"
#include "myclose.xpm"
#include "mynew.xpm"
#include "myopen.xpm"
#include "mysaveas.xpm"
#include "mysave.xpm"
#include "rocket_engine.xpm"
#include "piston_engine.xpm"
#include "turbine_engine.xpm"
#include "propeller.xpm"
#include "nozzle.xpm"
#include "direct.xpm"
#include "electric_engine.xpm"
#include "tank.xpm"
#include "operator_le.xpm"
#include "operator_lt.xpm"
#include "operator_gt.xpm"
#include "operator_ge.xpm"
#include "operator_eq.xpm"
#include "operator_ne.xpm"
#include "operator_and.xpm"
#include "operator_or.xpm"
#include "operator_not.xpm"
#include "operator_ifthen.xpm"

#endif


BEGIN_EVENT_TABLE (FCSFunctionDialog, wxDialog)
  EVT_CHECKBOX(ID_CLIPABLE, FCSFunctionDialog::OnClipable)
  EVT_BUTTON  (wxID_OK, FCSFunctionDialog::OnOK)
  EVT_LISTBOX (ID_INPUT_LISTBOX, FCSFunctionDialog::OnInputListBox)
  EVT_RADIOBOX (ID_INPUT_RADIO_SET, FCSFunctionDialog::OnInputRadioSet)
  EVT_BUTTON  (ID_INPUT_ADD, FCSFunctionDialog::OnInputAdd)
  EVT_BUTTON  (ID_INPUT_REMOVE, FCSFunctionDialog::OnInputRemove)
END_EVENT_TABLE ()

FCSFunctionDialog::FCSFunctionDialog(FCSFunction * componment, wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, style)
{
    // begin wxGlade: FCSFunctionDialog::FCSFunctionDialog
    notebook_Main_pane = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, 0);
    notebook_Main_pane_input = new wxPanel(notebook_Main_pane, -1);
    notebook_Main_pane_basic = new wxPanel(notebook_Main_pane, -1);
    notebook_Main_pane_tree = new wxPanel(notebook_Main_pane, -1);
    sizer_7_staticbox = new wxStaticBox(notebook_Main_pane_basic, -1, _("cliper"));
    label_name = new wxStaticText(notebook_Main_pane_basic, -1, _("Name:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    label_Type = new wxStaticText(notebook_Main_pane_basic, -1, _("Type:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    label_order = new wxStaticText(notebook_Main_pane_basic, -1, _("Order:"));
    text_ctrl_name = new wxTextCtrl(notebook_Main_pane_basic, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NONE, &name) );
    label_type = new wxStaticText(notebook_Main_pane_basic, -1, wxT("FUNCTION"));
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

    tree_func = new wxTreeCtrl(notebook_Main_pane_tree, TREE, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS|wxTR_DEFAULT_STYLE);

    button_OK = new wxButton(this, wxID_OK, _("OK"));
    button_Cancel = new wxButton(this, wxID_CANCEL, _("Cancel"));

    set_properties();
    do_layout();
    // end wxGlade

    GetDataIn(componment);
    tree_func->Expand(root);
}


void FCSFunctionDialog::set_properties()
{
  // begin wxGlade: FCSFunctionDialog::set_properties
  button_Cancel->SetFocus();
  // end wxGlade
  
  TreeItemEvtHandler * tree_item_evt_handler = new TreeItemEvtHandler(tree_func);
  tree_item_evt_handler->FuncPrefix = wxT("fcs/function/func");
  tree_func->PushEventHandler(tree_item_evt_handler);

  wxBitmap bmp(wxBITMAP(fileheader_icon));
  wxImageList * images = new wxImageList(bmp.GetWidth(), bmp.GetHeight());
  images->Add(bmp); // 0
  images->Add(wxBITMAP(metrics_icon));
  images->Add(wxBITMAP(mass_icon));
  images->Add(wxBITMAP(gear_icon));
  images->Add(wxBITMAP(engine_icon));
  images->Add(wxBITMAP(ap_icon)); // 5
  images->Add(wxBITMAP(fcs_icon));
  images->Add(wxBITMAP(aero_icon));
  images->Add(wxBITMAP(output_icon));
  images->Add(wxBITMAP(axis));
  images->Add(wxBITMAP(operator_product)); //10
  images->Add(wxBITMAP(operator_difference));
  images->Add(wxBITMAP(operator_sum));
  images->Add(wxBITMAP(operator_quotient));
  images->Add(wxBITMAP(operator_pow));
  images->Add(wxBITMAP(operator_abs));   //15
  images->Add(wxBITMAP(operator_sin));
  images->Add(wxBITMAP(operator_cos));
  images->Add(wxBITMAP(operator_tan));
  images->Add(wxBITMAP(operator_asin));
  images->Add(wxBITMAP(operator_acos)); //20
  images->Add(wxBITMAP(operator_atan));
  images->Add(wxBITMAP(operator_derivative));
  images->Add(wxBITMAP(operator_integral));
  images->Add(wxBITMAP(operator_func));
  images->Add(wxBITMAP(property_icon));  //25
  images->Add(wxBITMAP(table_icon));
  images->Add(wxBITMAP(value_icon));
  images->Add(wxBITMAP(piston_engine));
  images->Add(wxBITMAP(turbine_engine));
  images->Add(wxBITMAP(rocket_engine));   //30
  images->Add(wxBITMAP(electric_engine));
  images->Add(wxBITMAP(propeller));
  images->Add(wxBITMAP(nozzle));
  images->Add(wxBITMAP(direct));
  images->Add(wxBITMAP(tank));  //35
  images->Add(wxBITMAP(operator_lt));
  images->Add(wxBITMAP(operator_le));
  images->Add(wxBITMAP(operator_gt));
  images->Add(wxBITMAP(operator_ge));
  images->Add(wxBITMAP(operator_eq));  //40
  images->Add(wxBITMAP(operator_ne));
  images->Add(wxBITMAP(operator_and));
  images->Add(wxBITMAP(operator_or));
  images->Add(wxBITMAP(operator_not));
  images->Add(wxBITMAP(operator_ifthen));  //45

  tree_func->AssignImageList(images);
}


void FCSFunctionDialog::do_layout()
{
    // begin wxGlade: FCSFunctionDialog::do_layout
    wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_14 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_3 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_13 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_12 = new wxBoxSizer(wxVERTICAL);
    
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
       
    sizer_12->Add(tree_func, 1, wxALL|wxEXPAND, 5);
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
FCSFunctionDialog::Show( bool show)
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
FCSFunctionDialog::OnOK(wxCommandEvent & event )
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
FCSFunctionDialog::OnClipable(wxCommandEvent & WXUNUSED(event) )
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
FCSFunctionDialog::GetDataIn(FCSFunction * s)
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

  wxStringInputStream istr(s->function);
  wxTextInputStream tistr(istr);
  std::stringstream os;
  while (!istr.Eof())
  {
    os << tistr.ReadLine().c_str() << '\n';
  }
  std::istream is(os.rdbuf());
  JSBSim::FGXMLParse p;
  try
  {
    readXML(is, p);
  }
  catch (...)
  {
    wxLogError(wxT("Fail to parse it"));
    //TODO
    tree_func->DeleteAllItems();
    function_data = new TreeItemData_function;
    function_data->Init(tree_func);
    function_data->name = wxString::Format("fcs/function/func%lu", wxDateTime::Now().GetTicks());
    function_data->type = wxT("function");
    function_data->Type = TreeItemData_function::eTopLevel;
    root = tree_func->AddRoot(function_data->name, 24, 24, function_data);
    return;
  }
  
  tree_func->DeleteAllItems();
  function_data = new TreeItemData_function;
  root = tree_func->AddRoot(wxT("root"), 24, 24, function_data);
  function_data->Init(tree_func);
  function_data->Load(p.GetDocument());

  tree_func->Expand(root);
}

void
FCSFunctionDialog::SetDataOut(FCSFunction * s)
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

  if (tree_func->GetCount() <= 1 || !root.IsOk() || function_data->Type != TreeItemData_function::eTopLevel)
  {
    return;
  }

  s->function.Clear();
  wxStringOutputStream os(&(s->function));
  wxTextOutputStream out(os,wxEOL_UNIX);
  out << wxT("<?xml version=\"1.0\"?>\n");
  function_data->Save(out, wxEmptyString);
}

void 
FCSFunctionDialog::OnInputListBox (wxCommandEvent & WXUNUSED(event))
{
  wxString str = list_box_input->GetStringSelection();
  if ( str == wxT("negative") )
    radio_box_set->SetSelection(1);
  else
    radio_box_set->SetSelection(0);
}


void 
FCSFunctionDialog::OnInputRadioSet (wxCommandEvent & WXUNUSED(event))
{
  int pos =  list_box_input->GetSelection();
  if ( radio_box_set->GetSelection() == 0 )
    list_box_input->SetString(pos,wxT("positive"));
  else
    list_box_input->SetString(pos,wxT("negative"));
}


void 
FCSFunctionDialog::OnInputAdd (wxCommandEvent & WXUNUSED(event))
{
  list_box_input->Append(wxT("positive"));
  wxString tmp = wxT("$INPUT") + wxString::Format("%d",list_box_input->GetCount());
}


void 
FCSFunctionDialog::OnInputRemove (wxCommandEvent & WXUNUSED(event))
{
  if ( list_box_input->GetCount() <= 1)
    return;

  wxString tmp = wxT("$INPUT") + wxString::Format("%d",list_box_input->GetCount());
  
  int pos =  list_box_input->GetSelection();
  list_box_input->Delete(pos);
}


