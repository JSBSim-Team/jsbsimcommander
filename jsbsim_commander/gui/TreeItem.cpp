/////////////////////////////////////////////////////////////////////////////
// Name:        Aircraft_Diag.cpp
// Purpose:     Manager of the Aircraft
// Author:      Matthew Gong
// Created:     05/04/2005
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

#include <wx/wfstream.h>
#include <wx/datetime.h>

#include "FGXMLElement.h"
#include "FGXMLParse.h"

#include "TreeItem.h"
#include "Commander.h"
#include "AeroDynamics_Diag.h"
#include "Axis_Diag.h"
#include "Function_Diag.h"
#include "Table_Diag.h"
#include "Property_Diag.h"

BEGIN_EVENT_TABLE(TreeItemEvtHandler, wxEvtHandler)
  EVT_TREE_ITEM_MENU(TREE, TreeItemEvtHandler::OnRightClick) 
  EVT_TREE_ITEM_ACTIVATED(TREE, TreeItemEvtHandler::OnDoubleClick)
  EVT_MENU(wxID_PROPERTIES, TreeItemEvtHandler::OnProperties)
  EVT_MENU(EXPRESSION, TreeItemEvtHandler::OnExpression)
  EVT_MENU(wxID_DELETE, TreeItemEvtHandler::OnDelete)
  EVT_MENU(wxID_COPY, TreeItemEvtHandler::OnCopy)
  EVT_MENU(wxID_CUT, TreeItemEvtHandler::OnCut)
  EVT_MENU(wxID_PASTE, TreeItemEvtHandler::OnPaste)
  EVT_MENU(ADD_AXIS, TreeItemEvtHandler::OnAddAxis)
  EVT_MENU(ADD_FUNC, TreeItemEvtHandler::OnAddFunc)
  EVT_MENU(ADD_PROP, TreeItemEvtHandler::OnAddProp)
  EVT_MENU(ADD_VAL, TreeItemEvtHandler::OnAddVal)
  EVT_MENU(ADD_TABLE, TreeItemEvtHandler::OnAddTable)
  EVT_UPDATE_UI (wxID_PASTE, TreeItemEvtHandler::OnShowPaste)
  EVT_UPDATE_UI_RANGE (ADD_FUNC, ADD_AXIS, TreeItemEvtHandler::OnShowAddItem)
END_EVENT_TABLE()

void TreeItemEvtHandler::OnRightClick(wxTreeEvent & event)
{
  wxTreeItemId cur = event.GetItem();
  tree->SelectItem(cur);

  wxMenu menu;

  TreeItemData_base * data = dynamic_cast<TreeItemData_base *>(tree->GetItemData(cur));

  if (data)
  {
    data->PrepareMenu(menu);
    tree->PopupMenu(&menu);
  }
}

void TreeItemEvtHandler::OnDoubleClick(wxTreeEvent & event)
{
  wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, wxID_PROPERTIES);
  ProcessEvent(e);
}

void TreeItemEvtHandler::OnProperties(wxCommandEvent & event)
{
  if (TreeItemData_base * data = dynamic_cast<TreeItemData_base *>(tree->GetItemData(tree->GetSelection())))
  {
    data->ShowDialog();
  }
}

void TreeItemEvtHandler::OnExpression(wxCommandEvent & event)
{
  TreeItemData_ebase * data = dynamic_cast<TreeItemData_ebase *>(tree->GetItemData(tree->GetSelection()));
  if (data)
  {
    ::wxMessageBox( data->GetExpression(), _("Expression"), wxOK|wxICON_INFORMATION, tree);
  }
}

void TreeItemEvtHandler::OnDelete(wxCommandEvent & event)
{
  if (::wxMessageBox( _("Are you sure to delete it?"), _("Confirm"), wxYES_NO|wxICON_QUESTION, tree) == wxYES)
  {
    tree->Delete(tree->GetSelection());
  }
}

void TreeItemEvtHandler::OnCopy(wxCommandEvent & event)
{
  TreeItemData_ebase * data = dynamic_cast<TreeItemData_ebase *>(tree->GetItemData(tree->GetSelection()));
  if (data)
  {
    flag_copy_cut = 1;
    id_copy_cut = data->GetId();
  }
}

void TreeItemEvtHandler::OnCut(wxCommandEvent & event)
{
  TreeItemData_ebase * data = dynamic_cast<TreeItemData_ebase *>(tree->GetItemData(tree->GetSelection()));
  if (data)
  {
    flag_copy_cut = 2;
    id_copy_cut = data->GetId();
  }
}

void TreeItemEvtHandler::OnPaste(wxCommandEvent & event)
{
  TreeItemData_base * data = dynamic_cast<TreeItemData_ebase *>(tree->GetItemData(tree->GetSelection()));
  if (!data)
    data =  dynamic_cast<TreeItemData_func_root *>(tree->GetItemData(tree->GetSelection()));
  if (data && (flag_copy_cut == 1 || flag_copy_cut == 2))
  {
    wxTreeItemId cur = data->GetId();
    if (cur != id_copy_cut)
    {
      Copy(id_copy_cut, cur);
      if (flag_copy_cut == 2)
      {
	tree->Delete(id_copy_cut);
        flag_copy_cut = 0;
      }
      tree->Refresh();
      tree->Update();
    }
  }
}

void TreeItemEvtHandler::Copy(wxTreeItemId & from, wxTreeItemId & to)
{
  TreeItemData_ebase * ddata = NULL;
  if (TreeItemData_function * sdata = dynamic_cast<TreeItemData_function *>(tree->GetItemData(from)))
  {
    ddata = new TreeItemData_function(*sdata);
  }
  else if (TreeItemData_axis * sdata = dynamic_cast<TreeItemData_axis *>(tree->GetItemData(from)))
  {
    ddata = new TreeItemData_axis(*sdata);
  }
  else if (TreeItemData_Property * sdata = dynamic_cast<TreeItemData_Property *>(tree->GetItemData(from)))
  {
    ddata = new TreeItemData_Property(*sdata);
  }
  else if (TreeItemData_Value * sdata = dynamic_cast<TreeItemData_Value *>(tree->GetItemData(from)))
  {
    ddata = new TreeItemData_Value(*sdata);
  }
  else if (TreeItemData_Table * sdata = dynamic_cast<TreeItemData_Table *>(tree->GetItemData(from)))
  {
    ddata = new TreeItemData_Table(*sdata);
  }
  
  if (ddata)
  {
    wxTreeItemId id = tree->AppendItem(to, tree->GetItemText(from), tree->GetItemImage(from), tree->GetItemImage(from, wxTreeItemIcon_Selected), ddata );

    wxTreeItemIdValue cookie = (wxTreeItemIdValue)0501217LU;
    for (wxTreeItemId node = tree->GetFirstChild(from, cookie); node.IsOk(); node = tree->GetNextChild(from, cookie) )
    {
      Copy(node, id);
    }
  }
}

void TreeItemEvtHandler::OnAddAxis(wxCommandEvent & event)
{
  if (tree->GetSelection().IsOk())
  {
    TreeItemData_axis * data = new TreeItemData_axis;
    wxTreeItemId id= tree->AppendItem(tree->GetSelection(), wxEmptyString, -1, -1, data );
    data->Init(tree);
    tree->Expand(tree->GetSelection());
    tree->EnsureVisible(id);
    if (data->ShowDialog() != wxID_OK)
    {
      tree->Delete(id);
    }
  }  
}


void TreeItemEvtHandler::OnAddFunc(wxCommandEvent & event)
{ 
  if (tree->GetSelection().IsOk())
  {
    unsigned long t = wxDateTime::Now().GetTicks();
    wxTreeItemId id;
    TreeItemData_function * data = new TreeItemData_function;
    if (dynamic_cast<TreeItemData_aerodynamics *>(tree->GetItemData(tree->GetSelection())))
    {
      id = tree->PrependItem(tree->GetSelection(), wxEmptyString, -1, -1, data );
      data->Init(tree);
      data->Type = TreeItemData_function::eTopLevel;
      data->type = wxT("function");
      data->name = wxString::Format("aero/function/func%lu", t);
    }
    else if (dynamic_cast<TreeItemData_axis *>(tree->GetItemData(tree->GetSelection())))
    {
      id = tree->AppendItem(tree->GetSelection(), wxEmptyString, -1, -1, data );
      data->Init(tree);
      data->Type = TreeItemData_function::eTopLevel;
      data->type = wxT("function");
      data->name = wxString::Format("aero/function/func%lu", t);
    }
    else if (dynamic_cast<TreeItemData_script_root *>(tree->GetItemData(tree->GetSelection())))
    {
      id = tree->AppendItem(tree->GetSelection(), wxEmptyString, -1, -1, data );
      data->Init(tree);
      data->Type = TreeItemData_function::eTopLevel;
      data->type = wxT("function");
      data->name = wxString::Format("script/func%lu", t);
    }
    else if (dynamic_cast<TreeItemData_func_root *>(tree->GetItemData(tree->GetSelection())))
    {
      id = tree->AppendItem(tree->GetSelection(), wxEmptyString, -1, -1, data );
      data->Init(tree);
      data->Type = TreeItemData_function::eTopLevel;
      data->type = wxT("function");
      data->name = wxString::Format("function/func%lu", t);
    }
    else
    {
      id = tree->AppendItem(tree->GetSelection(), wxEmptyString, -1, -1, data );
      data->Init(tree);
    }
    tree->Expand(tree->GetSelection());
    tree->EnsureVisible(id);
    if (data->ShowDialog()!= wxID_OK)
    {
      tree->Delete(id);
    }
  }
}

void TreeItemEvtHandler::OnAddProp(wxCommandEvent & event)
{
  if (tree->GetSelection().IsOk())
  {
    TreeItemData_Property * data = new TreeItemData_Property;
    wxTreeItemId id = tree->AppendItem(tree->GetSelection(), wxEmptyString, -1, -1, data );
    data->Init(tree);
    tree->Expand(tree->GetSelection());
    tree->EnsureVisible(id);
    if (data->ShowDialog() != wxID_OK)
    {
      tree->Delete(id);
    }
  }
}

void TreeItemEvtHandler::OnAddVal(wxCommandEvent & event)
{
  if (tree->GetSelection().IsOk())
  {
    TreeItemData_Value * data = new TreeItemData_Value;
    wxTreeItemId id = tree->AppendItem(tree->GetSelection(), wxEmptyString, -1, -1, data );
    data->Init(tree);
    tree->Expand(tree->GetSelection());
    tree->EnsureVisible(id);
    if (data->ShowDialog() != wxID_OK)
    {
      tree->Delete(id);
    }
  }
}

void TreeItemEvtHandler::OnAddTable(wxCommandEvent & event)
{
  //TODO
  if (tree->GetSelection().IsOk())
  {
    TreeItemData_Table * data = new TreeItemData_Table;
    wxTreeItemId ptr = tree->AppendItem(tree->GetSelection(), wxEmptyString, -1, -1, data );
    data->Init(tree);
    tree->Expand(tree->GetSelection());
    tree->EnsureVisible(ptr);
    bool result=true;

    data->name = ::wxGetTextFromUser(_("Name:"), _("Input Name of Table"), wxEmptyString, tree);
    
    if (result)
    {
      const wxString tt[]={wxT("tt1D"), wxT("tt2D"), wxT("tt3D"), wxT("tt4D")};
      switch ( ::wxGetSingleChoiceIndex(_("Type:"), _("select type of table"), 4, tt, tree) )
      {
        case 0 :
          data->Type = TreeItemData_Table::tt1D;
          break;
        case 1 :
          data->Type = TreeItemData_Table::tt2D;
          break;
        case 2 :
          data->Type = TreeItemData_Table::tt3D;
          break;
        case 3 :
          data->Type = TreeItemData_Table::tt4D;
          break;
        default:
          data->Type = TreeItemData_Table::tUndef;
          result = false;
      }
    }

    if (!result)
    {
      tree->Delete(ptr);
    }
  }
}

void TreeItemEvtHandler::OnShowPaste(wxUpdateUIEvent & event)
{
  if (flag_copy_cut == 1 || flag_copy_cut == 2)
  {
   if (TreeItemData_function * data = dynamic_cast<TreeItemData_function *>(tree->GetItemData(tree->GetSelection())))
   {
    switch (data->Type)
    {
      case TreeItemData_function::eTopLevel :
      case TreeItemData_function::eAbs :
      case TreeItemData_function::eSin :
      case TreeItemData_function::eCos :
      case TreeItemData_function::eTan :
      case TreeItemData_function::eASin :
      case TreeItemData_function::eACos :
      case TreeItemData_function::eATan :
      case TreeItemData_function::eDerivative :
      case TreeItemData_function::eIntegral :
      case TreeItemData_function::eNOT :
        {
          if (tree->GetChildrenCount(data->GetId(), false) < 1u)
          {
            event.Enable(true);
          }
          else
          {
            event.Enable(false);  
          }
        }
        break;
      case TreeItemData_function::ePow :
      case TreeItemData_function::eLT :
      case TreeItemData_function::eLE :
      case TreeItemData_function::eGE :
      case TreeItemData_function::eGT :
      case TreeItemData_function::eEQ :
      case TreeItemData_function::eNE :
        {
          if (tree->GetChildrenCount(data->GetId(), false) <  2u)
          {
            event.Enable(true);
          }
          else
          {
            event.Enable(false);  
          }        
        }
        break;
      case TreeItemData_function::eIFTHEN :
        {
          if (tree->GetChildrenCount(data->GetId(), false) < 3u)
          {
            event.Enable(true);
          }
          else
          {
            event.Enable(false);  
          }        
        }
        break;
      default :
        {
          event.Enable(true);
        }
        break;
    }
   }
   else
    event.Enable(true);
  }
  else
  {
    event.Enable(false);  
  }
}

void TreeItemEvtHandler::OnShowAddItem(wxUpdateUIEvent & event)
{
  if (TreeItemData_function * data = dynamic_cast<TreeItemData_function *>(tree->GetItemData(tree->GetSelection())))
  {
    switch (data->Type)
    {
      case TreeItemData_function::eTopLevel :
      case TreeItemData_function::eAbs :
      case TreeItemData_function::eSin :
      case TreeItemData_function::eCos :
      case TreeItemData_function::eTan :
      case TreeItemData_function::eASin :
      case TreeItemData_function::eACos :
      case TreeItemData_function::eATan :
      case TreeItemData_function::eDerivative :
      case TreeItemData_function::eIntegral :
      case TreeItemData_function::eNOT :
        {
          if (tree->GetChildrenCount(data->GetId(), false) < 1u)
          {
            event.Enable(true);
          }
          else
          {
            event.Enable(false);  
          }
        }
        break;
      case TreeItemData_function::ePow :
      case TreeItemData_function::eLT :
      case TreeItemData_function::eLE :
      case TreeItemData_function::eGE :
      case TreeItemData_function::eGT :
      case TreeItemData_function::eEQ :
      case TreeItemData_function::eNE :
        {
          if (tree->GetChildrenCount(data->GetId(), false) <  2u)
          {
            event.Enable(true);
          }
          else
          {
            event.Enable(false);  
          }        
        }
        break;
      case TreeItemData_function::eIFTHEN :
        {
          if (tree->GetChildrenCount(data->GetId(), false) < 3u)
          {
            event.Enable(true);
          }
          else
          {
            event.Enable(false);  
          }        
        }
        break;
      default :
        {
          event.Enable(true);
        }
        break;
    }
  }
}



void TreeItemData_aerodynamics::Init(wxTreeCtrl* t)
{
    TreeItemData_base::Init(t);

    has_alphalimits = false;
    alphaclmin = -20;
    alphaclmax = 90;
    has_hysteresis_limits = false;
    alphahystmin = 30;
    alphahystmax = 30;    
}

void TreeItemData_aerodynamics::Load(JSBSim::Element * el)
{
  JSBSim::Element * temp_element;
  if (temp_element = el->FindElement("alphalimits")) {
    has_alphalimits = true;
    alphaclmin = temp_element->FindElementValueAsNumberConvertTo("min", "DEG");
    alphaclmax = temp_element->FindElementValueAsNumberConvertTo("max", "DEG");
  }
  else {
    has_alphalimits = false;
  }

  if (temp_element = el->FindElement("hysteresis_limits")) {
    has_hysteresis_limits = true;
    alphahystmin = temp_element->FindElementValueAsNumberConvertTo("min", "DEG");
    alphahystmax = temp_element->FindElementValueAsNumberConvertTo("max", "DEG");
  }
  else {
    has_hysteresis_limits = false;
  }

  tree->DeleteChildren(GetId());

  JSBSim::Element * el_func;
  el_func = el->FindElement("function");
  while (el_func) {
    TreeItemData_function * tmp = new TreeItemData_function;
    tree->AppendItem(GetId(), wxT("Function"), 24, 24, tmp);
    tmp->Init(tree);
    tmp->Load(el_func);

    el_func = el->FindNextElement("function");
  }

  JSBSim::Element * el_axis;
  el_axis = el->FindElement("axis");
  while (el_axis) {
    TreeItemData_axis * tmp = new TreeItemData_axis;
    tree->AppendItem(GetId(), wxT("Axis"), 9, 9, tmp);
    tmp->Init(tree);
    tmp->Load(el_axis);

    el_axis = el->FindNextElement("axis");
  }

}

void TreeItemData_aerodynamics::Save(wxTextOutputStream & out, const wxString & prefix)
{
  out << prefix << wxT("<aerodynamics>\n");
  wxString pre = prefix + wxT("    ");
  if (has_alphalimits)
  {
    out << pre << wxT("<alphalimits>\n");
    out << pre << wxT("    <min unit=\"DEG\"> ") << alphaclmin << wxT("</min>\n");
    out << pre << wxT("    <max unit=\"DEG\"> ") << alphaclmax << wxT("</max>\n");
    out << pre << wxT("</alphalimits>\n");
  }
  if (has_hysteresis_limits)
  {
    out << pre << wxT("<hysteresis_limits>\n");
    out << pre << wxT("    <min unit=\"DEG\"> ") << alphahystmin << wxT("</min>\n");
    out << pre << wxT("    <max unit=\"DEG\"> ") << alphahystmax << wxT("</min>\n");
    out << pre << wxT("</hysteresis_limits>\n");  
  }
  
  wxTreeItemIdValue cookie = (wxTreeItemIdValue)0401211LU;
  for (wxTreeItemId node = tree->GetFirstChild(GetId(), cookie); node.IsOk(); node = tree->GetNextChild(GetId(), cookie) )
  {
    TreeItemData_ebase * tmp = dynamic_cast<TreeItemData_ebase *>(tree->GetItemData(node));
    if (tmp)
      tmp->Save(out, pre);
  }
  out << prefix << wxT("</aerodynamics>\n");
}

void TreeItemData_aerodynamics::PrepareMenu(wxMenu & menu)
{
    menu.Append(wxID_PROPERTIES, _("&Properties"), _("Detail of Function or operator"));
    menu.AppendSeparator();
    menu.Append(ADD_FUNC, _("Add &Func"), _("Add Function or operator"));
    menu.Append(ADD_AXIS, _("Add &Axis"), _("Add Axis"));
}

int TreeItemData_aerodynamics::ShowDialog(void * data)
{
  int result;
  AeroDynamicsDialog dlg(tree, -1);
  dlg.Load(this);
  if ( (result = dlg.ShowModal()) == wxID_OK)
  {
    dlg.Save(this);
  }
  return result;
}

void TreeItemData_axis::Init(wxTreeCtrl* t)
{
    TreeItemData_base::Init(t);

    name = wxT("Undef");
    unit = wxEmptyString;
    tree->SetItemText(GetId(), name);
    tree->SetItemImage(GetId(), 9);
    tree->SetItemImage(GetId(), 9, wxTreeItemIcon_Selected);
}

void TreeItemData_axis::Load(JSBSim::Element * el)
{
  name = el->GetAttributeValue("name");
  unit = el->GetAttributeValue("unit");
  if (unit.IsEmpty())
  {
    if (name == wxT("DRAG") || name == wxT("SIDE") || name == wxT("LIFT") ||
        name == wxT("FORWARD") || name == wxT("RIGHT") || name == wxT("DOWN")
        )
      unit = wxT("LBS");
    else
      unit = wxT("LBS*FT");
  }
  tree->SetItemText(GetId(), name + wxT("(") + unit + wxT(")") );

  JSBSim::Element * el_func = el->FindElement("function");
  while (el_func) {
    TreeItemData_function * tmp = new TreeItemData_function;
    tree->AppendItem(GetId(), wxT("Function"), 24, 24, tmp);
    tmp->Init(tree);
    tmp->Load(el_func);

    el_func = el->FindNextElement("function");
  }

}

void TreeItemData_axis::Save(wxTextOutputStream & out, const wxString & prefix)
{
  if (unit.IsEmpty())
  {
    out << prefix << wxT("<axis name=\"") << name << wxT("\">\n");
  }
  else
  {
    out << prefix << wxT("<axis name=\"") << name << wxT("\" unit=\"") << unit << wxT("\">\n");
  } 
  wxString pre = prefix + wxT("    ");
  wxTreeItemIdValue cookie = (wxTreeItemIdValue)0401212LU;
  for (wxTreeItemId node = tree->GetFirstChild(GetId(), cookie); node.IsOk(); node = tree->GetNextChild(GetId(), cookie) )
  {
    TreeItemData_function * tmp = dynamic_cast<TreeItemData_function *>(tree->GetItemData(node));
    if (tmp)
      tmp->Save(out, pre);
  }
  out << prefix << wxT("</axis>\n");
}

void TreeItemData_axis::PrepareMenu(wxMenu & menu)
{
    menu.Append(wxID_PROPERTIES, _("&Properties"), _("Detail of Function or operator"));
    menu.Append(EXPRESSION, _("&Expression"), _("Expression of Function"));
    menu.AppendSeparator();
    menu.Append(wxID_DELETE, _("&Delete"), _("Delete Item"));
    menu.Append(wxID_COPY, _("&Copy"), _("Copy Item"));
    menu.Append(wxID_CUT, _("Cu&t"), _("Cut Item"));
    menu.Append(wxID_PASTE, _("&Paste"), _("Paste Item"));
    menu.AppendSeparator();
    menu.Append(ADD_FUNC, _("Add &Func"), _("Add Function or operator"));
}

wxString TreeItemData_axis::GetExpression()
{
  wxString result=wxEmptyString;
  bool first=true;
  wxTreeItemIdValue cookie = (wxTreeItemIdValue)0401213LU;
  for (wxTreeItemId node = tree->GetFirstChild(GetId(), cookie); node.IsOk(); node = tree->GetNextChild(GetId(), cookie) )
  {
    TreeItemData_function * tmp = dynamic_cast<TreeItemData_function *>(tree->GetItemData(node));
    if (tmp)
    {
      if (first)
      {
        first = false;
      }
      else
      {
	      result += wxT(" + ");      
      }
      TreeItemData_function::functionType childType = tmp->GetType();
      bool isAddBracket = TreeItemData_function::cmpPrior(TreeItemData_function::eSum, childType);
      if ( isAddBracket )
      {
        result += wxT("(");
      }
	    result += tmp->GetExpression();
      if ( isAddBracket )
      {
        result += wxT(")");        
      }
    }
  }
  return result;
}

int TreeItemData_axis::ShowDialog(void * data)
{
  int result;
  AxisDialog dlg(tree, -1);
  dlg.Load(this);
  if ( (result = dlg.ShowModal()) == wxID_OK)
  {
    dlg.Save(this);
  }
  return result;
}

void TreeItemData_function::Init(wxTreeCtrl* t)
{
    TreeItemData_base::Init(t);

    Type = eUndef;
    type = wxT("Undef");
    tree->SetItemText(GetId(), type);
    tree->SetItemImage(GetId(), -1);
    tree->SetItemImage(GetId(), -1, wxTreeItemIcon_Selected);
}

void TreeItemData_function::Load(JSBSim::Element * el)
{
  type = el->GetName();
  if ( type == wxT("function"))
  {
    Type = eTopLevel;
    name = el->GetAttributeValue("name");
    tree->SetItemText(GetId(), name);
    tree->SetItemImage(GetId(), 24);
    tree->SetItemImage(GetId(), 24, wxTreeItemIcon_Selected);
  }
  else if  ( type == wxT("product"))
  {
    Type = eProduct;
    tree->SetItemText(GetId(), type);
    tree->SetItemImage(GetId(), 10);
    tree->SetItemImage(GetId(), 10, wxTreeItemIcon_Selected);
  }
  else if  ( type == wxT("difference"))
  {
    Type = eDifference;
    tree->SetItemText(GetId(), type);
    tree->SetItemImage(GetId(), 11);
    tree->SetItemImage(GetId(), 11, wxTreeItemIcon_Selected);
  }
  else if  ( type == wxT("sum"))
  {
    Type = eSum;
    tree->SetItemText(GetId(), type);
    tree->SetItemImage(GetId(), 12);
    tree->SetItemImage(GetId(), 12, wxTreeItemIcon_Selected);
  }
  else if  ( type == wxT("quotient"))
  {
    Type = eQuotient;
    tree->SetItemText(GetId(), type);
    tree->SetItemImage(GetId(), 13);
    tree->SetItemImage(GetId(), 13, wxTreeItemIcon_Selected);
  }
  else if  ( type == wxT("pow"))
  {
    Type = ePow;
    tree->SetItemText(GetId(), type);
    tree->SetItemImage(GetId(), 14);
    tree->SetItemImage(GetId(), 14, wxTreeItemIcon_Selected);
  }
  else if  ( type == wxT("abs"))
  {
    Type = eAbs;
    tree->SetItemText(GetId(), type);
    tree->SetItemImage(GetId(), 15);
    tree->SetItemImage(GetId(), 15, wxTreeItemIcon_Selected);
  }
  else if  ( type == wxT("sin"))
  {
    Type = eSin;
    tree->SetItemText(GetId(), type);
    tree->SetItemImage(GetId(), 16);
    tree->SetItemImage(GetId(), 16, wxTreeItemIcon_Selected);
  }
  else if  ( type == wxT("cos"))
  {
    Type = eCos;
    tree->SetItemText(GetId(), type);
    tree->SetItemImage(GetId(), 17);
    tree->SetItemImage(GetId(), 17, wxTreeItemIcon_Selected);
  }
  else if  ( type == wxT("tan"))
  {
    Type = eTan;
    tree->SetItemText(GetId(), type);
    tree->SetItemImage(GetId(), 18);
    tree->SetItemImage(GetId(), 18, wxTreeItemIcon_Selected);
  }
  else if  ( type == wxT("asin"))
  {
    Type = eASin;
    tree->SetItemText(GetId(), type);
    tree->SetItemImage(GetId(), 19);
    tree->SetItemImage(GetId(), 19, wxTreeItemIcon_Selected);
  }
  else if  ( type == wxT("acos"))
  {
    Type = eACos;
    tree->SetItemText(GetId(), type);
    tree->SetItemImage(GetId(), 20);
    tree->SetItemImage(GetId(), 20, wxTreeItemIcon_Selected);
  }
  else if  ( type == wxT("atan"))
  {
    Type = eATan;
    tree->SetItemText(GetId(), type);
    tree->SetItemImage(GetId(), 21);
    tree->SetItemImage(GetId(), 21, wxTreeItemIcon_Selected);
  }
  else if  ( type == wxT("derivative"))
  {
    Type = eDerivative;
    tree->SetItemText(GetId(), type);
    tree->SetItemImage(GetId(), 22);
    tree->SetItemImage(GetId(), 22, wxTreeItemIcon_Selected);
  }
  else if  ( type == wxT("integral"))
  {
    Type = eIntegral;
    tree->SetItemText(GetId(), type);
    tree->SetItemImage(GetId(), 23);
    tree->SetItemImage(GetId(), 23, wxTreeItemIcon_Selected);
  }
  else if  ( type == wxT("lt"))
  {
    Type = eLT;
    tree->SetItemText(GetId(), type);
    tree->SetItemImage(GetId(), 36);
    tree->SetItemImage(GetId(), 36, wxTreeItemIcon_Selected);
  }
  else if  ( type == wxT("le"))
  {
    Type = eLE;
    tree->SetItemText(GetId(), type);
    tree->SetItemImage(GetId(), 37);
    tree->SetItemImage(GetId(), 37, wxTreeItemIcon_Selected);
  }
  else if  ( type == wxT("gt"))
  {
    Type = eGT;
    tree->SetItemText(GetId(), type);
    tree->SetItemImage(GetId(), 38);
    tree->SetItemImage(GetId(), 38, wxTreeItemIcon_Selected);
  }
  else if  ( type == wxT("ge"))
  {
    Type = eGE;
    tree->SetItemText(GetId(), type);
    tree->SetItemImage(GetId(), 39);
    tree->SetItemImage(GetId(), 39, wxTreeItemIcon_Selected);
  }
  else if  ( type == wxT("eq"))
  {
    Type = eEQ;
    tree->SetItemText(GetId(), type);
    tree->SetItemImage(GetId(), 40);
    tree->SetItemImage(GetId(), 40, wxTreeItemIcon_Selected);
  }
  else if  ( type == wxT("ne"))
  {
    Type = eNE;
    tree->SetItemText(GetId(), type);
    tree->SetItemImage(GetId(), 41);
    tree->SetItemImage(GetId(), 41, wxTreeItemIcon_Selected);
  }
  else if  ( type == wxT("and"))
  {
    Type = eAND;
    tree->SetItemText(GetId(), type);
    tree->SetItemImage(GetId(), 42);
    tree->SetItemImage(GetId(), 42, wxTreeItemIcon_Selected);
  }
  else if  ( type == wxT("or"))
  {
    Type = eOR;
    tree->SetItemText(GetId(), type);
    tree->SetItemImage(GetId(), 43);
    tree->SetItemImage(GetId(), 43, wxTreeItemIcon_Selected);
  }
  else if  ( type == wxT("not"))
  {
    Type = eNOT;
    tree->SetItemText(GetId(), type);
    tree->SetItemImage(GetId(), 44);
    tree->SetItemImage(GetId(), 44, wxTreeItemIcon_Selected);
  }
  else if  ( type == wxT("ifthen"))
  {
    Type = eIFTHEN;
    tree->SetItemText(GetId(), type);
    tree->SetItemImage(GetId(), 45);
    tree->SetItemImage(GetId(), 45, wxTreeItemIcon_Selected);
  }
  else
  {
    throw wxT("Unkown type");
  }

  description = wxEmptyString;
  
  JSBSim::Element * el_sub = el->GetElement();
  while (el_sub)
  {
    if (el_sub->GetName() == "description")
    {
      description = el_sub->GetDataLine();
      if (Type == eTopLevel)
      {
        tree->SetItemText(GetId(), name + wxT("(") + description + wxT(")"));
      }
      else
      {
        tree->SetItemText(GetId(), type + wxT("(") + description + wxT(")"));
      }
    }
    else
    {
      wxString type_sub = el_sub->GetName();
      TreeItemData_ebase * tmp;
      if ( type_sub == wxT("property"))
      {
         tmp = new TreeItemData_Property;      
      }
      else if (type_sub == wxT("value"))
      {
         tmp = new TreeItemData_Value;    
      }
      else if (type_sub == wxT("table"))
      {
         tmp = new TreeItemData_Table;
      }
      else
      {
         tmp = new TreeItemData_function;
      }
      tree->AppendItem(GetId(), wxEmptyString, -1, -1, tmp);
      tmp->Init(tree);
      tmp->Load(el_sub);
    }
    el_sub = el->GetNextElement();
  }
}

void TreeItemData_function::Save(wxTextOutputStream & out, const wxString & prefix)
{
  if (Type == eUndef)
  {
    return;
  }
  if (tree->GetChildrenCount(GetId(), false) == 0u)
  {
    return;
  }
  if (Type == eTopLevel)
  {
    out << prefix << wxT("<") << type << wxT(" name=\"") << name << wxT("\">\n");
  }
  else
  {
    out << prefix << wxT("<") << type << wxT(">\n");
  }
  
  wxString pre = prefix + wxT("    ");
  if (!description.IsEmpty())
  {
    out << pre << wxT("<description>") << description << wxT("</description>\n");
  }

  wxTreeItemIdValue cookie = (wxTreeItemIdValue)0401214LU;
  for (wxTreeItemId node = tree->GetFirstChild(GetId(), cookie); node.IsOk(); node = tree->GetNextChild(GetId(), cookie) )
  {
    TreeItemData_ebase * tmp = dynamic_cast<TreeItemData_ebase *>(tree->GetItemData(node));
    if (tmp)
      tmp->Save(out, pre);
  }
  
  out << prefix << wxT("</") << type << wxT(">\n");
}

void TreeItemData_function::PrepareMenu(wxMenu & menu)
{
    menu.Append(wxID_PROPERTIES, _("&Properties"), _("Detail of Function or operator"));
    menu.Append(EXPRESSION, _("&Expression"), _("Expression of Function"));
    menu.AppendSeparator();
    menu.Append(wxID_DELETE, _("&Delete"), _("Delete Item"));
    menu.Append(wxID_COPY, _("&Copy"), _("Copy Item"));
    menu.Append(wxID_CUT, _("Cu&t"), _("Cut Item"));
    menu.Append(wxID_PASTE, _("&Paste"), _("Paste Item"));
    menu.AppendSeparator();
    menu.Append(ADD_FUNC, _("Add &Func"), _("Add Function or operator"));
    menu.Append(ADD_PROP, _("Add P&roperty"), _("Add Property Node")); 
    menu.Append(ADD_VAL, _("Add &Value"), _("Add Const Value"));
    menu.Append(ADD_TABLE, _("Add &Table"), _("Add Table"));
}

bool TreeItemData_function::cmpPrior(functionType parent, functionType child)
{
  bool result = false;
  switch (child)
  {
    case eAbs :
    case eSin :
    case eCos :
    case eTan :
    case eASin :
    case eACos :
    case eATan :
    case eDerivative :
    case eIntegral :
      result = false;
      break;
    default :
      result = (getPrior(parent) >= getPrior(child));
  }
  return result;
}

int TreeItemData_function::getPrior(functionType child)
{
  switch (child)
  {
    case eTopLevel :
      return 0;
    case eIFTHEN :
      return 1;
    case eOR :
      return 2;
    case eAND :
      return 3;
    case eEQ :
    case eNE :
      return 4;
    case eLE :
    case eLT :
    case eGE :
    case eGT :
      return 5;
    case eSum :
    case eDifference :
      return 6;
    case eProduct :
    case eQuotient :
      return 7;
    case ePow :
    case eNOT :
      return 8;
    case eUndef :
      return 99;
    default :
      return 99;
  }
}

wxString TreeItemData_function::GetExpression()
{
  wxString result;
  bool first=true;
  
  wxString head, tail, deli;
  head = tail = deli = wxEmptyString;
  
  if (Type == eProduct)
  {
    deli = wxT(" * ");
  }
  else if (Type == eDifference)
  {
    deli = wxT(" - ");
  }
  else if (Type == eSum)
  {
    deli = wxT(" + ");
  }
  else if (Type == eQuotient)
  {
    deli = wxT(" / ");
  }
  else if (Type == ePow)
  {
    deli = wxT(" ^ ");
  }
  else if (Type == eAbs)
  {
    head = tail = wxT("|");
  }
  else if (Type == eTopLevel)
  {
    head = wxString(name.AfterLast('/') + wxT(" = "));
  }
  else if (Type == eLT)
  {
    deli = wxT(" < ");
  }
  else if (Type == eLE)
  {
    deli = wxT(" <= ");
  }
  else if (Type == eGT)
  {
    deli = wxT(" > ");
  }
  else if (Type == eGE)
  {
    deli = wxT(" >= ");
  }
  else if (Type == eEQ)
  {
    deli = wxT(" == ");
  }
  else if (Type == eNE)
  {
    deli = wxT(" != ");
  }
  else if (Type == eAND)
  {
    deli = wxT(" and ");
  }
  else if (Type == eOR)
  {
    deli = wxT(" or ");
  }
  else if (Type == eNOT)
  {
    head = wxT("!");
    deli = tail = wxEmptyString;
  }
  else if (Type == eIFTHEN)
  {
    deli = wxT(" ? ");
  }
  else
  {
    head = type + wxT("(");
    tail = wxT(")");
    deli = wxT(", ");
  }

  result = head;
  
  wxTreeItemIdValue cookie = (wxTreeItemIdValue)0401215LU;
  for (wxTreeItemId node = tree->GetFirstChild(GetId(), cookie); node.IsOk(); node = tree->GetNextChild(GetId(), cookie) )
  {
    TreeItemData_ebase * tmp = dynamic_cast<TreeItemData_ebase *>(tree->GetItemData(node));
    if (tmp)
    {
      if (first)
      {
        first = false;
      }
      else
      {
	      result += deli;
      }
      TreeItemData_function * child = dynamic_cast<TreeItemData_function *>(tmp);
      functionType childType = eUndef;
      if (child)
      {
        childType = child->Type;
      }
      bool isAddBracket = cmpPrior(Type, childType);
      if ( isAddBracket )
      {
        result += wxT("(");
      }
	    result += tmp->GetExpression();
      if ( isAddBracket )
      {
        result += wxT(")");        
      }
    }
  }

  result += tail;

  return result;
}

int TreeItemData_function::ShowDialog(void * data)
{
  int result;
  FunctionDialog dlg(tree, -1);
  dlg.Load(this);
  if ( (result = dlg.ShowModal()) == wxID_OK)
  {
    dlg.Save(this);
  }
  return result;
}

void TreeItemData_Property::Init(wxTreeCtrl* t)
{
  TreeItemData_base::Init(t);
  value = wxT("Unbinded");
  tree->SetItemText(GetId(), value);
  tree->SetItemImage(GetId(), 25);
  tree->SetItemImage(GetId(), 25, wxTreeItemIcon_Selected);
}

void TreeItemData_Property::Load(JSBSim::Element * el)
{
  value = el->GetDataLine(); 
  tree->SetItemText(GetId(), value);
  tree->SetItemImage(GetId(), 25);
  tree->SetItemImage(GetId(), 25, wxTreeItemIcon_Selected);
}

void TreeItemData_Property::Save(wxTextOutputStream & out, const wxString & prefix)
{
  if (value != wxT("Unbinded"))
    out << prefix << wxT("<property>") << value << wxT("</property>\n");
}

void TreeItemData_Property::PrepareMenu(wxMenu & menu)
{
    menu.Append(wxID_PROPERTIES, _("&Properties"), _("Detail of Function or operator"));
    menu.AppendSeparator();
    menu.Append(wxID_DELETE, _("&Delete"), _("Delete Item"));
    menu.Append(wxID_COPY, _("&Copy"), _("Copy Item"));
    menu.Append(wxID_CUT, _("Cu&t"), _("Cut Item"));

}

wxString TreeItemData_Property::GetExpression()
{
  return value.AfterLast('/');
}

int TreeItemData_Property::ShowDialog(void * data)
{
  int result;
  GetPropertyDialog()->Update();
  GetPropertyDialog()->Select(value);
  if ( (result = GetPropertyDialog()->ShowModal()) == wxID_OK)
  {
    value = GetPropertyDialog()->GetResult();
    tree->SetItemText(GetId(), value);
  }
  return result;
}

void TreeItemData_Value::Init(wxTreeCtrl* t)
{
  TreeItemData_base::Init(t);

  value = 0;
  tree->SetItemText(GetId(), wxT("Unset"));
  tree->SetItemImage(GetId(), 27);
  tree->SetItemImage(GetId(), 27, wxTreeItemIcon_Selected);
}

void TreeItemData_Value::Load(JSBSim::Element * el)
{
  value = el->GetDataAsNumber();
  tree->SetItemText(GetId(), wxString::Format("%g",value));
  tree->SetItemImage(GetId(), 27);
  tree->SetItemImage(GetId(), 27, wxTreeItemIcon_Selected);
}

void TreeItemData_Value::Save(wxTextOutputStream & out, const wxString & prefix)
{
  if (tree->GetItemText(GetId()) != wxT("Unset"))
    out << prefix << wxT("<value>") << wxString::Format(wxT("%g"), value) << wxT("</value>\n");
}

void TreeItemData_Value::PrepareMenu(wxMenu & menu)
{
    menu.Append(wxID_PROPERTIES, _("&Properties"), _("Detail of Function or operator"));
    menu.AppendSeparator();
    menu.Append(wxID_DELETE, _("&Delete"), _("Delete Item"));
    menu.Append(wxID_COPY, _("&Copy"), _("Copy Item"));
    menu.Append(wxID_CUT, _("Cu&t"), _("Cut Item"));
}

wxString TreeItemData_Value::GetExpression()
{
  return wxString::Format("%g",value);
}

int TreeItemData_Value::ShowDialog(void * data)
{
  int result;
  double bak = value;
  wxString val = wxString::Format("%g", value);
  val = ::wxGetTextFromUser(_("Value:"), _("Input Value"), val, tree);
  if ( val.ToDouble(&value) )
  {
    tree->SetItemText(GetId(), val);
    result = wxID_OK;
  }
  else
  {
    value = bak;
    result = wxID_CANCEL;
  }
  return result;
}


void TreeItemData_Table::Init(wxTreeCtrl* t)
{
  TreeItemData_base::Init(t);

  Type = tUndef;
  name = wxEmptyString;
  independentVar_row = wxEmptyString;
  independentVar_column = wxEmptyString;
  independentVar_table = wxEmptyString;
  independentVar_frame = wxEmptyString;
  tree->SetItemText(GetId(), wxT("Unset"));
  tree->SetItemImage(GetId(), 26);
  tree->SetItemImage(GetId(), 26, wxTreeItemIcon_Selected);
}

void TreeItemData_Table::Load(JSBSim::Element * el)
{
  std::stringstream buf;
  wxString property_string;
  name = el->GetAttributeValue("name");
  JSBSim::Element *tableData;
  int dimension = 0;

  // Determine and store the lookup properties for this table

  JSBSim::Element* axisElement = el->FindElement("independentVar");
  if (!axisElement) {
    throw  wxT("No independent variable found for table.");
  }

  independentVar_row = independentVar_column = independentVar_table = independentVar_frame = wxEmptyString;
  
  while (axisElement) {
    property_string = axisElement->GetDataLine();
    if (axisElement->GetAttributeValue("lookup") == string("row")) {
      independentVar_row = property_string;
    } else if (axisElement->GetAttributeValue("lookup") == string("column")) {
      independentVar_column = property_string;
    } else if (axisElement->GetAttributeValue("lookup") == string("table")) {
      independentVar_table = property_string;
    } else if (axisElement->GetAttributeValue("lookup") == string("frame")) {
      independentVar_frame = property_string;
    } else { // assumed single dimension table; row lookup
      independentVar_row = property_string;
    }
    axisElement = el->FindNextElement("independentVar");
  }
  dimension = 0;
  if (!independentVar_row.IsEmpty())
    ++dimension;
  if (!independentVar_column.IsEmpty())
    ++dimension;
  if (!independentVar_table.IsEmpty())
    ++dimension;
  if (!independentVar_frame.IsEmpty())
    ++dimension;
  // end lookup property code

  tableData = el->FindElement("tableData");
  for (int i=0; i<tableData->GetNumDataLines(); i++) {
    buf << tableData->GetDataLine(i) << string(" ");
  }

  switch (dimension) {
  case 1:
    Type = tt1D;
    value_2d.SetTable(tableData->GetNumDataLines(), 1);
    T1D_in(value_2d, buf);
    break;
  case 2:
    {
      unsigned int nRows = tableData->GetNumDataLines()-1;
      unsigned int nCols;
      if (nRows >= 2) {
        nCols = 0;
        // determine number of data columns in table (first column is row lookup - don't count)
        int position=0;
        while ((position = tableData->GetDataLine(0).find_first_not_of(" ", position)) != string::npos) {
          nCols++;
          position = tableData->GetDataLine(0).find_first_of(" ", position);
        }
      } else {
        throw "Not enough rows";
      }
      if (nCols > 1) {
        Type = tt2D;
      } else if (nCols == 1) {
        Type = tt1D;
      } else {
        throw "FGTable cannot accept 'Rows=0'";
      }
      value_2d.SetTable(nRows, nCols);
      T2D_in(value_2d, buf);
    }
    break;
  case 3: 
    Type = tt3D;
    T3D_in(value_3d, el);
    break;
  case 4:
    Type = tt4D;
    T4D_in(value_4d, el);
    break;
  default:
    throw "No dimension given";
    break;
  }

  tree->SetItemText(GetId(), GetExpression());
  tree->SetItemImage(GetId(), 26);
  tree->SetItemImage(GetId(), 26, wxTreeItemIcon_Selected);
}

void T1D_in(Table2D & table, std::stringstream& buf)
{
  double tmpR, tmpV;
  unsigned int nRows = table.GetNumRows();
  unsigned int nCols = table.GetNumCols();
  if (nCols != 1u)
    table.SetNumCols(1u);
  for (unsigned int i=1; i <= nRows; ++i)
  {
    buf >> tmpR >> tmpV;
    table.SetValue(i, 0u, tmpR);
    table.SetValue(i, 1u, tmpV);
  }
}

void T2D_in(Table2D & table, std::stringstream& buf)
{
  unsigned int nRows = table.GetNumRows();
  unsigned int nCols = table.GetNumCols();
  double tmpR, tmpC, tmpV;
  for (unsigned int i=1; i <= nCols; ++i)
  {
    buf >> tmpC;
    table.SetValue(0u, i, tmpC);
  }
  
  for (unsigned int i=1; i <= nRows; ++i)
  {
    buf >> tmpR;
    table.SetValue(i, 0u, tmpR);
    for (unsigned int j=1; j <= nCols; ++j)
    {
      buf >> tmpV;
      table.SetValue(i, j, tmpV);
    }
  }
}

void T3D_in(Table3D & table, JSBSim::Element * el)
{
  table.Clear();
  JSBSim::Element * tableData = el->FindElement("tableData");
  while (tableData)
  {
    std::stringstream buf;
    for (int i=0; i<tableData->GetNumDataLines(); i++) {
      buf << tableData->GetDataLine(i) << string(" ");
    }
    unsigned int nRows = tableData->GetNumDataLines()-1;
    unsigned int nCols;
    if (nRows >= 2) {
      nCols = 0;
      // determine number of data columns in table (first column is row lookup - don't count)
      int position=0;
      while ((position = tableData->GetDataLine(0).find_first_not_of(" ", position)) != string::npos) {
        nCols++;
        position = tableData->GetDataLine(0).find_first_of(" ", position);
      }
    } else {
      throw "Not enough rows";
    }
    Table2D tmp(nRows, nCols);
    T2D_in(tmp, buf);
    table.AddTable(tableData->GetAttributeValueAsNumber("breakPoint"), tmp);
    
    tableData = el->FindNextElement("tableData");
  }
}

void T4D_in(Table4D & table, JSBSim::Element * el)
{
  table.Clear();
  JSBSim::Element * tableData = el->FindElement("tableData");
  while (tableData)
  {
    Table3D tmp;
    T3D_in(tmp, tableData);
    table.AddTable(tableData->GetAttributeValueAsNumber("breakPoint"), tmp);
    
    tableData = el->FindNextElement("tableData");
  }
}

void TreeItemData_Table::Save(wxTextOutputStream & out, const wxString & prefix)
{
  if (Type == tUndef)
  {
    return;
  }
  switch (Type)
  {
    case tt4D :
      if (independentVar_frame.IsEmpty())
        return;
    case tt3D :
      if (independentVar_table.IsEmpty())
        return;
    case tt2D :
      if (independentVar_column.IsEmpty())
        return;
    case tt1D :
      if (independentVar_row.IsEmpty())
        return;
  }

  if (name == wxEmptyString)
  {
    out << prefix << wxT("<table>\n");
  }
  else
  {
    out << prefix << wxT("<table name=\"") << name << wxT("\">\n");
  }
  wxString pre = prefix + wxT("    ");
  switch (Type)
  {
    case tt1D :
      out << pre << wxT("<independentVar>") << independentVar_row << wxT("</independentVar>\n");
      break;
    case tt2D :
      out << pre << wxT("<independentVar lookup=\"row\">") << independentVar_row << wxT("</independentVar>\n");
      out << pre << wxT("<independentVar lookup=\"column\">") << independentVar_column << wxT("</independentVar>\n");
      break;
    case tt3D :
      out << pre << wxT("<independentVar lookup=\"row\">") << independentVar_row << wxT("</independentVar>\n");
      out << pre << wxT("<independentVar lookup=\"column\">") << independentVar_column << wxT("</independentVar>\n");
      out << pre << wxT("<independentVar lookup=\"table\">") << independentVar_table << wxT("</independentVar>\n");
      break;
    case tt4D :
      out << pre << wxT("<independentVar lookup=\"row\">") << independentVar_row << wxT("</independentVar>\n");
      out << pre << wxT("<independentVar lookup=\"column\">") << independentVar_column << wxT("</independentVar>\n");
      out << pre << wxT("<independentVar lookup=\"table\">") << independentVar_table << wxT("</independentVar>\n");
      out << pre << wxT("<independentVar lookup=\"frame\">") << independentVar_frame << wxT("</independentVar>\n");
      break;
    default :
      break;
  }

  switch (Type)
  {
    case tt1D :
      T1D_out(value_2d, out, pre);
      break;
    case tt2D :
      T2D_out(value_2d, out, pre);
      break;
    case tt3D :
      T3D_out(value_3d, out, pre);
      break;
    case tt4D :
      T4D_out(value_4d, out, pre);
      break;
  }
  
  out << prefix << wxT("</table>\n");
}

void T1D_out(Table2D & table, wxTextOutputStream & out, const wxString & prefix)
{
  out << prefix << wxT("<tableData>\n");

  unsigned int nRows = table.GetNumRows();
  unsigned int nCols = table.GetNumCols();
  for (unsigned int i=1u; i <= nRows; ++i)
  {
    out << prefix << wxT("  ") << wxString::Format(wxT("%-12g"), table.GetValue(i, 0u)) << wxT("  ") << wxString::Format(wxT("%-12g"), table.GetValue(i, 1u)) << wxT("\n") ;
  }
  
  out << prefix << wxT("</tableData>\n");
}

void T2D_out(Table2D & table, wxTextOutputStream & out, const wxString & prefix)
{
  out << prefix << wxT("<tableData>\n");

  unsigned int nRows = table.GetNumRows();
  unsigned int nCols = table.GetNumCols();
  out << prefix << wxT("  ") << wxT("             ");
  for (unsigned int i=1u; i <= nCols; ++i)
  {
    out << wxString::Format(wxT("%-12g"), table.GetValue(0u, i)) << wxT("  ");
  }
  out << wxT("\n");

  for (unsigned int j=1u; j <= nRows; ++j)
  {
    out << prefix << wxT("  ") << wxString::Format(wxT("%-12g"), table.GetValue(j, 0u)) << wxT("  ");
    for (unsigned int i=1; i <= nCols; ++i)
    {
      out << wxString::Format(wxT("%-12g"), table.GetValue(j, i)) << wxT("  ");
    }
    out << wxT("\n");
  }
  out << prefix << wxT("</tableData>\n");
}

void T3D_out(Table3D & table, wxTextOutputStream & out, const wxString & prefix)
{
  std::vector<double> keylist = table.GetKeyList();
  for (std::vector<double>::iterator i = keylist.begin(); i != keylist.end();
      ++i)
  {
    Table2D * tmp = table.GetTable(*i);
    if (tmp)
    {
      out << prefix << wxT("<tableData breakPoint=\"") << wxString::Format(wxT("%g"), *i) << wxT("\">\n");
      unsigned int nRows = tmp->GetNumRows();
      unsigned int nCols = tmp->GetNumCols();
      out << prefix << wxT("  ") << wxT("             ");
      for (unsigned int i=1u; i <= nCols; ++i)
      {
        out << wxString::Format(wxT("%-12g"), tmp->GetValue(0u, i)) << wxT("  ");
      }
      out << wxT("\n");

      for (unsigned int j=1u; j <= nRows; ++j)
      {
        out << prefix << wxT("  ") << wxString::Format(wxT("%-12g"), tmp->GetValue(j, 0u)) << wxT("  ");
        for (unsigned int i=1; i <= nCols; ++i)
        {
          out << wxString::Format(wxT("%-12g"), tmp->GetValue(j, i)) << wxT("  ");
        }
        out << wxT("\n");
      }
      out << prefix << wxT("</tableData>\n");
    }
  }
}

void T4D_out(Table4D & table, wxTextOutputStream & out, const wxString & prefix)
{
  std::vector<double> keylist = table.GetKeyList();
  for (std::vector<double>::iterator i = keylist.begin(); i != keylist.end();
      ++i)
  {
    Table3D * tmp = table.GetTable(*i);
    if (tmp)
    {
      out << prefix << wxT("<tableData breakPoint=\"") << wxString::Format(wxT("%g"), *i) << wxT("\">\n");
      T3D_out(*tmp, out, prefix + wxT("  "));
      out << prefix << wxT("</tableData>\n");
    }
  }
}

void TreeItemData_Table::PrepareMenu(wxMenu & menu)
{
    menu.Append(wxID_PROPERTIES, _("&Properties"), _("Detail of Function or operator"));
    menu.AppendSeparator();
    menu.Append(wxID_DELETE, _("&Delete"), _("Delete Item"));
    menu.Append(wxID_COPY, _("&Copy"), _("Copy Item"));
    menu.Append(wxID_CUT, _("Cu&t"), _("Cut Item"));
}

wxString TreeItemData_Table::GetExpression()
{
  wxString T;
  if (name == wxEmptyString)
  {
    T = wxT("T(");
  }
  else
  {
    T = name + wxT("(");
  }
  switch (Type)
  {
  case tt1D :
    return T + independentVar_row.AfterLast('/') + wxT(")");
  case tt2D :
    return T + independentVar_row.AfterLast('/') + wxT(", ") + independentVar_column.AfterLast('/') + wxT(")");
  case tt3D :
    return T + independentVar_row.AfterLast('/') + wxT(", ") + independentVar_column.AfterLast('/') + wxT(", ") + independentVar_table.AfterLast('/') + wxT(")");
  case tt4D :
    return T + independentVar_row.AfterLast('/') + wxT(", ") + independentVar_column.AfterLast('/') + wxT(", ") + independentVar_table.AfterLast('/') + wxT(", ") + independentVar_frame.AfterLast('/') + wxT(")");     
  }
}

int TreeItemData_Table::ShowDialog(void * data)
{
  int result;
  TableDialog dlg(tree, -1);
  dlg.Load(this);
  if ( (result = dlg.ShowModal()) == wxID_OK)
  {
    dlg.Save(this);
  }
  return result;
}

/******************************************************************************
void TreeItemData_flight_control::Init(wxTreeCtrl* t)
{
    TreeItemData_base::Init(t);
    type = tUndef;
    file_name = wxEmptyString;
}


void CopyTo(wxTextOutputStream & out, JSBSim::Element * el, const wxString & prefix)
{
  int attribute_num = el->GetNumAttributeKeys();
  wxString attr = wxEmptyString;
  for (int i=0; i<attribute_num ; ++i)
  {
    string key = el->GetAttributeKey(i);
    attr += wxT(" ") + key + wxT("=\"") + el->GetAttributeValue(key) + wxT("\"");
  }
  int children_num = el->GetNumElements();
  if (children_num ==0)
  {
    out << prefix << wxT("<") << el->GetName() << attr << wxT(">");
    int line_num = el->GetNumDataLines();
    if (line_num>1)
      out << wxT("\n");
    for (int i=0; i< line_num; ++i)
    {
      out << el->GetDataLine(i);
      if (i<line_num-1) out << wxT("\n");
    }
    out << wxT("</") << el->GetName() << wxT(">\n");
  }
  else
  {
    out << prefix << wxT("<") << el->GetName() << attr << wxT(">\n");
    JSBSim::Element * tmp = el->GetElement();
    while (tmp)
    {
      CopyTo(out, tmp, prefix+wxT("    "));
      tmp = el->GetNextElement();
    }
    out << prefix << wxT("</") << el->GetName() << wxT(">\n");
  }
}


void mv2file(JSBSim::Element * el, const wxString & file_name)
{
  wxFileOutputStream os(file_name);
  wxTextOutputStream out(os,wxEOL_UNIX);

  out << wxT("<?xml version=\"1.0\"?>\n");

  CopyTo(out, el, wxEmptyString);

  os.Close();
}

void TreeItemData_flight_control::Load(JSBSim::Element * el)
{
  wxString tmp = el->GetAttributeValue("file");
  if (tmp.IsEmpty())
  {
    switch (type)
    {
    case tAP :
      file_name = wxT("ap");
      break;
    case tFCS :
      file_name = wxT("fcs");
      break;
    default :
      break;
    }
    wxString path=wxEmptyString;
#ifdef __WXMSW__
    path = filename.BeforeLast('\\') + wxT("\\");
#else
    path = filename.BeforeLast('/') + wxT("/");      
#endif
    mv2file(el, path+file_name+wxT(".xml"));
  }
  else
  {
    file_name = tmp;
  }
}

void TreeItemData_flight_control::Save(wxTextOutputStream & out, const wxString & prefix)
{
  if (file_name.IsEmpty())
    return;

  switch (type)
  {
    case tAP :
      {
        out << prefix << wxT("<autopilot file=\"") << file_name << wxT("\" />\n");
      }
      break;
    case tFCS :
      {
        out << prefix << wxT("<flight_control file=\"") << file_name << wxT("\" />\n");
      }
      break;
    default :
      break;
  }
}
    
void TreeItemData_flight_control::PrepareMenu(wxMenu & menu)
{
  switch (type)
  {
    case tAP :
      menu.Append(wxID_PROPERTIES, wxT("&Detail"), wxT("Detail of AP"));
      break;
    case tFCS :
      menu.Append(wxID_PROPERTIES, wxT("&Detail"), wxT("Detail of FCS"));
      break;
    default :
      break;
  }
}

bool TreeItemData_flight_control::check_fcs_file(const wxString & fcsfile, const wxString & diafile)
{
  if (::wxFileExists(diafile))
    return true;
  
  if (!::wxFileExists(fcsfile))
  {
    wxFileOutputStream os(fcsfile);
    if (!os.Ok())
      return false;
    wxTextOutputStream out(os,wxEOL_UNIX);

    out << wxT("<?xml version=\"1.0\"?>\n");

    switch (type)
    {
    case tAP :
      out << wxT("<autopilot name=\"unknown\">\n");
      out << wxT("</autopilot>\n");  
      break;
    case tFCS :
      out << wxT("<flight_control name=\"unknown\">\n");
      out << wxT("</flight_control>\n");
      break;
    default :
      throw "tUndef occure.";
      break;
    }
  }
  return false;
}

void TreeItemData_flight_control::ShowDialog(void * data)
{
  wxString filename = *(wxString *)data;
  wxString dir = filename.BeforeLast(wxGetApp().Delimited[0]);
  wxString fcsfile = dir + wxGetApp().Delimited + file_name + wxT(".xml");
  wxString diafile = dir + wxGetApp().Delimited + file_name + wxT(".dia");
  if (check_fcs_file(fcsfile, diafile))
  {
      ::wxExecute(wxT("./fcsbuilder ") + diafile);
  }
  else
  {
      ::wxExecute(wxT("./fcsbuilder ") + fcsfile);
  }

}

******************************************************************************/

void TreeItemData_func_root::Init(wxTreeCtrl* t)
{
  TreeItemData_base::Init(t);
}

void TreeItemData_func_root::PrepareMenu(wxMenu & menu)
{
    menu.Append(ADD_FUNC, _("Add &Function"), _("Add Function"));
    menu.Append(wxID_PASTE, _("&Paste"), _("Paste Item"));
}

void TreeItemData_script_root::Init(wxTreeCtrl* t)
{
  TreeItemData_base::Init(t);
}

void TreeItemData_script_root::PrepareMenu(wxMenu & menu)
{
    menu.Append(ADD_FUNC, _("Add &Function"), _("Add Function"));
    menu.Append(wxID_PASTE, _("&Paste"), _("Paste Item"));
}


