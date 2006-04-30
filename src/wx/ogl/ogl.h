/////////////////////////////////////////////////////////////////////////////
// Name:        ogl.h
// Purpose:     OGL main include
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _OGL_OGL_H_
#define _OGL_OGL_H_


    #define WXDLLIMPEXP_OGL



#include "wx/ogl/basic.h"      // Basic shapes
#include "wx/ogl/basicp.h"
#include "wx/ogl/lines.h"      // Lines and splines
#include "wx/ogl/linesp.h"
#include "wx/ogl/divided.h"    // Vertically-divided rectangle
#include "wx/ogl/composit.h"   // Composite images
#include "wx/ogl/canvas.h"     // wxShapeCanvas for displaying objects
#include "wx/ogl/ogldiag.h"    // wxDiagram

#include "wx/ogl/bmpshape.h"
#include "wx/ogl/constrnt.h"
#include "wx/ogl/drawn.h"
#include "wx/ogl/drawnp.h"
#include "wx/ogl/mfutils.h"
#include "wx/ogl/misc.h"

// TODO: replace with wxModule implementation
extern WXDLLIMPEXP_OGL void wxOGLInitialize();
extern WXDLLIMPEXP_OGL void wxOGLCleanUp();

#endif
 // _OGL_OGL_H_
