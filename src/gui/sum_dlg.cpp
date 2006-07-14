/////////////////////////////////////////////////////////////////////////////
// Name:        sum_dlg.cpp
// Purpose:     dialog for FGSummer
// Author:      Matthew Gong
// Created:     07/06/2006
// Copyright:   (c) Matthew Gong
// Licence:     GPL licence
//
// Functions:
//
// SummerPropertyDialog - Constructor
// OnClipable
// GetDataIn
// SetDataOut
// OnListBox
// OnRadioSign
// OnInputAdd
// OnInputRemove
//
/////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// GCC implementation
//-----------------------------------------------------------------------------

#ifdef __GNUG__
// #pragma implementation
#endif

//-----------------------------------------------------------------------------
// Standard wxWidgets headers
//-----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// For all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

//-----------------------------------------------------------------------------
// Header of this .cpp file
//-----------------------------------------------------------------------------

#include "sum_dlg.h"

//-----------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//-----------------------------------------------------------------------------

#include <wx/valtext.h>
#include <wx/valgen.h>
#include <wx/xrc/xmlres.h>              // XRC XML resouces

#include "shape.h"
#include "sum.h"

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
BEGIN_EVENT_TABLE (SummerPropertyDialog, wxDialog)
  EVT_CHECKBOX(XRCID("ID_CLIPABLE"), SummerPropertyDialog::OnClipable)
  EVT_LISTBOX (XRCID("ID_LISTBOX_INPUTS"), SummerPropertyDialog::OnListBox)
  EVT_RADIOBOX (XRCID("ID_RADIO_SIGN"), SummerPropertyDialog::OnRadioSign)
  EVT_BUTTON  (XRCID("ID_INPUT_ADD"), SummerPropertyDialog::OnInputAdd)
  EVT_BUTTON  (XRCID("ID_INPUT_REMOVE"), SummerPropertyDialog::OnInputRemove)
END_EVENT_TABLE ()

/**
* Constructor ==================================================================
*/

SummerPropertyDialog::SummerPropertyDialog(Summer * sum, wxWindow* parent)
{
    // Load up this frame from XRC. [Note, instead of making a class's
    // constructor take a wxWindow* parent with a default value of NULL,
    // we could have just had designed MyFrame class with an empty
    // constructor and then written here:
    // wxXmlResource::Get()->LoadFrame(this, (wxWindow* )NULL, "main_frame");
    // since this frame will always be the top window, and thus parentless.
    // However, the current approach has source code that can be recycled
    // for other frames that aren't the top level window.]
    wxXmlResource::Get()->LoadDialog(this, parent, wxT("SUM_DIALOG"));

    // Get the widgets needed.
    text_ctrl_name = XRCCTRL(*this, "ID_TEXT_NAME", wxTextCtrl);
    text_ctrl_order = XRCCTRL(*this, "ID_TEXT_ORDER", wxTextCtrl);
    checkbox_clipable = XRCCTRL(*this, "ID_CLIPABLE", wxCheckBox);
    text_ctrl_clipmax = XRCCTRL(*this, "ID_TEXT_CLIP_MAX", wxTextCtrl);
    text_ctrl_clipmin = XRCCTRL(*this, "ID_TEXT_CLIP_MIN", wxTextCtrl);
    text_ctrl_bias = XRCCTRL(*this, "ID_TEXT_BIAS", wxTextCtrl);
    list_box_input = XRCCTRL(*this, "ID_LISTBOX_INPUTS", wxListBox);
    radio_box_sign = XRCCTRL(*this, "ID_RADIO_SIGN", wxRadioBox);
    notebook_Main_pane_basic = XRCCTRL(*this, "ID_PANEL_BASIC", wxPanel);
    notebook_Main_pane_input = XRCCTRL(*this, "ID_PANEL_INPUTS", wxPanel);
    
    // Set the validators.
    text_ctrl_name->SetValidator(wxTextValidator(wxFILTER_NONE, &name));
    text_ctrl_order->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &order));
    checkbox_clipable->SetValidator(wxGenericValidator(&clipable));
    text_ctrl_clipmax->SetValidator(wxTextValidator(wxFILTER_NONE, &clipmax));
    text_ctrl_clipmin->SetValidator(wxTextValidator(wxFILTER_NONE, &clipmin));
    text_ctrl_bias->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &bias));

    // Make sure to set the extra flag wxWS_EX_VALIDATE_RECURSIVELY for the dialog.
    SetExtraStyle(GetExtraStyle() | wxWS_EX_VALIDATE_RECURSIVELY);

    // Read data in.
    if (sum)
      GetDataIn(sum);
}

/**
* OnClipable ===================================================================
*/

void SummerPropertyDialog::OnClipable(wxCommandEvent & WXUNUSED(event) )
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

/**
* GetDataIn ====================================================================
*/

void SummerPropertyDialog::GetDataIn(const Summer * g)
{
  name       = g->GetName();
  order      = wxString::Format(wxT("%ld"),g->GetOrder());
  clipable   = g->IsClipable();
  clipmax    = g->GetClipMax();
  clipmin    = g->GetClipMin();

  bias       = wxString::Format(wxT("%g"),g->GetBias());

  list_box_input->Clear();
  wxArrayString inputs = g->GetInputNames();
  for (size_t i = 0; i < inputs.GetCount(); ++i)
    {
      if (g->GetInputIsInverted(i+1))
	{
	  list_box_input->Append(wxT("-") + inputs[i]);
	}
      else
	{
	  list_box_input->Append(inputs[i]);
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
  if ( list_box_input->GetString(0)[0] == wxT('-') )
    radio_box_sign->SetSelection(1);
  else
    radio_box_sign->SetSelection(0);
}

/**
* SetDataOut ===================================================================
*/

void SummerPropertyDialog::SetDataOut(Summer * g) const
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

  double w = g->GetWidth();
  double h = g->GetHeight();
  g->ClearAttachments ();
  int n = g->GetNumberOfAttachments();
  n++;
  g->GetAttachments ().Append (new wxAttachmentPoint (0,  w * 0.5, 0.0));

  int s = list_box_input->GetCount();
  for ( int i = 0; i < s; ++i)
    {
      wxString str = list_box_input->GetString(i);
      if ( str[0] == wxT('-') )
	g->SetInputIsInverted(true, i+1);
      else
	g->SetInputIsInverted(false, i+1);
      g->GetAttachments ().Append (new wxAttachmentPoint (i+1, -w * 0.5, 0.0));
    }

  g->SetAttachmentSize(w,h);
  g->NormalizeLines();
}

/**
* OnListBox ====================================================================
*/

void SummerPropertyDialog::OnListBox (wxCommandEvent & WXUNUSED(event))
{
  wxString str = list_box_input->GetStringSelection();
  if ( str[0] == wxT('-') )
    radio_box_sign->SetSelection(1);
  else
    radio_box_sign->SetSelection(0);

}

/**
* OnRadioSet ===================================================================
*/

void SummerPropertyDialog::OnRadioSign (wxCommandEvent & WXUNUSED(event))
{
  int pos =  list_box_input->GetSelection();
  if (pos != wxNOT_FOUND)
  {
    wxString name = list_box_input->GetString(pos);
    if (name[0] == wxT('-'))
    {
      name.Remove(0u, 1u);
    }
    if ( radio_box_sign->GetSelection() == 0 )
      list_box_input->SetString(pos,name);
    else
      list_box_input->SetString(pos,wxT("-")+name);
  }
}

/**
* OnInputAdd ===================================================================
*/

void SummerPropertyDialog::OnInputAdd (wxCommandEvent & WXUNUSED(event))
{
  list_box_input->Append(wxT("NULL"));
}

/**
* OnInputRemove ================================================================
*/

void SummerPropertyDialog::OnInputRemove (wxCommandEvent & WXUNUSED(event))
{
  size_t n;
  while ( (n = list_box_input->GetCount()) > 2)
  {
    --n;
    if (list_box_input->GetString(n) == wxT("NULL")
     || list_box_input->GetString(n) == wxT("-NULL") )
    {
      list_box_input->Delete(n);
    }
    else
    {
      break;
    }
  }
}

