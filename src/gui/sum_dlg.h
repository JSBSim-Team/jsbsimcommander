/////////////////////////////////////////////////////////////////////////////
// Name:        sum_dlg.h
// Purpose:     dialog for FGSummer
// Author:      Matthew Gong
// Created:     07/06/2006
// Copyright:   (c) Matthew Gong
// Licence:     GPL licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
// #pragma interface
#endif

#include <wx/wx.h>


#ifndef SUMDIALOG_H
#define SUMDIALOG_H

class Summer;

/** The Dialog for summer component.
 *  The dialog use the XRC dialog resource SUM_DIALOG(sum_dlg.xrc).
 *  And all these ID is needed:
 *     ID_TEXT_NAME,
 *     ID_TEXT_ORDER,
 *     ID_CLIPABLE,
 *     ID_TEXT_CLIP_MAX,
 *     ID_TEXT_CLIP_MIN,
 *     ID_TEXT_BIAS,
 *     ID_LISTBOX_INPUTS,
 *     ID_RADIO_SIGN,
 *     ID_PANEL_BASIC,
 *     ID_PANEL_INPUTS,
 *     ID_INPUT_ADD,
 *     ID_INPUT_REMOVE.
 *  Use it like this:
 *     SummerPropertyDialog  dlg((Summer *)theShape, this);
 *     if ( dlg.ShowModal() == wxID_OK )
 *     {
 *       dlg.SetDataOut((Summer *)theShape);
 *     }
 */

class SummerPropertyDialog : public wxDialog 
{
 public:
  /// Constructor
  SummerPropertyDialog(Summer * sum, wxWindow* parent);

  /// Read data in.
  void GetDataIn(const Summer * g);

  /// Send data out.
  void SetDataOut(Summer * g) const;

 private:
  wxString name;
  wxString order;
  bool clipable;
  wxString clipmax;
  wxString clipmin;

  wxString bias;

  wxTextCtrl* text_ctrl_name;
  wxTextCtrl* text_ctrl_order;
  wxCheckBox* checkbox_clipable;
  wxTextCtrl* text_ctrl_clipmax;
  wxTextCtrl* text_ctrl_clipmin;
  wxTextCtrl* text_ctrl_bias;
  wxPanel* notebook_Main_pane_basic;
  wxListBox* list_box_input;
  wxRadioBox* radio_box_sign;
  wxPanel* notebook_Main_pane_input;

 public:

  void OnClipable (wxCommandEvent & event);
  void OnListBox (wxCommandEvent & event);
  void OnRadioSign (wxCommandEvent & event);
  void OnInputAdd (wxCommandEvent & event);
  void OnInputRemove (wxCommandEvent & event);

  DECLARE_EVENT_TABLE ()
};


#endif // SUMDIALOG_H
