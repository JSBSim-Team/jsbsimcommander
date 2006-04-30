AM_CPPFLAGS = -I$(top_srcdir)/src \
              -I$(top_srcdir)/src/math \
              -I$(top_srcdir)/src/input_output \
              -I$(top_srcdir)/src/simgear/xml \
	      -I$(top_srcdir)/src/gui \
              -I$(top_srcdir)/src/bitmaps \
              -DwxUSE_DEPRECATED=1 -DwxUSE_PROLOGIO=1 \
              ${WX_CXXFLAGS}
	      
noinst_LIBRARIES = libGUI.a

libGUI_a_SOURCES = \
AeroDynamics_Diag.cpp  ResultShow_Diag.cpp  fcsprop_dlg.cpp  kinemat_dlg.cpp \
AircraftDialog.cpp     Table_Diag.cpp       filter.cpp       shape.cpp \
Axis_Diag.cpp          TreeItem.cpp         filter_dlg.cpp   sum.cpp \
Engine_Diag.cpp        Turbine_Diag.cpp     func.cpp         sum_dlg.cpp \
Function_Diag.cpp      When_Diag.cpp        func_dlg.cpp     switch.cpp \
LGear_Diag.cpp         condition.cpp        gain.cpp         switch_dlg.cpp \
Mass_Diag.cpp          deadband.cpp         gain_dlg.cpp     toolbar.cpp \
Property_Diag.cpp      deadband_dlg.cpp     import.cpp       view.cpp \
Propulsion_Diag.cpp    doc.cpp              kinemat.cpp      EngineMgr_Diag.cpp
                   

noinst_HEADERS = \
AeroDynamics_Diag.h  ResultShow_Diag.h  fcsprop_dlg.h  kinemat_dlg.h \
AircraftDialog.h     Table_Diag.h       filter.h       shape.h \
Axis_Diag.h          TreeItem.h         filter_dlg.h   sum.h \
Engine_Diag.h        Turbine_Diag.h     func.h         sum_dlg.h \
Function_Diag.h      When_Diag.h        func_dlg.h     switch.h \
LGear_Diag.h         condition.h        gain.h         switch_dlg.h \
Mass_Diag.h          deadband.h         gain_dlg.h     toolbar.h \
Property_Diag.h      deadband_dlg.h     import.h       view.h \
Propulsion_Diag.h    doc.h              kinemat.h      EngineMgr_Diag.h


