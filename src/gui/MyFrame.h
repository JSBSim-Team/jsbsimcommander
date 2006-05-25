//-----------------------------------------------------------------------------
// Name:        myframe.h
// Purpose:     A derived frame, called MyFrame
// Author:      Matthew Gong
// Licence:     wxWindows licence
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//-----------------------------------------------------------------------------

#ifndef _MYFRAME_H_
#define _MYFRAME_H_

//-----------------------------------------------------------------------------
// GCC interface
//-----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(__APPLE__)
//    #pragma interface "myframe.h"
#endif

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include <wx/frame.h>

//-----------------------------------------------------------------------------
// Class definition: MyFrame
//-----------------------------------------------------------------------------

/**
* My Frame is the main frame.
* Define a new frame type: this is going to be our main frame.
*/

class MyFrame : public wxFrame
{

public:

    /// Constructor.
    MyFrame( wxWindow* parent=(wxWindow *)NULL);
    
private:

    // Event handlers (these functions should _not_ be virtual)
    void OnExitToolOrMenuCommand(wxCommandEvent& event);
    void OnAboutToolOrMenuCommand(wxCommandEvent& event);
    void OnAircraftManagerToolOrMenuCommand(wxCommandEvent& event);
    void OnPropertyManagerToolOrMenuCommand(wxCommandEvent& event);
    void OnResultManagerToolOrMenuCommand(wxCommandEvent& event);
    void OnRunScriptToolOrMenuCommand(wxCommandEvent& event);

    // Any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()

};

//-----------------------------------------------------------------------------
// End single inclusion of this .h file condition
//-----------------------------------------------------------------------------

#endif  // _MYFRAME_H_

