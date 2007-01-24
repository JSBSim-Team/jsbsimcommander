/////////////////////////////////////////////////////////////////////////////
// Name:        PropDlg.cpp
// Purpose:     list of properties
// Author:      Matthew Gong
// Created:     02/26/2006
// Copyright:   (c) Matthew Gong
// Licence:     GPL licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
 #pragma implementation 
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
#include <wx/mstream.h>
#include <wx/txtstrm.h>
#include <wx/xrc/xmlres.h>

#include "Property_Diag.h"

wxArrayString readStrsfromString(wxString str)
{
  wxArrayString list;
  while (!str.IsEmpty())
  {
    if (str[0] == wxT('\"'))
    {
      int end = str.Find(wxT("\","));
      if (end == -1)
      {
	wxString tmp = str.Mid(1u, str.Length()-2);
	tmp.Replace(wxT("\"\""), wxT("\""));
	list.Add(tmp);
	str = wxEmptyString;
      }
      else
      {     
	wxString tmp = str.Mid(1u, end-1);
	tmp.Replace(wxT("\"\""), wxT("\""));
	list.Add(tmp);
	str = str.Mid(end+2);
      }
    }
    else
    {
      int end = str.Find(wxT(','));
      if (end == -1)
      {
        list.Add(str);
        str = wxEmptyString;
      }
      else
      {
	list.Add(str.Mid(0u, end));
	str = str.Mid(end+1);
      }
    }
  }
  
  return list;
}


//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------

// The event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
// The reason why the menuitems and tools are given the same name in the
// XRC file, is that both a tool (a toolbar item) and a menuitem are designed
// to fire the same kind of event (an EVT_MENU) and thus I give them the same
// ID name to help new users emphasize this point which is often overlooked
// when starting out with wxWidgets.
BEGIN_EVENT_TABLE(PropertyDialog, wxDialog)
    EVT_BUTTON(XRCID("button_filter"), PropertyDialog::OnFilter)
    EVT_LIST_ITEM_SELECTED(XRCID("list_prop"), PropertyDialog::OnSelect) 
    EVT_TEXT_ENTER(XRCID("text_filter"), PropertyDialog::OnFilter)
    EVT_BUTTON(XRCID("button_showall"), PropertyDialog::OnShowAll)
END_EVENT_TABLE();

//-----------------------------------------------------------------------------
// Class implementation
//-----------------------------------------------------------------------------

PropertyDialog::PropertyDialog(wxWindow* parent)
{
    wxXmlResource::Get()->LoadDialog(this, parent, wxT("prop_dlg"));
    text_ctrl_sel = XRCCTRL(*this, "text_sel", wxTextCtrl);
    label_info = XRCCTRL(*this, "label_info", wxStaticText); 
    list_prop = XRCCTRL(*this, "list_prop", wxListCtrl); 
    text_ctrl_filter = XRCCTRL(*this, "text_filter", wxTextCtrl); 

}


void PropertyDialog::OnFilter(wxCommandEvent &event)
{
    wxString tmp = GetResult();
    Update(text_ctrl_filter->GetValue());
    Select(tmp);
}


void PropertyDialog::OnShowAll(wxCommandEvent &event)
{
    wxString tmp = GetResult();
    Update();
    Select(tmp);
}

void PropertyDialog::OnSelect(wxListEvent &event)
{
  long i = event.GetIndex();
  if ( i == -1 )
    return;
  
  wxListItem item;
  item.SetId(i);
  item.SetColumn(0);
  item.SetText(wxEmptyString);
  list_prop->GetItem(item);
  text_ctrl_sel->SetValue(item.GetText());
}


bool PropertyDialog::Load(const wxString & filename)
{
  wxFileInputStream input( filename );
  if (!input.Ok())
    return false;
#if wxUSE_UNICODE
  wxTextInputStream text( input, wxT("\t"), wxConvLibc );
#else
  wxTextInputStream text( input, wxT("\t") );
#endif

  names = readStrsfromString(text.ReadLine());
  list_prop->ClearAll();
  wxListItem item;
  for (size_t i = 0u; i < names.GetCount(); ++i)
  {
    item.SetText(names[i]);
    list_prop->InsertColumn(i, item);
  }
  
  wxArrayString line;
  properties.clear();
  properties.reserve(256);
  while (!input.Eof())
  {
    line = readStrsfromString(text.ReadLine());
    if (line.GetCount() <= names.GetCount() && line.GetCount() >= 1u )
    {
      properties.push_back(line);
    }
  }

  size = properties.size();
  Update();
  return true;
}

void PropertyDialog::Create(const wxArrayString & tmp)
{
  names = tmp;
  list_prop->ClearAll();
  wxListItem item;
  for (size_t i = 0u; i < names.GetCount(); ++i)
  {
    item.SetText(names[i]);
    list_prop->InsertColumn(i, item);
  }
  
  properties.clear();
  properties.reserve(256);
  size = properties.size();
}

void PropertyDialog::AddItem(const wxArrayString & tmp)
{
  if (tmp.GetCount() > 1)
    properties.push_back(tmp);
}

void PropertyDialog::ClearVar()
{
  while (properties.size() > size)
    properties.pop_back();
}

wxString PropertyDialog::GetResult(int c) const
{
  if (c == 0)
  {
    return text_ctrl_sel->GetValue();
  }
  else
  {
    long i = -1;
    for ( ;; )
    {
        i = list_prop->GetNextItem(i,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_SELECTED);
        if ( i == -1 )
            return wxEmptyString;

        wxListItem item;
	item.SetId(i);
	item.SetColumn(c);
	item.SetText(wxEmptyString);
	list_prop->GetItem(item);
	return item.GetText();
    }
  
  }
}

void PropertyDialog::Update(const wxString & filter)
{
  list_prop->DeleteAllItems();
  vector<wxArrayString>::iterator i;
  int j;
  size_t n = names.GetCount();
  for (i = properties.begin(), j = 0; i != properties.end(); ++i)
  {
    if (pass_filter(*i, filter) )
    {
      wxListItem item;
      item.SetId(j);
      item.SetColumn(0);
      item.SetText((*i)[0]);
      list_prop->InsertItem(item);
      for (size_t k = 1u; k < i->GetCount() && k < n; ++k)
      {
        item.SetColumn(k);
        item.SetText((*i)[k]);
        list_prop->SetItem(item);
      }
      ++j;
    }
  }
  for (size_t k = 0u; k < n; ++k)
  {
      list_prop->SetColumnWidth( k, wxLIST_AUTOSIZE );
  }
  label_info->SetLabel(wxString::Format(_("show %d, hide %d"), j, properties.size()-j));
}

bool PropertyDialog::pass_filter(const wxArrayString & item, const wxString & filter)
{
  if (filter.IsEmpty())
    return true;
  for (size_t k = 0u; k < item.GetCount(); ++k)
  {
    if (item[k].Find(filter) != -1)
      return true;
  }
  return false;
}

bool PropertyDialog::Select(const wxString & name)
{
  text_ctrl_sel->SetValue(name);

  long item = -1;
  for ( ;; )
  {
        item = list_prop->GetNextItem(item,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_DONTCARE);
        if ( item == -1 )
            break;

	wxString tmp = list_prop->GetItemText(item);
        if (tmp.BeforeFirst('[') == name.BeforeFirst('['))
        {
	  list_prop->SetItemState(item, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
	  list_prop->EnsureVisible(item);
          return true;
        }
  }

  item = -1;
  for ( ;; )
  {
        item = list_prop->GetNextItem(item,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_SELECTED);
        if ( item == -1 )
            break;
	list_prop->SetItemState(item, 0u, wxLIST_STATE_SELECTED);
  }
  return false;
}

