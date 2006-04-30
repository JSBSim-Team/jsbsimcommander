/////////////////////////////////////////////////////////////////////////////
// Name:        import.cpp
// Purpose:     Implements importing in FCSBuilder
// Author:      Matthew Gong
// Created:     03/19/2005
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

#include <string>
#include <wx/regex.h>

#include "doc.h"
#include "view.h"
#include "shape.h"
#include "sum.h"
#include "gain.h"
#include "filter.h"
#include "deadband.h"
#include "switch.h"
#include "kinemat.h"
#include "func.h"
#include "import.h"
#include "Commander.h"
#include "Property_Diag.h"

void DiagramDocument::Import(JSBSim::Element * el)
{
  // read type in
  type = el->GetName().c_str();

  // read name in
  name = el->GetAttributeValue("name").c_str();

  // read properties in
  bool flag = false;
  JSBSim::Element * property = el->FindElement("property");
  prop_addition.Clear();
  while (property)
  {
    if (!flag)
      flag = true;
    prop_addition.Add(wxString(property->GetDataLine().c_str()));
    property = el->FindNextElement("property");
  }

  if (flag)
  {
    GetPropertyDialog()->ClearVar();
    for (int i=0; i<prop_addition.GetCount(); ++i)
    {
      GetPropertyDialog()->AddItem(prop_addition[i], wxT("user defined in FCS"), wxEmptyString, wxEmptyString, wxEmptyString);
    }
  }

  // read components in
  JSBSim::Element * channel = el->FindElement("channel");
  while (channel)
  {
    wxString channel_name = channel->GetAttributeValue("name");
    Channel c = CreateChannel (channel_name);
    //Load the channel diagram, if present.
    if (LoadObject(c.diagram, c.name))
    {
      channel = el->FindNextElement("channel");
      continue;
    }

    JSBSim::Element * component = channel->GetElement();

    //wxLogMessage(wxString(wxT("load channel "))+c.name+wxT(" directly."));
    TBoxMgr box_mgr(c.diagram);
    while (component) {
      std::string type = component->GetName();

      ComponentShape * shape;
      if ((type == "pure_gain") ||
          (type == "scheduled_gain") ||
          (type == "aerosurface_scale") )
      {
        shape = new Gain;
      } else if ( (type == "integrator") ||
                  (type == "lag_filter") ||
                  (type == "lead_lag_filter") ||
                  (type == "second_order_filter") ||
                  (type == "washout_filter") ||
                  (type == "integrator") )
      {
        shape = new Filter;
      } else if (type == "summer") {
        shape = new Summer;
      } else if (type == "deadband") {
        shape = new DeadBand;
      } else if (type == "switch") {
        shape = new Switch;
      } else if (type == "kinematic") {
        shape = new Kinemat;
      } else if (type == "function") {
        shape = new FCSFunction;
      } else {
        wxString str = type.c_str();
        str = _("Unknown component(") + str + _(")\n Please check the file.");
        ::wxMessageBox(str, _("Error"), wxOK | wxICON_ERROR );
        throw str;
      }

      shape->AssignNewIds ();
      shape->SetEventHandler(new MyEvtHandler(shape, shape));
      c.diagram->AddShape (shape);
      shape->SetOrder();

      shape->SetType(wxString(type.c_str()));
      wxArrayString inputs = shape->ImportXML(component);
      shape->SetCentreResize (false);
      shape->SetPen (wxBLACK_PEN);
      shape->SetBrush (wxWHITE_BRUSH);
      shape->SetSize (50, 50);

      box_mgr.Add(shape, inputs);

      component = channel->GetNextElement();
    }

    box_mgr.ShowAll();
    double maxx,maxy;
    box_mgr.GetSize(maxx,maxy);

    //reset margin
    if (maxx < 125*30) maxx = 125*30;
    if (maxy < 100*30) maxy = 100*30;

    c.canvas->SetScrollbars (30, 30, int(maxx/30)+10, int(maxy/30)+10);

    channel = el->FindNextElement("channel");
  }

  wxRegEx reg(wxT("[0-9]+$"));
  for (std::vector<Channel>::iterator iter = channels.begin();
      iter != channels.end(); ++iter)
  {
    wxDiagram * diagram = iter->diagram;
    if (diagram) {
      wxObjectList::compatibility_iterator node = diagram->GetShapeList ()->GetFirst ();
      while (node) {
        ComponentShape * shape =
          dynamic_cast<ComponentShape *>((wxShape *) node->GetData ());
        if (shape) {
          wxString name = shape->GetName();
          if (reg.Matches(name)) {
            name = reg.GetMatch(name);
            unsigned long tmp;
            if (name.ToULong(&tmp)) {
              if (unused <= tmp) {
                unused = tmp + 1u;
              }
            }
          }
        }
        node = node->GetNext();
      }
    }
  }
}


TBoxMgr::~TBoxMgr()
{
  while ( !boxes.empty() )
  {
    delete boxes.back();
    boxes.pop_back();
  }
}

void
TBoxMgr::Add(ComponentShape * shape, const wxArrayString & inputs)
{
  boxlist_iter_list tmp = isStandAlone(inputs);
  boxlist_iter iter = combine(shape, inputs, tmp);
  move_down(iter);
}

void
TBoxMgr::ShowAll()
{
  bool first_flag = true;
  wxClientDC * dcp = NULL;
  for (boxlist_iter i = boxes.begin(); i != boxes.end(); ++i)
  {
    TBox * box = *i;
    for (TBox::shapelist_iter j = box->shapes.begin(); j != box->shapes.end(); ++j)
    {
      wxShape * shape = *j;
      shape->Show (true);

      if (first_flag)
      {
        first_flag = false;
        dcp = new wxClientDC(shape->GetCanvas ());
        shape->GetCanvas ()->PrepareDC (*dcp);
      }

      shape->Move (*dcp, shape->GetX(), shape->GetY());
    }
  }
  delete dcp;
}

void
TBoxMgr::GetSize(double &x, double &y)
{
  double maxx=0;
  double maxy=0;
  for (boxlist_iter iter=boxes.begin();iter!=boxes.end();++iter)
  {
    double tmpx = (*iter)->max_x();
    double tmpy = (*iter)->max_y();
    if (tmpx > maxx)
      maxx = tmpx;
    if (tmpy > maxy)
      maxy = tmpy;
  }
  x = maxx;
  y = maxy;
}

TBoxMgr::boxlist_iter_list
TBoxMgr::isStandAlone(const wxArrayString & inputs)
{
  boxlist_iter_list tmp;
  for (boxlist_iter iter=boxes.begin();iter!=boxes.end();++iter)
  {
    if ( (*iter)->have_output_node )
      continue;
    wxString str = ComponentShape::mkName((*iter)->current_shape->GetName(),true);
    if ( inputs.Index(str,false,false) != wxNOT_FOUND && str != inputs[0] )
      tmp.push_back(iter);
  }
  return tmp;
}

TBoxMgr::boxlist_iter
TBoxMgr::combine(ComponentShape * shape, const wxArrayString & inputs, boxlist_iter_list & b)
{
  double total_y=0;
  double maxx;
  double miny;
  boxlist_iter_list_iter iter;

  TBox * box = new TBox;
  box->have_output_node = false;
  box->current_shape = shape;

  if ( boxes.empty() )
  {
    maxx = 100;
    miny = 100;
    boxes.push_back(box);
  }
  else
  {
    if ( b.size() == 0u)
    {
      maxx = 100;
      miny = 100 + boxes.back()->max_y();
      boxes.push_back(box);
    }
    else
    {
      iter = b.begin();
      maxx = (**iter)->max_x();
      miny = (**iter)->min_y();
      while (++iter != b.end())
      {
        if ( (**iter)->max_x() > maxx )
          maxx = (**iter)->max_x();
      }
      boxes.insert( *(b.begin()), box );
    }
  }

  for ( size_t i = 1; i < inputs.Count(); ++i)
  {
    wxString str = inputs[i];
    bool flag = false;
    for ( iter = b.begin(); iter != b.end(); ++iter)
    {
      if ( str == ComponentShape::mkName((**iter)->current_shape->GetName(), true) )
      {
        flag = true;
  break;
      }
    }
    if (flag)
    {
      //move
      double tmpx = (**iter)->max_x();
      double tmpy = (**iter)->min_y();
      (**iter)->move_delta(maxx-tmpx,miny-tmpy);
      miny = (**iter)->max_y()+100;
      total_y += (**iter)->current_shape->GetY();

      //add line
      wxLineShape  * line = new wxLineShape;
      line->AssignNewIds ();
      line->SetEventHandler(new MyEvtHandler(line, line));
      line->SetPen (wxBLACK_PEN);
      line->SetBrush (wxRED_BRUSH);
      line->MakeLineControlPoints (2);
      line->AddArrow (ARROW_ARROW, ARROW_POSITION_END, 10.0, 0.0,
          _T ("Normal arrowhead"));
      diagram->AddShape (line);
      (**iter)->current_shape->AddLine (line, shape, 0, i);
      (**iter)->current_shape->NormalizeLine (line);

      //del
      TBox * tmpbox = **iter;
      for (TBox::shapelist_iter j = tmpbox->shapes.begin(); j != tmpbox->shapes.end(); ++j)
      {
        box->shapes.push_back(*j);
      }
      boxlist_iter tmp = *iter;
      b.erase(iter);
      boxes.erase(tmp);
      delete tmpbox;

      box->shapes.push_back((wxShape *)line);
    }
    else
    {
      //add source
      SourceShape * sshape = new SourceShape;
      sshape->AssignNewIds ();
      sshape->SetEventHandler(new MyEvtHandler(sshape, sshape));
      sshape->SetCentreResize (false);
      sshape->SetPen (wxBLACK_PEN);
      sshape->SetBrush (wxWHITE_BRUSH);
      sshape->SetSize (50, 50);
      diagram->AddShape (sshape);
      sshape->SetName(str);
      sshape->SetX(maxx);
      sshape->SetY(miny);
      total_y += miny;
      miny += 100;
      box->shapes.push_back((wxShape *)sshape);

      //add line
      wxLineShape  * line = new wxLineShape;
      line->AssignNewIds ();
      line->SetEventHandler(new MyEvtHandler(line, line));
      line->SetPen (wxBLACK_PEN);
      line->SetBrush (wxRED_BRUSH);
      line->MakeLineControlPoints (2);
      line->AddArrow (ARROW_ARROW, ARROW_POSITION_END, 10.0, 0.0,
          _T ("Normal arrowhead"));
      diagram->AddShape (line);
      sshape->AddLine (line, shape, 0, i);
      sshape->NormalizeLine (line);
      box->shapes.push_back((wxShape *)line);
    }
  }

  //move shape to proper pos
  if (box->shapes.size() == 0u)
  {
    shape->SetX(maxx);
    shape->SetY(miny);
  }
  else
  {
    shape->SetX(maxx+=175);
    shape->SetY( total_y /(inputs.Count()-1.0) );
  }
  box->shapes.push_back((wxShape *)shape);

  // have output node
  if ( inputs.size() != 0u && !inputs[0].IsEmpty() )
  {
    box->have_output_node = true;
    DestinationShape * dshape = new DestinationShape;
    dshape->AssignNewIds ();
    dshape->SetEventHandler(new MyEvtHandler(dshape, dshape));
    dshape->SetCentreResize (false);
    dshape->SetPen (wxBLACK_PEN);
    dshape->SetBrush (wxWHITE_BRUSH);
    dshape->SetSize (50, 50);
    diagram->AddShape (dshape);
    dshape->SetName(inputs[0]);
    dshape->SetX( maxx+=175 );
    dshape->SetY( shape->GetY() );

    box->shapes.push_back((wxShape *)dshape);

    wxLineShape  * line = new wxLineShape;
    line->AssignNewIds ();
    line->SetEventHandler(new MyEvtHandler(line, line));
    line->SetPen (wxBLACK_PEN);
    line->SetBrush (wxRED_BRUSH);
    line->MakeLineControlPoints (2);
    line->AddArrow (ARROW_ARROW, ARROW_POSITION_END, 10.0, 0.0,
        _T ("Normal arrowhead"));
    diagram->AddShape (line);
    box->current_shape->AddLine (line, dshape, 0, 1);
    box->current_shape->NormalizeLine (line);
    box->shapes.push_back((wxShape *)line);
  }

  for (boxlist_iter k = boxes.begin(); k != boxes.end(); ++k)
  {
     if ( *k == box)
       return k;
  }
  return boxes.end();
}


void
TBoxMgr::move_down(boxlist_iter iter)
{
  if ( iter == boxes.end() )
    return;

  double y = (*iter)->max_y() + 100;

  while ( ++iter != boxes.end() )
  {
     double tmpy = (*iter)->min_y();
     (*iter)->move_delta(0, y-tmpy);
     y = (*iter)->max_y() + 100;
  }
}

double
TBox::min_x()
{
  double result;
  shapelist_iter i = shapes.begin();
  result = (*i)->GetX();
  while (++i != shapes.end())
  {
    if ( !(*i)->IsKindOf(CLASSINFO(wxLineShape)) )
    {
      double tmp = (*i)->GetX();
      if ( tmp < result )
        result = tmp;
    }
  }
  return result;
}

double
TBox::min_y()
{
  double result;
  shapelist_iter i = shapes.begin();
  result = (*i)->GetY();
  while (++i != shapes.end())
  {
    if ( !(*i)->IsKindOf(CLASSINFO(wxLineShape)) )
    {
      double tmp = (*i)->GetY();
      if ( tmp < result )
        result = tmp;
    }
  }
  return result;
}

double
TBox::max_x()
{
  double result;
  shapelist_iter i = shapes.begin();
  result = (*i)->GetX();
  while (++i != shapes.end())
  {
    if ( !(*i)->IsKindOf(CLASSINFO(wxLineShape)) )
    {
      double tmp = (*i)->GetX();
      if ( tmp > result )
        result = tmp;
    }
  }
  return result;
}

double
TBox::max_y()
{
  double result;
  shapelist_iter i = shapes.begin();
  result = (*i)->GetY();
  while (++i != shapes.end())
  {
    if ( !(*i)->IsKindOf(CLASSINFO(wxLineShape)) )
    {
      double tmp = (*i)->GetY();
      if ( tmp > result )
        result = tmp;
    }
  }
  return result;
}

void
TBox::move_delta(double dx, double dy)
{
  for (shapelist_iter i = shapes.begin(); i != shapes.end(); ++i)
  {
    //if ( !(*i)->IsKindOf(CLASSINFO(wxLineShape)) )
    //{
      (*i)->SetX((*i)->GetX()+dx);
      (*i)->SetY((*i)->GetY()+dy);
    //}
  }
}


