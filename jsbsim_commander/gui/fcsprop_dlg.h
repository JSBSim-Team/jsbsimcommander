/////////////////////////////////////////////////////////////////////////////
// Name:        fcsprop_dlg.h
// Purpose:     FCS Builder Property Dialogs
// Author:      Matthew Gong
// Modified by:
// Created:     02/02/2005
// Copyright:   (c) Matthew Gong
// Licence:     GPL licence
/////////////////////////////////////////////////////////////////////////////

#include <wx/wx.h>
#include <wx/image.h>
// begin wxGlade: ::dependencies
// end wxGlade


#ifndef FCSPROP_DLG_H
#define FCSPROP_DLG_H


class FCSPropDialog: public wxDialog {
public:
    // begin wxGlade: FCSPropDialog::ids
    enum
    {
      ID_BOX = 700,
      ID_TEXT,
      ID_ADD,
      ID_MODIFY,
      ID_REMOVE
    };
    // end wxGlade

    FCSPropDialog(const wxString & t, const wxString & n, const wxArrayString & as, wxWindow* parent, int id, const wxString& title=_("Properties defined in FCS"), const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxSize(330, 560), long style=wxDEFAULT_DIALOG_STYLE);

private:
    // begin wxGlade: FCSPropDialog::methods
    void set_properties();
    void do_layout();
    // end wxGlade

protected:
    // begin wxGlade: FCSPropDialog::attributes
    wxStaticText* label_type;
    wxComboBox* combo_box_type;
    wxStaticText* label_name;
    wxTextCtrl* text_ctrl_name;
    wxListBox* list_box;
    wxTextCtrl* text_ctrl;
    wxButton* button_add;
    wxButton* button_modify;
    wxButton* button_remove;
    wxButton* button_OK;
    wxButton* button_cancel;
    // end wxGlade

public:
    void OnOK( wxCommandEvent &event );
    void OnAdd(wxCommandEvent & event);
    void OnModify(wxCommandEvent & event);
    void OnRemove(wxCommandEvent & event);
    void OnSelect(wxCommandEvent & event);

    wxArrayString data;
    wxString type;
    wxString name;

    DECLARE_EVENT_TABLE ()
};


#endif // FCSPROP_DLG_H
