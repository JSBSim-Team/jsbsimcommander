/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 File:         PropDlg.h
 Author:       Matthew Gong
 Date started: 02/26/2006

 -------- Copyright (C) 2006 Matthew Gong (matthewzhenggong@gmail.com) --------

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA  02111-1307, USA.

 Further information about the GNU General Public License can also be found on
 the world wide web at http://www.gnu.org.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
SENTRY
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#ifndef PROPERTYDLG_H
#define PROPERTYDLG_H

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
 #pragma interface
#endif

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <vector>
using std::vector;

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
DEFINITIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
FORWARD DECLARATIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DOCUMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/** The PropDialog class is a dialog to manage all the properties.
    It is used to search a proper property.

    @author Matthew Gong
*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DECLARATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

class PropertyDialog: public wxDialog {
public:
    /// Constructor
    PropertyDialog(wxWindow* parent);

    /** Load all properties from the file
     * @param filename The file to be loaded.
     * @return Return true for success.
     */
    bool Load(const wxString & filename);

    /** Create a property from the string array.
     * @param tmp The string array stands for a property
     */
    void Create(const wxArrayString & tmp);
    
    /** Add a property from the string array.
     * @param tmp The string array stands for a property
     */
    void AddItem(const wxArrayString & tmp);

    /** Clear all the properties Added by AddItem function.
     * @see AddItem
     */
    void ClearVar();

    /** Get the selected property.
     * @return Return the selected property.
     */
    wxString GetResult(int c=0) const;

    /** Update the list with the filter.
     * Only the properties which are match with the filter string will be shown.
     * @param The filter string. If it's empty, all properties will be shown.
     */
    void Update(const wxString & filter = wxEmptyString);

    /** Select the property by name.
     * @param name The property name will be selected.
     * @return Return true if this property is in the list and is selected.
     */
    bool Select(const wxString & name);

protected:
    wxTextCtrl* text_ctrl_sel;
    wxStaticText* label_info;
    wxListCtrl* list_prop;
    wxTextCtrl* text_ctrl_filter;

    DECLARE_EVENT_TABLE();

public:
    /// Deal with the filter button event.
    void OnFilter(wxCommandEvent &event);

    /// Deal with the showall button event.
    void OnShowAll(wxCommandEvent &event);

    /// Deal with the list select event.
    void OnSelect(wxListEvent &event);

protected:
    bool pass_filter(const wxArrayString & item, const wxString & filter);
    wxArrayString names;
    vector<wxArrayString> properties;
    unsigned size;
    
}; // wxGlade: end class


#endif // PROPERTYDLG_H
