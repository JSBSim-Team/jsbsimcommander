/////////////////////////////////////////////////////////////////////////////
// Name:        doc.cpp
// Purpose:     Implements document functionality in FCSBuilder
// Author:      Matthew Gong
// Created:     02/04/2005
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

#include <wx/colordlg.h>
#include <wx/datetime.h>
#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/notebook.h>

#include "FGXMLParse.h"

#include "doc.h"
#include "view.h"
#include "shape.h"
#include "sum.h"
#include "sum_dlg.h"
#include "gain.h"
#include "gain_dlg.h"
#include "filter.h"
#include "filter_dlg.h"
#include "deadband.h"
#include "deadband_dlg.h"
#include "switch.h"
#include "switch_dlg.h"
#include "kinemat.h"
#include "kinemat_dlg.h"
#include "fcsprop_dlg.h"
#include "toolbar.h"
#include "Commander.h"
#include "AircraftDialog.h"
#include "Property_Diag.h"
#include "func.h"
#include "func_dlg.h"

BEGIN_EVENT_TABLE (DiagramDocument, wxPanel)
  EVT_MENU (FCSBUILDER_EXPORT, DiagramDocument::OnExportXML)
  EVT_MENU (FCSBUILDER_IMPORT, DiagramDocument::OnImportXML)
  EVT_TOOL (FCSBUILDER_GENERAL, DiagramDocument::OnGeneral)
  EVT_TOOL (FCSBUILDER_ADD_CHANNEL, DiagramDocument::OnAddChannel)
  EVT_TOOL (FCSBUILDER_SNAP_CHANNEL, DiagramDocument::OnSnapChannel)
  EVT_MENU (FCSBUILDER_DEL, DiagramDocument::OnDel)
  EVT_TOOL (TOOLBAR_ADJ_PROPERTY, DiagramDocument::OnSetShapeProperty)
  EVT_TOOL (TOOLBAR_ADJ_REVERSE_SHAPE, DiagramDocument::OnShapeReverse)
  EVT_TOOL (TOOLBAR_ADJ_NEW_POINT, DiagramDocument::OnLineNewPoint)
  EVT_TOOL (TOOLBAR_ADJ_DEL_POINT, DiagramDocument::OnLineCutPoint)
  EVT_TOOL (TOOLBAR_ADJ_STRAIGHTEN_LINE, DiagramDocument::OnLineStraighten) 
END_EVENT_TABLE ()

DiagramDocument::DiagramDocument(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
  :
  wxPanel(parent, id, pos, size, style),
  unused(0),
  type(wxT("flight_control")),
  name(wxT("fcs"))
{
  //canvas = CreateCanvas (frame);

  addtb = CreateAddToolBar (this);
  adjtb = CreateAdjToolBar (this);
  notebook = new wxNotebook(this, -1);

  wxBoxSizer *frameSizer = new wxBoxSizer (wxVERTICAL);
  frameSizer->Add (addtb, 0, wxEXPAND|wxALL, 2);
  frameSizer->Add (adjtb, 0, wxEXPAND|wxALL, 2);
  frameSizer->Add (notebook, 1, wxEXPAND|wxALL, 5);

  SetSizer (frameSizer);
  SetAutoLayout (true);
  Layout ();
  Show (true);
}


DiagramDocument::~DiagramDocument (void)
{
  for (std::vector<Channel>::iterator i = channels.begin(); i != channels.end(); ++i)
  {
    (*i).diagram->SetCanvas (NULL);
    (*i).canvas->SetDiagram (NULL);
    delete (*i).diagram;
  } 
}

void DiagramDocument::ClearAll()
{
  for (std::vector<Channel>::iterator i = channels.begin(); i != channels.end(); ++i)
  {
    (*i).diagram->SetCanvas (NULL);
    (*i).canvas->SetDiagram (NULL);
    delete (*i).diagram;
  } 

  notebook->DeleteAllPages();
  channels.clear();
}

#if wxUSE_STD_IOSTREAM

bool DiagramDocument::SaveObject(MyDiagram * diagram, const wxString & name)
{
  if (name.Length()==0)
    return false;
#if wxUSE_PROLOGIO
  AircraftDialog * p = dynamic_cast<AircraftDialog *>(GetParent()->GetParent());
  if (p)
  {
    wxString buf = p->GetFileName();
    buf = buf.BeforeLast(wxGetApp().Delimited[0]);
    buf += wxGetApp().Delimited + name + wxT(".dia");
    diagram->SaveFile (buf);
  }
#endif

  return true;
}

bool DiagramDocument::LoadObject(MyDiagram * diagram, const wxString & name)
{
#if wxUSE_PROLOGIO
  AircraftDialog * p = dynamic_cast<AircraftDialog *>(GetParent()->GetParent());
  if (p)
  {
    wxString file = p->GetFileName();
    wxString buf = file.BeforeLast(wxGetApp().Delimited[0]);
    buf += wxGetApp().Delimited + name + wxT(".dia");
    if (!wxFileExists(buf))
      return false;
    wxFileName filename(file);
    wxFileName bufname(buf);
    wxDateTime fa, fm, fc, ba, bm, bc;
    filename.GetTimes(&fa, &fm, &fc);
    bufname.GetTimes(&ba, &bm, &bc);
    if (bm.IsEarlierThan(fm.Add(wxTimeSpan(0, 0, -10, 0))) 
      && ::wxMessageBox(_("FCS model file ")+ name + _(" seems older than aircraft model file. ") + _("Do you really want to load FCS model file?"), _("Confirm"), wxYES_NO | wxICON_QUESTION, this) == wxNO)
    {
      return false;
    }
    else
    {
      diagram->DeleteAllShapes ();
      diagram->LoadFile (buf);
      return true;
    }
  }
#endif

  return false;
}

#else
#error set wxUSE_STD_IOSTREAM=1
#endif

/***************************************************************
bool DiagramDocument::OnSaveDocument(const wxString& filename)
{
  if (wxDocument::OnSaveDocument(filename))
  {
    {
      wxFileInputStream is(filename);
      wxTextInputStream tis(is);
      wxFileOutputStream os(filename+wxT(".test"));
      wxTextOutputStream tos(os, wxEOL_UNIX);
      wxString tmp;
      while (!is.Eof())
      {
        tmp = tis.ReadLine();
        tos << tmp << endl;
      }
    }
    if (wxRemoveFile(filename))
    {
      wxRenameFile(filename+wxT(".test"), filename);
    }
    wxString xmlfile = filename.BeforeLast('.') + wxT(".xml");
    wxFileOutputStream stream(xmlfile);
    if (stream.Ok())
    {
      ExportXML(stream);
    }
    return true;
  }
  else
  {
    return false;
  }
}

******************************************************************************/


static int ccompare(const void *arg1, const void *arg2)
{
  const ComponentShape ** c1 = (const ComponentShape **)arg1;
  const ComponentShape ** c2 = (const ComponentShape **)arg2;
  long int order1 = (**c1).GetOrder();
  long int order2 = (**c2).GetOrder();
  if (order1 == order2)
    return 0;
  else if (order1 > order2)
    return 1;
  else
    return -1;
}

bool DiagramDocument::ExportXML(const wxString & filename)
{
  wxFileOutputStream stream(filename);
  if (!stream.Ok())
    return false;
  wxTextOutputStream tstream(stream,wxEOL_UNIX);
  tstream << wxT("<?xml version=\"1.0\"?>") << endl;
  tstream << wxT("<!--  Generated by FCS Builder for JSBSim -->\n") << endl ;
  
  Export(tstream, wxEmptyString);

  return true;
}

void DiagramDocument::Export(wxTextOutputStream & tstream, const wxString & prefix)
{
  tstream << endl << prefix << wxT("<") << type << wxT(" name=\"") << name << wxT("\">") << endl;

  wxString pre = prefix + wxT("    ");
  int n;
  if ( (n = prop_addition.GetCount()) > 0 )
  {
    tstream << wxT("\n<!-- INTERFACE PROPERTIES -->\n") << endl ;
    for (int i = 0; i< n; ++i)
    {
      tstream << pre << wxT("<property>") << prop_addition[i] << wxT("</property>") << endl ;
    }
  }
  
  for ( std::vector<Channel>::iterator i = channels.begin(); i != channels.end(); ++i)
  {
  
    SaveObject((*i).diagram, (*i).name);
    wxList clist;
    clist.DeleteContents(false);
    wxList * slist = (*i).diagram->GetShapeList();
    wxNode * node = slist->GetFirst();
    while (node)
    {
      wxShape * value = (wxShape *)node->GetData();
      if (value->IsKindOf(CLASSINFO(ComponentShape)))
      {
        clist.Append(value);
      }
      node = node->GetNext();
    }

    if (clist.GetCount() == 0u)
      continue;

    tstream << endl << pre << wxT("<channel name=\"") << (*i).name << wxT("\">") << endl;

    clist.Sort(ccompare);
    node = clist.GetFirst();
    while (node)
    {
      ComponentShape * value = (ComponentShape *)node->GetData();
      value->ExportXML(tstream, pre+wxT("    "));
      node = node->GetNext();
    }

    tstream << endl << pre << wxT("</channel>") << endl;
  }
    
  tstream << endl << prefix  << wxT("</") << type << wxT(">") << endl;
}

bool DiagramDocument::ImportXML(const wxString & filename)
{

  JSBSim::FGXMLParse controls_file_parser;
  
  try
  {
    readXML (filename.c_str(), controls_file_parser);
  }
  catch (...)
  {
    return false;
  }

  JSBSim::Element *document = controls_file_parser.GetDocument();
  JSBSim::Element *autopilot=0, *flight_control=0;
  JSBSim::Element * tmp;


  if ( document->GetName() == "autopilot" )
    autopilot = document;

  if ( document->GetName() == "flight_control" )
    flight_control = document;

  if ( !autopilot && !flight_control)
  {
    ::wxMessageBox(wxT("Can not find FCS components in this file! Check it."),
	           wxT("Error"),
		   wxOK | wxICON_ERROR
		   );
    return false;
  }
  
  if (autopilot)
    Import(autopilot);
  
  if (flight_control)
    Import(flight_control);
  
  return true;
}

Channel DiagramDocument::CreateChannel (const wxString & channel_name)
{
  int width, height;
  GetClientSize (&width, &height);

  // Non-retained canvas
  MyCanvas *canvas =
    new MyCanvas (notebook, ID_CANVAS, wxPoint (0, 0),
		  wxSize (width, height), 0);
  canvas->SetCursor (wxCursor (wxCURSOR_HAND));

  // Give it scrollbars
  canvas->SetScrollbars (30, 30, 125, 100);

  MyDiagram * diagram = new MyDiagram;
  
  canvas->SetDiagram (diagram);
  diagram->SetCanvas (canvas);
  
  Channel channel;
  channel.name = channel_name;
  channel.canvas = canvas;
  channel.diagram = diagram;
  channels.push_back(channel);
  
  notebook->AddPage(canvas, channel_name);
  notebook->Layout();

  return channel;
}


wxDiagram *DiagramDocument::GetActiveDiagram ()
{
  MyCanvas * cur = dynamic_cast<MyCanvas *>(notebook->GetCurrentPage());
  if (cur)
  {
    return cur->GetDiagram();
  }
  else
  {
    return NULL;
  }
}

  
wxShape *
DiagramDocument::FindSelectedShape (void)
{
  wxDiagram * diagram = GetActiveDiagram();
  if (diagram)
  {
    wxObjectList::compatibility_iterator node = diagram->GetShapeList ()->GetFirst ();
    while (node)
    {
      wxShape *eachShape = (wxShape *) node->GetData ();
      if ((eachShape->GetParent () == NULL) && eachShape->Selected ())
	    {
	      return eachShape;
	    }
	    node = node->GetNext();
    }
  }
  return NULL;
}

void
DiagramDocument::OnExportXML (wxCommandEvent & WXUNUSED (event))
{
  wxFileDialog dlg(this, _("Input the file name"), 
      wxT(""), wxT("noname.xml"), wxT("XML files(*.xml)|*.xml"),
      wxSAVE | wxOVERWRITE_PROMPT );
  if ( dlg.ShowModal() == wxID_OK )
  {
    wxString path = dlg.GetPath();
    if (path.AfterLast('.')!=wxT("xml"))
      path += wxT(".xml");
    ExportXML(path);
  }
}


void
DiagramDocument::OnImportXML (wxCommandEvent & WXUNUSED (event))
{
  ClearAll();
  wxFileDialog dlg(this, _("Input the file name"), 
      wxT(""), wxT("noname.xml"), wxT("XML files(*.xml)|*.xml"),
      wxOPEN);
  if ( dlg.ShowModal() == wxID_OK )
  {
    wxString path = dlg.GetPath();
    if (path.AfterLast('.')!=wxT("xml"))
      path += wxT(".xml");
    if ( ImportXML(path) )
    {
      ::wxMessageBox(_("Secceed in loading."),
	           _("Info"),
	           wxOK | wxICON_INFORMATION
		   );
    }
    else
    {
      ::wxMessageBox(_("Fail to load."),
	           _("Warning"),
	           wxOK | wxICON_ERROR
		   );    
    }
  }
}

void
DiagramDocument::OnDel (wxCommandEvent & WXUNUSED (event))
{
  if (!GetActiveDiagram())
    return;

  wxShape *shape = FindSelectedShape ();
  if (shape)
  {
	    shape->Select (false);

	    // Generate commands to explicitly remove each connected line.
	    RemoveLines (shape);

	    GetActiveDiagram()->RemoveShape (shape);
	    shape->Unlink ();
      shape->GetCanvas ()->Refresh ();
  }
}

void
DiagramDocument::RemoveLines (wxShape * shape)
{
  if (!GetActiveDiagram())
    return;

  wxObjectList::compatibility_iterator node = shape->GetLines ().GetFirst ();
  while (node)
    {
      wxLineShape *line = (wxLineShape *) node->GetData ();
      {
	      line->Select (false);

	      GetActiveDiagram()->RemoveShape (line);
	      line->Unlink ();
      }
      node = shape->GetLines ().GetFirst ();
    }
}


void
DiagramDocument::OnGeneral (wxCommandEvent & WXUNUSED (event))
{

  FCSPropDialog dlg(type, name, prop_addition, this, -1);
  if ( dlg.ShowModal() == wxID_OK )
  {
    type = dlg.type;
    name = dlg.name;
    prop_addition = dlg.data;
    GetPropertyDialog()->ClearVar();
    for (int i=0; i<prop_addition.GetCount(); ++i)
    {
      GetPropertyDialog()->AddItem(prop_addition[i], _("user defined in FCS"), wxEmptyString, wxEmptyString, wxEmptyString);
    }
  }
}

void
DiagramDocument::OnAddChannel (wxCommandEvent & WXUNUSED (event))
{
   wxString channel_name = wxGetTextFromUser(_("New Channel Name :"), _("Input the new channel name"), wxT(""), this);
   if (!channel_name.IsEmpty())
   {
     CreateChannel(channel_name);
   }
}

void
DiagramDocument::OnSnapChannel (wxCommandEvent & WXUNUSED (event))
{
  MyCanvas * cur = dynamic_cast<MyCanvas *>(notebook->GetCurrentPage());
  if (cur)
  {
    int w = cur->GetCanvasWidth();
    int h = cur->GetCanvasHeight();
    wxBitmap bitmap(w, h);
    wxMemoryDC dc;
    dc.SelectObject(bitmap);
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.DrawRectangle(0, 0, w, h);
    cur->DrawPic(&dc, w, h);
    wxString tmpfile = wxFileSelector(_("Get FCS snap picture"),
    		wxEmptyString, wxT("snapFCS.bmp"), wxT("bmp"),
    		wxT("BMP files (*.bmp)|*.bmp"), 
    		wxSAVE|wxOVERWRITE_PROMPT, this);
    if (!tmpfile.IsEmpty())
    {
      bitmap.SaveFile(tmpfile, wxBITMAP_TYPE_BMP);
    }
  }
}

void 
DiagramDocument::SetProperty(wxShape * theShape)
{
  if (!theShape)
    return;
  if (theShape->IsKindOf (CLASSINFO (SourceShape)))
    {
      GetPropertyDialog()->Update();
      GetPropertyDialog()->Select(((MISOShape *)theShape)->GetName());
      if ( GetPropertyDialog()->ShowModal() == wxID_OK )
	    {
	      ((MISOShape *)theShape)->SetName(GetPropertyDialog()->GetResult());
	    }
    }
  else if (theShape->IsKindOf (CLASSINFO (DestinationShape)))
    {
      GetPropertyDialog()->Update();
      GetPropertyDialog()->Select(((MISOShape *)theShape)->GetName());
      if ( GetPropertyDialog()->ShowModal() == wxID_OK )
	    {
	      ((MISOShape *)theShape)->SetName(GetPropertyDialog()->GetResult());
	    }
    }
  else if (theShape->IsKindOf (CLASSINFO (Gain)))
    {
      GainPropertyDialog  dlg((Gain *)theShape, wxGetApp().source_property, this, wxID_ANY, _("Gain Componment"));
      if ( dlg.ShowModal() == wxID_OK )
	    {
    	  dlg.SetDataOut((Gain *)theShape);
      }
    }
  else if (theShape->IsKindOf (CLASSINFO (Summer)))
    {
      SummerPropertyDialog  dlg((Summer *)theShape, this,wxID_ANY,_("Summer Componment"));
      if ( dlg.ShowModal() == wxID_OK )
	    {
	      dlg.SetDataOut((Summer *)theShape);
    	}
    }
  else if (theShape->IsKindOf (CLASSINFO (Filter)))
    {
      FilterPropertyDialog  dlg((Filter *)theShape, this,wxID_ANY,_("Filter Componment"));
      if ( dlg.ShowModal() == wxID_OK )
	    {
	      dlg.SetDataOut((Filter *)theShape);
	    }
    }
  else if (theShape->IsKindOf (CLASSINFO (DeadBand)))
    {
      DeadBandPropertyDialog  dlg((DeadBand *)theShape, this,wxID_ANY,_("DeadBand Componment"));
      if ( dlg.ShowModal() == wxID_OK )
    	{
	      dlg.SetDataOut((DeadBand *)theShape);
    	}
    }
  else if (theShape->IsKindOf (CLASSINFO (Switch)))
    {
      SwitchPropertyDialog  dlg((Switch *)theShape, this,wxID_ANY,_("Switch Componment"));
      if ( dlg.ShowModal() == wxID_OK )
	    {
	      dlg.SetDataOut((Switch *)theShape);
	    }
    }
  else if (theShape->IsKindOf (CLASSINFO (Kinemat)))
    {
      KinematPropertyDialog  dlg((Kinemat *)theShape, this,wxID_ANY,_("Kinemat Componment"));
      if ( dlg.ShowModal() == wxID_OK )
	    {
	      dlg.SetDataOut((Kinemat *)theShape);
	    }
    }
  else if (theShape->IsKindOf (CLASSINFO (FCSFunction)))
    {
      FCSFunctionDialog  dlg((FCSFunction *)theShape, this, wxID_ANY);
      if ( dlg.ShowModal() == wxID_OK )
	    {
	      dlg.SetDataOut((FCSFunction *)theShape);
	    }
    }

  theShape->GetCanvas ()->Refresh ();
}


void
DiagramDocument::OnSetShapeProperty(wxCommandEvent & WXUNUSED (event))
{
  wxShape *theShape = FindSelectedShape ();
  SetProperty(theShape);
}


void
DiagramDocument::OnShapeReverse(wxCommandEvent & WXUNUSED (event))
{
  wxShape *theShape = FindSelectedShape ();
  if (theShape && theShape->IsKindOf (CLASSINFO (MISOShape)))
    {
      MISOShape * shape = (MISOShape *)theShape;
      shape->Select (false);

      shape->SetDrawReverse(!shape->IsDrawReverse());

      shape->Select (true);
      theShape->GetCanvas ()->Refresh ();
    }
}

void
DiagramDocument::OnLineNewPoint (wxCommandEvent & WXUNUSED (event))
{
  wxShape *theShape = FindSelectedShape ();
  if (theShape && theShape->IsKindOf (CLASSINFO (wxLineShape)))
    {
      wxLineShape *line = (wxLineShape *) theShape;
      line->Select (false);

      wxClientDC dc (theShape->GetCanvas ());
      theShape->GetCanvas ()->PrepareDC (dc);

      line->InsertLineControlPoint (&dc);

      line->Select (true);
      theShape->GetCanvas ()->Refresh ();
    }
}

void
DiagramDocument::OnLineCutPoint (wxCommandEvent & WXUNUSED (event))
{
  wxShape *theShape = FindSelectedShape ();
  if (theShape && theShape->IsKindOf (CLASSINFO (wxLineShape)))
    {
      wxLineShape *line = (wxLineShape *) theShape;
      line->Select (false);

      line->DeleteLineControlPoint ();

      line->Select (true);
      theShape->GetCanvas ()->Refresh ();
    }
}

void
DiagramDocument::OnLineStraighten (wxCommandEvent & WXUNUSED (event))
{
  wxShape *theShape = FindSelectedShape ();
  if (theShape && theShape->IsKindOf (CLASSINFO (wxLineShape)))
    {
      wxLineShape *line = (wxLineShape *) theShape;
      line->Select (false);

      wxClientDC dc (theShape->GetCanvas ());
      theShape->GetCanvas ()->PrepareDC (dc);

      line->Straighten (&dc);

      line->Select (true);
      theShape->GetCanvas ()->Refresh ();
    }
}


/*
 * Implementation of drawing command
 */

/*****************************************************************************

DiagramCommand::DiagramCommand (const wxString & name, int command, DiagramDocument * ddoc, wxClassInfo * info, double xx, double yy, bool sel, wxShape * theShape, wxShape * fs, wxShape * ts, int from, int to):
wxCommand (true, name)
{
  doc = ddoc;
  cmd = command;
  shape = theShape;
  fromShape = fs;
  toShape = ts;
  attachFrom = from;
  attachTo = to;
  shapeInfo = info;
  shapeBrush = NULL;
  shapePen = NULL;
  x = xx;
  y = yy;
  selected = sel;
  deleteShape = false;
}

DiagramCommand::DiagramCommand (const wxString & name, int command,
				DiagramDocument * ddoc,
				wxBrush * backgroundColour,
				wxShape * theShape):
wxCommand (true, name)
{
  doc = ddoc;
  cmd = command;
  shape = theShape;
  fromShape = NULL;
  toShape = NULL;
  attachFrom = 0;
  attachTo = 0;
  shapeInfo = NULL;
  x = 0.0;
  y = 0.0;
  selected = false;
  deleteShape = false;
  shapeBrush = backgroundColour;
  shapePen = NULL;
}


DiagramCommand::~DiagramCommand (void)
{
  if (shape && deleteShape)
    {
      shape->SetCanvas (NULL);
      delete shape;
    }
}

bool
DiagramCommand::Do (void)
{
  switch (cmd)
    {
    case FCSBUILDER_DEL:
      {
	if (shape)
	  {
	    deleteShape = true;

	    shape->Select (false);

	    // Generate commands to explicitly remove each connected line.
	    RemoveLines (shape);

	    doc->GetDiagram ()->RemoveShape (shape);
	    if (shape->IsKindOf (CLASSINFO (wxLineShape)))
	      {
		wxLineShape *lineShape = (wxLineShape *) shape;
		fromShape = lineShape->GetFrom ();
		toShape = lineShape->GetTo ();
	      }
	    shape->Unlink ();

	    doc->Modify (true);
	    doc->UpdateAllViews ();
	  }

	break;
      }
    case FCSBUILDER_ADD_SHAPE:
      {
	wxShape *theShape;
	if (shape)
	  theShape = shape;	// Saved from undoing the shape
	else
	  {
	    theShape = (wxShape *) shapeInfo->CreateObject ();
	    theShape->AssignNewIds ();
	    theShape->SetEventHandler(new MyEvtHandler(theShape, theShape));
	    theShape->SetCentreResize (false);
	    theShape->SetPen (wxBLACK_PEN);
	    theShape->SetBrush (wxWHITE_BRUSH);
	    if (theShape->IsKindOf(CLASSINFO(SIMOShape)))
	      theShape->SetSize (10, 10);
	    else  
	      theShape->SetSize (60, 60);
	  }
	doc->GetDiagram ()->AddShape (theShape);
	if (theShape->IsKindOf(CLASSINFO(ComponentShape)))
	  ((ComponentShape*)theShape)->SetOrder();

	theShape->Show (true);

	wxClientDC dc (theShape->GetCanvas ());
	theShape->GetCanvas ()->PrepareDC (dc);

	theShape->Move (dc, x, y);

	shape = theShape;
	deleteShape = false;

	doc->Modify (true);
	doc->UpdateAllViews ();
	break;
      }
    case FCSBUILDER_ADD_LINE:
      {
	wxShape *theShape;
	if (shape)
	  theShape = shape;	// Saved from undoing the line
	else
	  {
	    theShape = (wxShape *) shapeInfo->CreateObject ();
	    theShape->AssignNewIds ();
	    theShape->SetEventHandler(new MyEvtHandler(theShape, theShape));
	    theShape->SetPen (wxBLACK_PEN);
	    theShape->SetBrush (wxRED_BRUSH);

	    wxLineShape *lineShape = (wxLineShape *) theShape;

	    // Yes, you can have more than 2 control points, in which case
	    // it becomes a multi-segment line.
	    lineShape->MakeLineControlPoints (3);
	    lineShape->AddArrow (ARROW_ARROW, ARROW_POSITION_END, 10.0, 0.0,
				 _("Normal arrowhead"));
	  }

	doc->GetDiagram ()->AddShape (theShape);

	fromShape->AddLine ((wxLineShape *) theShape, toShape, attachFrom,
			    attachTo);
	if (fromShape->IsKindOf (CLASSINFO (MISOShape)))
	  ((MISOShape *) fromShape)->
	    NormalizeLine ((wxLineShape *) theShape);

	theShape->Show (true);

	wxClientDC dc (theShape->GetCanvas ());
	theShape->GetCanvas ()->PrepareDC (dc);

	// It won't get drawn properly unless you move both
	// connected images
	fromShape->Move (dc, fromShape->GetX (), fromShape->GetY ());
	toShape->Move (dc, toShape->GetX (), toShape->GetY ());

	shape = theShape;
	deleteShape = false;

	doc->Modify (true);
	doc->UpdateAllViews ();
	break;
      }
    case FCSBUILDER_CHANGE_BACKGROUND_COLOUR:
      {
	if (shape)
	  {
	    wxClientDC dc (shape->GetCanvas ());
	    shape->GetCanvas ()->PrepareDC (dc);

	    wxBrush *oldBrush = shape->GetBrush ();
	    shape->SetBrush (shapeBrush);
	    shapeBrush = oldBrush;
	    shape->Draw (dc);

	    doc->Modify (true);
	    doc->UpdateAllViews ();
	  }

	break;
      }
    }
  return true;
}

bool
DiagramCommand::Undo (void)
{
  switch (cmd)
    {
    case FCSBUILDER_DEL:
      {
	if (shape)
	  {
	    doc->GetDiagram ()->AddShape (shape);
	    shape->Show (true);

	    if (shape->IsKindOf (CLASSINFO (wxLineShape)))
	      {
		wxLineShape *lineShape = (wxLineShape *) shape;

		fromShape->AddLine (lineShape, toShape);
	      }
	    if (selected)
	      shape->Select (true);

	    deleteShape = false;
	  }
	doc->Modify (true);
	doc->UpdateAllViews ();
	break;
      }
    case FCSBUILDER_ADD_SHAPE:
    case FCSBUILDER_ADD_LINE:
      {
	if (shape)
	  {
	    wxClientDC dc (shape->GetCanvas ());
	    shape->GetCanvas ()->PrepareDC (dc);

	    shape->Select (false, &dc);
	    doc->GetDiagram ()->RemoveShape (shape);
	    shape->Unlink ();
	    deleteShape = true;
	  }
	doc->Modify (true);
	doc->UpdateAllViews ();
	break;
      }
    case FCSBUILDER_CHANGE_BACKGROUND_COLOUR:
      {
	if (shape)
	  {
	    wxClientDC dc (shape->GetCanvas ());
	    shape->GetCanvas ()->PrepareDC (dc);

	    wxBrush *oldBrush = shape->GetBrush ();
	    shape->SetBrush (shapeBrush);
	    shapeBrush = oldBrush;
	    shape->Draw (dc);

	    doc->Modify (true);
	    doc->UpdateAllViews ();
	  }
	break;
      }
    }
  return true;
}

// Remove each individual line connected to a shape by sending a command.
void
DiagramCommand::RemoveLines (wxShape * shape)
{
  wxObjectList::compatibility_iterator node = shape->GetLines ().GetFirst ();
  while (node)
    {
      wxLineShape *line = (wxLineShape *) node->GetData ();
      doc->GetCommandProcessor ()->
	Submit (new
		DiagramCommand (_T ("Cut"), FCSBUILDER_DEL, doc, NULL, 0.0, 0.0,
				line->Selected (), line));

      node = shape->GetLines ().GetFirst ();
    }
}
*****************************************************************************/

/*
 * Diagram
 */

#if wxUSE_PROLOGIO

inline 
bool
MyDiagram::OnShapeSave (wxExprDatabase & db, wxShape & shape, wxExpr & expr)
{
  bool flag = wxDiagram::OnShapeSave (db, shape, expr);
  return flag;
}

inline
bool
MyDiagram::OnShapeLoad (wxExprDatabase & db, wxShape & shape, wxExpr & expr)
{
  bool flag = wxDiagram::OnShapeLoad (db, shape, expr);
  MyEvtHandler *handler = new MyEvtHandler(&shape, &shape);
  shape.SetEventHandler(handler);
  return flag;
}

#endif

