/////////////////////////////////////////////////////////////////////////////
// Name:        import.h
// Purpose:     classes for importing
// Author:      Matthew Gong
// Created:     03/19/2005
// Copyright:   (c) Matthew Gong
// Licence:     GPL licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _FCSBUILDER_IMPORT_H_
#define _FCSBUILDER_IMPORT_H_

#if defined(__GNUG__) && !defined(__APPLE__)
// #pragma interface
#endif

#include "wx/ogl/ogl.h"

#include <vector>
#include <list>

#include "easyxml.hxx"
#include "FGXMLElement.h"

#include "shape.h"

struct TBox
{
    typedef std::vector<wxShape *> shapelist;
    typedef std::vector<wxShape *>::iterator shapelist_iter;
    shapelist shapes;
    ComponentShape* current_shape;
    bool have_output_node;

    double min_x();
    double max_x();
    double min_y();
    double max_y();
    
    void move_delta(double dx, double dy);
};

class TBoxMgr
{
  public:
    TBoxMgr(MyDiagram * diag)
    {
      diagram = diag;
    }
    ~TBoxMgr();

    typedef std::list<TBox *> boxlist;
    typedef boxlist::iterator boxlist_iter;
    typedef std::list<boxlist_iter> boxlist_iter_list;
    typedef boxlist_iter_list::iterator boxlist_iter_list_iter;
    
    void Add(ComponentShape *, const wxArrayString & inputs );
    void ShowAll();
    void GetSize(double &x, double &y);

  protected:
    MyDiagram *diagram;
    boxlist boxes;
    
    boxlist_iter_list isStandAlone(const wxArrayString & inputs);
    //void append(ComponentShape * shape, const wxArrayString & inputs);
    boxlist_iter combine(ComponentShape * shape, const wxArrayString & inputs, boxlist_iter_list & b);
    void move_down(boxlist_iter iter);
};


#endif  // _FCSBUILDER_IMPORT_H_

