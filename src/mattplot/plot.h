/////////////////////////////////////////////////////////////////////////////
// Name:        plot.h
// Purpose:     Plot demo for wxWidgets
// Author:      Matthew Gong
// Modified by:
// Created:     2005
// Copyright:   (c) Matthew Gong
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "plot.h"
#endif

#include <wx/print.h>
#include <wx/txtstrm.h>

#include <vector>
#include <map>
using std::vector;
using std::map;

#include "input_output/FGXMLElement.h"
#include "input_output/FGPropertyManager.h"
using JSBSim::Element;

namespace JSBSim
{
  class FGFunction;
};
using JSBSim::FGFunction;

#include "Condition.h"
using Matt::Condition;

#include "math/bspline.h"

#include "Table.h"
using Matt::Table;
using Matt::TableList;
using Matt::TableListIter;

/** Define the combine of X & Y axis direction.
  * Define all possiable combination for X & Y axises.
  */
   
enum AXISDIR{
      X_RIGHT_Y_UP,
      X_RIGHT_Y_DOWN,
      X_LEFT_Y_UP,
      X_LEFT_Y_DOWN,
      X_UP_Y_LEFT,
      X_UP_Y_RIGHT,
      X_DOWN_Y_LEFT,
      X_DOWN_Y_RIGHT
};


/** Curve to be drawed.
  * Base on B-Spline, and define its pen and font, etc.
  */

class Curve : public Matt::BSplineD
{
  public :
    /** Default Constructor.
     * Call Load later.
     *
     * @see Load
     */
    Curve();

    /** Constructor
     * construct a curve with array of (x, y)
     * 
     * @param x The array of x cood.
     * @param nx The size of the array.
     * @param y The array of y cood.
     */
    Curve(const double * x, size_t nx, const double * y);

    /** Constructor
     * construct a curve with array of (x, y)
     * 
     * @param x The array of x cood in the vector.
     * @param y The array of y cood in the vector.
     */
    Curve(vector<double> x, vector<double> y);

    /// the type of symbol drawed on the curve.
    enum CURVE_SYMBOL{
      CURVE_REC,
      CURVE_TRI_UP,
      CURVE_TRI_DOWN,
      CURVE_TRI_LEFT,
      CURVE_TRI_RIGHT,
      CURVE_DIAMOND,
      CURVE_PENTAGON,
      CURVE_HEXAGON,
      CURVE_NONE
    };

    /// the type of curve, S-spline or polyline.
    enum CURVE_TYPE{
      CURVE_BSPLINE,
      CURVE_POLYLINE
    };

    enum CURVE_TEXT{
      CURVE_NO_TEXT,
      CURVE_LEFT_UP,
      CURVE_LEFT_DOWN,
      CURVE_RIGHT_UP,
      CURVE_RIGHT_DOWN
    };

    enum CURVE_LABEL{
      CURVE_LABEL_NONE,
      CURVE_LABEL_PREFIX,
      CURVE_LABEL_SUFFIX,
      CURVE_LABEL_REPLACE,
    };

    /** Draw the curve on the dc.
     * 
     * @param dc The DC to draw on.
     * @param x  The X coord of the start point on the DC.
     * @param y  The Y coord of the start point on the DC.
     * @param w  The width of the area on the DC.
     * @param h  The height of the area on the DC.
     * @param minx  The min X coord of the curve.
     * @param maxx  The max X coord of the curve.
     * @param minu  The min Y coord of the curve.
     * @param maxx  The max Y coord of the curve.
     * @param axis_dir The direction of axises.
     */
    void DrawCurve(wxDC& dc, wxCoord x, wxCoord y, wxCoord w, wxCoord h, double minx, double maxx, double miny, double maxy, AXISDIR axis_dir);

    /** Draw the B-spline on the dc.
     * 
     * @param dc The DC to draw on.
     * @param x  The X coord of the start point on the DC.
     * @param y  The Y coord of the start point on the DC.
     * @param w  The width of the area on the DC.
     * @param h  The height of the area on the DC.
     * @param minx  The min X coord of the curve.
     * @param maxx  The max X coord of the curve.
     * @param minu  The min Y coord of the curve.
     * @param maxx  The max Y coord of the curve.
     * @param axis_dir The direction of axises.
     */
    void DrawBSpline(wxDC& dc, wxCoord x, wxCoord y, wxCoord w, wxCoord h, double minx, double maxx, double miny, double maxy, AXISDIR axis_dir);

    /** Draw the polyline on the dc.
     * 
     * @param dc The DC to draw on.
     * @param x  The X coord of the start point on the DC.
     * @param y  The Y coord of the start point on the DC.
     * @param w  The width of the area on the DC.
     * @param h  The height of the area on the DC.
     * @param minx  The min X coord of the curve.
     * @param maxx  The max X coord of the curve.
     * @param minu  The min Y coord of the curve.
     * @param maxx  The max Y coord of the curve.
     * @param axis_dir The direction of axises.
     */
    void DrawPolyline(wxDC& dc, wxCoord x, wxCoord y, wxCoord w, wxCoord h, double minx, double maxx, double miny, double maxy, AXISDIR axis_dir);

    /** Draw the points on the dc.
     * 
     * @param dc The DC to draw on.
     * @param x  The X coord of the start point on the DC.
     * @param y  The Y coord of the start point on the DC.
     * @param w  The width of the area on the DC.
     * @param h  The height of the area on the DC.
     * @param minx  The min X coord of the curve.
     * @param maxx  The max X coord of the curve.
     * @param minu  The min Y coord of the curve.
     * @param maxx  The max Y coord of the curve.
     * @param axis_dir The direction of axises.
     */
    void DrawPoints(wxDC& dc, wxCoord x, wxCoord y, wxCoord w, wxCoord h, double minx, double maxx, double miny, double maxy, AXISDIR axis_dir);

    void DrawText(wxDC& dc, const wxPoint * list, int s);

    wxFont GetFont() const { return fnt; }
    void   SetFont(const wxFont & f) { fnt = f; }

    /// Get the pen to draw the curve.
    wxPen  GetPen() const { return pen; }

    /// Set the pen to draw the curve.
    void   SetPen(const wxPen & p) { pen = p; }

    /// Get the type of the curve, B-spline or polyline.
    CURVE_TYPE GetType() const { return type; }

    /// Set the type of the curve, B-spline or polyline.
    void       SetType(const CURVE_TYPE & t) { type = t; }

    /// Get the symbol type to draw on the curve.
    CURVE_SYMBOL GetSymbol() const { return symbol; }

    /// Set the symbol type to draw on the curve.
    void       SetSymbol(const CURVE_SYMBOL & s) { symbol = s; }

    /// Is the symbol drawed on the curve filled or not.
    bool       IsSymbolFilled() const { return is_symbol_filled;}

    /// Set whether the symbol drawed on the curve filled or not.
    void       SetSymbolFilled(bool b = true) { is_symbol_filled = b;}
    
    /// Get the text drawed on the curve
    wxString GetText() const { return text; }
    
    /// Set the text drawed on the curve
    void     SetText(const wxString & t) { text = t; }

    /// Get the text drawed on the curve
    wxString GetLabel() const { return label; }
    
    /// Set the text drawed on the curve
    void     SetLabel(const wxString & t) { label = t; }

    CURVE_TEXT GetTextPos() const { return text_pos; }
    void     SetTextPos(const CURVE_TEXT & t) { text_pos = t; }

    CURVE_LABEL GetLabelPos() const { return label_pos; }
    void     SetLabelPos(const CURVE_LABEL & t) { label_pos = t; }

    double GetTextValue() const { return text_value; }

    wxString GetWholeText() const;

    /// Set Text pos, in the range of (0,1)
    void     SetTextValue(const double & t) { text_value = t; }

    int GetLegendPos() const { return legend_pos; }

    void SetLegendPos(const int & t) { legend_pos = t; }

    bool IsHide() const { return isHide; }

    void IsHide(const bool & t) { isHide = t; }

    /// Load the info from XML file.
    void Load(Element * element);

    /// Export the info from XML file.
    void Export(wxTextOutputStream & tstream, const wxString & prefix, const unsigned & n, bool reverse=false) const;

    bool operator != (const Curve & c) const;

    /** Init the symbol points.
     * 
     * @param size The size of symbol in points.
     */
    static void init_points(wxCoord size=3);

    /** Draw the symbol on the DC.
     * 
     * @param dc The DC to draw on.
     * @param x  The X coord of the start point on the DC.
     * @param y  The Y coord of the start point on the DC.
     * @param symbol The type of symbol.
     * @param filled Whether filled or not.
     */
    static void draw_symbol(wxDC &dc, wxCoord x, wxCoord y, CURVE_SYMBOL symbol, bool filled);
  protected :
    static wxPoint points[8][7];
    static bool inited;
    wxFont fnt;
    wxPen  pen;
    CURVE_TYPE    type;
    CURVE_SYMBOL  symbol;
    CURVE_TEXT text_pos;
    CURVE_LABEL label_pos;
    double     text_value;
    int        legend_pos;
    wxString text;
    wxString label;
    bool is_symbol_filled;
    bool isHide;
};


/** Manager for windows and draw the grid.
  * Calculate the positions for the windows.
  */

class PosMgr
{
public :
  /// Default Constructor
  PosMgr();

  /// Load info from XML file.
  bool Load(Element * element);

  /** Save graph from file.
   * @param filename The XML file of the graph.
   */
  void Export(wxTextOutputStream & tstream, const wxString & prefix) const;

  /** Get the area to draw based on.
   *
   * @param r The number in the row, start from 0.
   * @param c The number in the col, start from 0.
   * @param dc The DC to draw on.
   * @param x  The X coord of the start point on the DC.
   * @param y  The Y coord of the start point on the DC.
   * @param w  The width of the area on the DC.
   * @param h  The height of the area on the DC.
   */
  void GetPos(const size_t &r, const size_t &c, wxCoord &x, wxCoord &y, size_t &w, size_t &h, const unsigned int &no=0u, wxDC* dc=0u);

  /// Set the X pos of the start point on the 'paper'
  void SetX(const wxCoord &x);

  /// Get the X pos of the start point on the 'paper'
  wxCoord GetX() const { return pos_x;}

  /// Set the Y pos of the start point on the 'paper'
  void SetY(const wxCoord &y);

  /// Get the Y pos of the start point on the 'paper'
  wxCoord GetY() const { return pos_y;}

  /// Set the width of the 'paper'
  void SetW(const size_t &w);

  /// Get the width of the 'paper'
  size_t GetW() const { return size_w;}

  /// Set the height of the 'paper'
  void SetH(const size_t &h);

  /// Get the height of the 'paper'
  size_t GetH() const { return size_h;}

  /// Expire the cache.
  void ExpireCache(void);

  /** Draw the grid on the 'paper'.
   *
   * @param dc The DC to draw the grid.
   */
  void DrawGrid(wxDC& dc);

  /// Whether show the grid.
  bool   IsShowGrid() const { return is_show_grid; }

  /** Show the grid.
   * 
   * @param b Setting true to show the grid
   */
  void   ShowGrid(bool b=true) { is_show_grid = b; }

  /// Get the pen to draw grid.
  wxPen  GetGridPen() const { return pen_grid; }

  /// Set the pen to draw grid.
  void   SetGridPen(const wxPen & p) { pen_grid = p; }

  /// Get the pen to draw sub grid.
  wxPen  GetSubGridPen() const { return pen_sub_grid; }

  /// Set the pen to draw sub grid.
  void   SetSubGridPen(const wxPen & p) { pen_sub_grid = p; }

  /// Get the width of each grid.
  unsigned GetGridWidth() const { return grid_width; }
  unsigned GetGridW() const { return width; }

  /// Get the height of each grid.
  unsigned GetGridHeight() const { return grid_height; }
  unsigned GetGridH() const { return height; }

  /// Get how many pic to div the width in x dir.
  unsigned GetGridX() const { return grid_x; }
  
  /// Get how many pic to div the width in x dir.
  void   SetGridX(const unsigned & s) { grid_x = s; ExpireCache(); } 
  
  /// Get how many pic to div the height in y dir.
  unsigned GetGridY() const { return grid_y; }
  
  /// Get how many pic to div the height in y dir.
  void   SetGridY(const unsigned & s) { grid_y = s; ExpireCache(); } 

  /// Get how many pic to div the grid in x dir.
  unsigned GetSubGridX() const { return sub_grid_x; }
  
  /// Get how many pic to div the grid in x dir.
  void   SetSubGridX(const unsigned & s) { sub_grid_x = s; } 
  
  /// Get how many pic to div the grid in y dir.
  unsigned GetSubGridY() const { return sub_grid_y; }
  
  /// Get how many pic to div the grid in y dir.
  void   SetSubGridY(const unsigned & s) { sub_grid_y = s; } 

  /// div info
  struct PosMap{
    vector<wxUint32> row_list;
    vector<wxUint32> col_list;
    bool cached;
    vector<wxUint32> row_pos;
    vector<wxUint32> col_pos;  
    vector<wxUint32> row_size;
    vector<wxUint32> col_size;
  };

  /// Get the Div.
  vector<PosMap> & GetDiv() {return div_list;}

protected :
  vector<PosMap> div_list;
  void cal(wxDC* dc, const unsigned int &no=0u);
  wxCoord pos_x;
  wxCoord pos_y;
  size_t size_h;
  size_t size_w;

  /*
   * grid setting
   */
  bool is_show_grid;
  unsigned int grid_width;
  unsigned int grid_height;
  unsigned int sub_grid_x;
  unsigned int sub_grid_y;
  unsigned int grid_x;
  unsigned int grid_y;
  wxCoord width;
  wxCoord height;
  wxPen  pen_grid;
  wxPen  pen_sub_grid;
};


class PlotHandler;
class PlotWindow;

struct Focus{
  PlotWindow * window;
  enum Type {
    WINDOW = 0x01,
    LEGEND = 0x02,
    AXISX  = 0x04,
    AXISY  = 0x08,
    CURVE  = 0x10,
    TITLE  = 0x20,
    GRID   = 0x40,
  } type;
  unsigned iter;
};

typedef vector<Focus> FocusList;
typedef FocusList::const_iterator FocusListCIter;
typedef FocusList::iterator FocusListIter;

/** The window to draw the curves.
  * Draw curves in the window.
  */

class PlotWindow
{
  public :
    /** Constructor.
     * Init itself and get connect with plothandler
     *
     * @param h The parent.
     */
    PlotWindow(PlotHandler * h);

    /** Constructor.
     * Init itself and get connect with plothandler.
     * Then loaded based on the XML file
     *
     * @param h The parent.
     * @param element The info how to load.
     * @see Load
     */
    PlotWindow(PlotHandler * h, Element * element);

    /// Destructor.
    ~PlotWindow();

    /// Get the pos in row.
    wxCoord GetRowPos() const { return row_pos;}

    /// Set the pos in row.
    void    SetRowPos(wxCoord p) { row_pos = p; }

    /// Get the pos in col.
    wxCoord GetColPos() const { return col_pos;}

    /// Set the pos in col.
    void    SetColPos(wxCoord p) { col_pos = p; }
    
    /// Get the pos in col.
    unsigned GetMgrPos() const { return mgr_pos;}

    /// Set the pos in col.
    void    SetMgrPos(unsigned p) { mgr_pos = p; }
    
    /// Load from the XML file after being constructed.
    bool Load(Element * element);

    /** Save graph from file.
      * @param filename The XML file of the graph.
      */
    void Export(wxTextOutputStream & tstream, const wxString & prefix) const;

    /** Draw the plot window.
     *
     * @param dc The DC to be drawed on.
     * @param pos_mgr The positon manager to support pos info.
     */
    void Draw(wxDC& dc, PosMgr& pos_mgr);

    /** Draw the indicator
     *
     * @param dc The DC to be drawed on.
     * @param pos_mgr The positon manager to support pos info.
     * @param xpos The x pos of the mouse.
     * @param ypos The y pos of the mouse.
     */
    void DrawIndicator(wxDC& dc, PosMgr& pos_mgr, wxCoord &xpos, wxCoord &ypos);

    /** Draw the focus zone
     *
     * @param dc The DC to be drawed on.
     * @param pos_mgr The positon manager to support pos info.
     * @param xpos The x pos of the mouse.
     * @param ypos The y pos of the mouse.
     */
    void DrawFocus(wxDC& dc, PosMgr& pos_mgr, wxCoord &xpos, wxCoord &ypos, int mask=-1);

    /** Set the position of the legend
     *
     * @param pos_mgr The positon manager to support pos info.
     * @param xpos The x pos of the mouse.
     * @param ypos The y pos of the mouse.
     */
    bool SetLegendPos(PosMgr& pos_mgr, wxCoord &xpos, wxCoord &ypos, const int &no=0);

    /** Set the position from curves to the legend
     *
     * @param pos_mgr The positon manager to support pos info.
     * @param xpos The x pos of the mouse.
     * @param ypos The y pos of the mouse.
     */
    bool SetLegendVal(PosMgr& pos_mgr, wxCoord &xpos, wxCoord &ypos, const int &no=0);

    /** Select a curve by the mouse.
     *
     * @param pos_mgr The positon manager to support pos info.
     * @param xpos The x pos of the mouse.
     * @param ypos The y pos of the mouse.
     */
    bool SelectCurrentCurve(PosMgr& pos_mgr, const wxCoord &xpos, const wxCoord &ypos);

    /* Curve Management. */

    /// Get the current curve.
    Curve * GetCurrentCurve();

    /// Pop out the current curve.
    Curve * PopCurrentCurve();

    /// push back a curve in the vector
    void push_back(Curve * c);

    /// pop back the curve in the vector
    void pop_back();

    /// Whether the vector of the curves is empty or not.
    bool empty() const;

    /// Get the i-th curve in the vector.
    Curve * at(size_t i);

    /// Erase the i-th curve in the vector.
    void erase(size_t i);

    /// Pop out the i-th curve in the vector.
    Curve * pop(size_t i);

    /// Clear all the curves in the vector.
    void clear();

    /*the Children Management.*/

    /// Whether has children.
    bool hasChildren() const {return !children.empty();}

    /// Get the num of childern.
    size_t nChildren() const {return children.size();}

    /// push back a child in the vector
    void PushBackChild(PlotWindow *c) {children.push_back(c);}

    /// pop back the child in the vector
    void PopBackChild();
    
    /// Get the i-th child in the vector.
    PlotWindow *GetChild(size_t i);

    /// Pop out the i-th child in the vector.
    PlotWindow *PopChild(size_t i);

    /// Erase the i-th child in the vector.
    void EraseChild(size_t i) { delete PopChild(i); }

    /// Clear all the children in the vector.
    void ClearChildren();

    //asociate with window axis(x-> y\|/)
    enum AXISPOS{
      LOW_OUT,
      LOW_IN,
      HIGH_IN,
      HIGH_OUT
    };
    
    /// Get the font to draw the axises.
    wxFont GetAxisFont() const { return fnt_axis; }
    
    /// Set the font to draw the axises.
    void   SetAxisFont(const wxFont & f) { fnt_axis = f; }
    
    /// Get the pen to draw the axises.
    wxPen  GetAxisPen() const { return pen_axis; }
    
    /// Set the pen to draw the axises.
    void   SetAxisPen(const wxPen & p) { pen_axis = p; }
   
    /// Whether show the legend.
    bool   IsShowLegend(const int & no=0) const { return legend_list[no].show; }
   
    /// Set whether show the legend.
    void   ShowLegend(bool b=true, const int & no=0) { legend_list[no].show = b; }
    
    /// Get the font to draw the legend.
    wxFont GetLegendFont(const int & no=0) const { return legend_list[no].fnt; }
    
    /// Set the font to draw the legend.
    void   SetLegendFont(const wxFont & f, const int & no=0) { legend_list[no].fnt = f; }
    
    /// Get the pen to draw the legend.
    wxPen  GetLegendPen(const int & no=0) const { return legend_list[no].pen; }
    
    /// Set the pen to draw the legend.
    void   SetLegendPen(const wxPen & p, const int & no=0) { legend_list[no].pen = p; }
    
    /// Get the X pos of the legend.
    double GetLegendPosX(const int & no=0) const { return legend_list[no].pos_x; }
    
    /// Set the X pos of the legend in the range of [0,1].
    void   SetLegendPosX(const double & x, const int & no=0) { legend_list[no].pos_x = x; }
    
    /// Get the Y pos of the legend.
    double GetLegendPosY(const int & no=0) const { return legend_list[no].pos_y; }
    
    /// Set the Y pos of the legend in the range of [0,1].
    void   SetLegendPosY(const double & y, const int & no=0) { legend_list[no].pos_y = y; }
    
    /// Set the pos of the legend lines out.
    double GetLegendValueX(const int & no=0) const { return legend_list[no].value_x; }
    
    /// Set the pos of the legend lines out in the range of [0,1].
    void   SetLegendValueX(const double & x, const int & no=0) { legend_list[no].value_x = x; }

    /* Axis info */
    
    /// Get the axis direction.
    AXISDIR GetAxisDir() const { return axis_dir;}

    /// Set the axis direction.
    void    SetAxisDir(AXISDIR dir) { axis_dir = dir; }

    /// Whether show the X axis.
    bool    IsHideAxisX() const { return axis_x_hide; }

    /// Set whether show the X axis.
    void    HideAxisX(bool h=true) { axis_x_hide = h; }

    /// Get the X axis pos.
    AXISPOS GetAxisXPos() const { return axis_x_pos;}

    /// Set the X axis pos.
    void    SetAxisXPos(AXISPOS pos) { axis_x_pos = pos; }

    /// Whether show the Y axis.
    bool    IsHideAxisY() const { return axis_y_hide; }

    /// Set hether show the Y axis.
    void    HideAxisY(bool h=true) { axis_y_hide = h; }

    /// Get the Y axis pos.
    AXISPOS GetAxisYPos() const { return axis_y_pos;}

    /// Set the Y axis pos.
    void    SetAxisYPos(AXISPOS pos) { axis_y_pos = pos; }

    /* X Axis range and mark info */
    
    /// Whether the X axis is auto range.
    bool   IsAutoXSpace() const { return auto_x_space; }
    
    /// Set Whether the X axis is auto ranged.
    void   AutoXSpace(bool b=true) { auto_x_space = b; }
    
    /// Get the max X value.
    double GetMaxX() const { return max_x; }
    
    /// Set the max X value, if not auto-range.
    void   SetMaxX(const double & max) { max_cx = max; }
    
    /// Get the min X value
    double GetMinX() const { return min_x; }
    
    /// Set the min X value, if not auto-range.
    void   SetMinX(const double & min) { min_cx = min; }
    
    /// Get the X axis mark mode  0: auto; 1:base on interval value; 2: base on list
    int    GetXMarkMode() const { return x_mark_mode; }
    
    /// Set the X axis mark mode
    bool   SetXMarkMode(const int & i)
    { if (i>=0&&i<=2)
      { 
        x_mark_mode = i;
        return true;
      }
      else
      {
        return false;
      }
    }
    
    /// Get the X axis mark Interval
    double GetXMarkInterval() const { return x_mark_interval; }
    
    /// Set the X axis mark Interval
    bool   SetXMarkInterval(const double & t)
    {
      if (t > 0)
      {
        x_mark_interval = t;
        return true;
      }
      else
      {
        return false;
      }
    }

    int GetXInfoFrom(void) const {return x_info_from;}
    
    /// Get the X axis mark list
    vector< double > GetXMarkList() const { return x_mark_list; }
    
    /// Set the X axis mark list
    void SetXMarkList(const vector< double > l) {x_mark_list = l;}
    
    /// Get the X axis label
    wxString GetXLabel() const { return str_x_label; }
    
    /// Set the X axis label
    void     SetXLabel(const wxString & s) { str_x_label = s; }

    /* Y Axis range and mark info */
    
    /// Whether the Y axis is auto range.
    bool   IsAutoYSpace() const { return auto_y_space; }
    
    /// Set Whether the Y axis is auto ranged.
    void   AutoYSpace(bool b=true) { auto_y_space = b; }
    
    /// Get the max Y value.
    double GetMaxY() const { return max_y; }
    
    /// Set the max Y value, if not auto-range.
    void   SetMaxY(const double & max) { max_cy = max; }
    
    /// Get the min Y value.
    double GetMinY() const { return min_y; }
    
    /// Set the min Y value, if not auto-range.
    void   SetMinY(const double & min) { min_cy = min; }
    
    /// Get the Y axis mark mode  0: auto; 1:base on interval value; 2: base on list
    int    GetYMarkMode() const { return y_mark_mode; }
    
    /// Set the Y axis mark mode
    bool   SetYMarkMode(const int & i)
    { if (i>=0&&i<=2)
      { 
        y_mark_mode = i;
        return true;
      }
      else
      {
        return false;
      }
    }
    
    /// Get the Y axis mark Interval
    double GetYMarkInterval() const { return y_mark_interval; }
    
    /// Set the Y axis mark Interval
    bool   SetYMarkInterval(const double & t)
    {
      if (t > 0)
      {
        y_mark_interval = t;
        return true;
      }
      else
      {
        return false;
      }
    }
    
    /// Get the Y axis mark list
    vector< double > GetYMarkList() const { return y_mark_list; }
    
    /// Set the Y axis mark list
    void SetYMarkList(const vector< double > l) {y_mark_list = l;}
    
    /// Get the Y axis label
    wxString GetYLabel() const { return str_y_label; }
    
    /// Set the Y axis label
    void     SetYLabel(const wxString & s) { str_y_label = s; }

    void  GetAxisLabelExtent(wxDC &dc, wxCoord &lw, wxCoord &lh);

    struct Legend{
      bool show;
      double value_x;
      double pos_x;
      double pos_y;
      wxFont fnt;
      wxPen  pen;
      bool align_left;
      bool indicate_left;
      wxRect rect;
    };
    typedef vector<Legend> LegendList;
    typedef LegendList::iterator LegendListIter;
    typedef LegendList::const_iterator LegendListCIter;

    LegendList & GetLegendList() {return legend_list;}

  protected :
    /*
     * get the boundary
     */
    double getMaxY();
    double getMinY();
    double getMaxX();
    double getMinX();

    wxCoord row_pos;
    wxCoord col_pos;
    unsigned mgr_pos;
    AXISDIR axis_dir;

    wxFont fnt_axis;
    wxPen  pen_axis;
    
    /*
     * current active curve 
     */
    Curve * cur_curve;
    vector<Curve *> curve_list;
    bool is_combine_curves;
    double combine_curves_distance;
    
    struct CurvesTag{
      unsigned ns;
      FGPropertyManager * property;
      vector <FGFunction *> func_list;
      Condition * data_condition;
      wxString data_full_name;
      wxString table_type;
      unsigned ne;
      wxString xcr; // 2D
      wxString base_name; //3D
      int from;
      bool nofilter;
      unsigned dest_num;
      wxString coord; //4D
      bool strange;
    };
    typedef vector<CurvesTag> CurvesTagList;
    typedef CurvesTagList::const_iterator CurvesTagListCIter;
    typedef CurvesTagList::iterator CurvesTagListIter;

    CurvesTagList curve_tag_list;

    double distance(Curve * c1, Curve * c2);
    bool min_distance(Curve *, const double &);

    /*
     * x axis setting
     */
    AXISPOS axis_x_pos;
    bool axis_x_hide;
    bool auto_x_space;
    double max_x;
    double max_cx;
    double min_x;
    double min_cx;
    int    x_mark_mode; // 0: auto; 1:base on interval value; 2: base on list
    double x_mark_interval;
    int x_info_from;
    vector< double > x_mark_list;
    wxString str_x_label;
    wxRect rect_axis_x;

    /*
     * y axis setting
     */
    AXISPOS axis_y_pos;
    bool axis_y_hide;
    bool auto_y_space;
    double max_y;
    double max_cy;
    double min_y;
    double min_cy;
    int    y_mark_mode; // 0: auto; 1:base on interval value; 2: base on list
    double y_mark_interval;
    vector< double > y_mark_list;
    wxString str_y_label;
    wxRect rect_axis_y;

    /*
     * legend setting
     */
    LegendList legend_list;

    vector<PlotWindow *> children;
  
    PlotHandler * handler;
    
    bool strange_mode;
    double x_pos_strange_mode;
    double x_base_strange_mode;

    void Draw(wxDC& dc, const wxCoord &x, const wxCoord &y, const size_t &w, const size_t &h, const unsigned & gw, const unsigned & gh);
    bool GetIndicatePos(const wxCoord &x, const wxCoord &y, const size_t &w, const size_t &h, wxCoord &xpos, wxCoord &ypos, double &xvalue, double &yvalue, Curve *curve);
    void DrawIndicator(wxDC& dc, const wxCoord &x, const wxCoord &y, const size_t &w, const size_t &h, const wxCoord &xpos, const wxCoord &ypos, const double &xvalue, const double &yvalue);
    void DrawFocus(wxDC& dc, const wxCoord &x, const wxCoord &y, const size_t &w, const size_t &h, const wxCoord &xpos, const wxCoord &ypos, int mask=-1);
    bool SelectCurrentCurve(const wxCoord &x, const wxCoord &y, const size_t &w, const size_t &h, const wxCoord &xpos, const wxCoord &ypos);
    void draw_y_axis(wxDC& dc, const wxCoord &x, const wxCoord &y, const size_t &w, const size_t &h);
    void draw_x_axis(wxDC& dc, const wxCoord &x, const wxCoord &y, const size_t &w, const size_t &h);
    void draw_curve(wxDC& dc, const wxCoord &x, const wxCoord &y, const size_t &w, const size_t &h);
    void draw_legend(wxDC& dc, const wxCoord &x, const wxCoord &y, const size_t &w, const size_t &h, LegendListIter & iter);
    static wxRect draw_vertical_axis(wxDC& dc, const wxCoord &x, const wxCoord &y, const wxCoord &yh, const double &min, const double &max, vector<double> &mark_list, const wxString & text, const bool &right);
    static wxRect draw_horizon_axis(wxDC& dc, const wxCoord &x, const wxCoord &y, const wxCoord &xh, const double &min, const double &max, vector<double> &mark_list, const wxString & text, const bool &bottom);
    static void get_interval(double & min, double & max, unsigned int num, double & mark_interval, vector< double > & mark_list);
    static bool pitch_col(vector<double> &x, vector<double> &y, double &key,  vector <unsigned> & list, Table * table, Condition *data_condition, FGPropertyManager *property);
};

class PlotCanvas;

/** The handler to manage all the windows.
  * Load and draw Nomo-digrames.
  */

class PlotHandler: public wxEvtHandler
{
  public:
    /** Constructor
     *
     * @param parent The widget connected to.
     */
    PlotHandler(wxWindow * parent);

    /// Destructor
    ~PlotHandler();

    /** 
     * draw all on the dc .
     * it will draw the buff first
     */
    void Draw(wxDC& dc, wxCoord w, wxCoord h);
    
    /** 
     * draw all on the dc(wxMemoryDC, wxPaintDC or wxPrinterDC).
     */
    void draw(wxDC& dc, wxCoord w, wxCoord h);
    
    /** 
     *  it will draw the indicate .
     */
    void DrawIndicator(wxDC &dc, wxCoord x, wxCoord y);
    
    /** 
     *  it will draw the focus zone .
     */
    void DrawFocus(wxDC &dc, wxCoord x, wxCoord y, int mask=-1);
    
    /** 
     *  it will reset the position of the legend.
     */
    bool SetLegendPos(wxCoord x, wxCoord y, const int & no=0);
    
    /** 
     *  it will reset the position from curve to the legend.
     */
    bool SetLegendVal(wxCoord x, wxCoord y, const int &no=0);
    
    /** 
     *  it will select the new current curve.
     */
    bool SelectCurrentCurve(wxCoord x, wxCoord y);
    
    /**
     * event handle
     */
    void OnOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnPrint(wxCommandEvent& event);
    void OnPrintPreview(wxCommandEvent& event);
    void OnPageSetup(wxCommandEvent& event);
    void OnSetDrawIndicator(wxCommandEvent& event);
    void OnSetWYSWYG(wxCommandEvent& event);
    void OnExitToolOrMenuCommand(wxCommandEvent& event);
    void OnAboutToolOrMenuCommand(wxCommandEvent& WXUNUSED(event));

    enum TITLE_POS{
      TITLE_NONE,
      TITLE_UP,
      TITLE_DOWN
    };

    TITLE_POS GetTitlePos() const { return pos_title; }
    void      SetTitlePos(const TITLE_POS & p) { pos_title = p; }
    wxFont GetTitleFont() const { return fnt_title; }
    void   SetTitleFont(const wxFont & f) { fnt_title = f; }
    wxPen  GetTitlePen() const { return pen_title; }
    void   SetTitlePen(const wxPen & p) { pen_title = p; }
    wxString GetTitleStr() const { return str_title; }
    void     SetTitleStr(const wxString & s) { str_title = s; }

    Table * GetTable(const wxString & str)
    {
        map<wxString, Table>::iterator i = table_map.find(str);
        if (i == table_map.end())
        {
          return NULL;
        }
        else
        {
          return &((*i).second);
        }
    }

    /** Load graph from file.
      * @param filename The XML file of the graph.
      */
    bool Load(const wxString & filename);

    /** Save graph from file.
      * @param filename The XML file of the graph.
      */
    bool Save(const wxString & filename=wxEmptyString);

    /** Save graph from file.
      * @param filename The XML file of the graph.
      */
    void Export(wxTextOutputStream & tstream, const wxString & prefix) const;

    bool IsDrawIndicator(void) const {return isDrawIndicator;}
    void DrawIndicator(bool b=true) 
    {
        isDrawIndicator = b;
        owner->Refresh();
        owner->Update();
    }

    /// Whether is 'What you see is what you get.'
    bool IsWYSWYG(void) const {return WYSWYG;}
    void SetWYSWYG(bool b=true); 

    wxString GetFileName() {return FileName;}

    struct IndecatorPos{
      wxCoord x;
      wxCoord y;
    };

    void AddIndicator(wxCoord &xx, wxCoord &yy)
    {IndecatorPos t; t.x=xx; t.y=yy; indicator_list.push_back(t);}
    void ClearIndicators(void){indicator_list.clear();}
    
    PlotWindow * GetWindow(size_t i) {return windows_list[i];}

    /**
     * Get the reference of the Pos Manager
     */
    PosMgr & GetPosMgr() {return pos_mgr;}

    void AddFocus(const Focus & f) { focus_list.push_back(f); }
    void ClearFocus() { focus_list.clear(); }

  protected :
    
    /** 
     *Whether draw indicate or not.
     */
    bool isDrawIndicator;

    vector<IndecatorPos> indicator_list;
    
    /**
     * picture buffer.
     */
    wxBitmap bitmap;

    /**
     * owner of the handle
     */
    wxWindow * owner;

    /**
     * title setting
     */
    TITLE_POS pos_title;
    wxFont fnt_title;
    wxString str_title;
    wxPen  pen_title;
    wxRect rect_title;

    /**
     * plot windows
     */
    vector<PlotWindow *> windows_list;

    map<wxString, Table> table_map;
    vector<TableList> table_list_list;
    vector<TableList> table_list_clist;

    /**
     * manage positions of windows
     */
    PosMgr pos_mgr;
    
    /// Global print data, to remember settings during the session
    wxPrintData printData;

    /// Global page setup data
    wxPageSetupDialogData pageSetupData;

    /// viewer pointer
    PlotCanvas * canvas;

    /// whether WYSWYG
    bool WYSWYG;
    
    friend class PlotCanvas;

    wxString FileName;
    bool hasLoaded;

    bool Load(Element * element);

    void get_paper_size(int &w, int &h);

    FocusList focus_list;

    DECLARE_EVENT_TABLE()
};

/**
 * Define a new canvas which can receive some events
 */

class PlotCanvas: public wxScrolledWindow
{
  public:
    /** Constructor.
     *
     * @param parent The parent widget.
     * @param id The widget ID.
     * @param hdl The handler connect with the Canvas
     */
    PlotCanvas(wxWindow *parent, wxWindowID id, PlotHandler *hdl, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize);

    /// Destructor
    ~PlotCanvas(void){};

    enum {
      FOCUS_START = 10000,
      FONT_START  = 10100,
      PEN_START = 10200,
      SUB_PEN_START = 10300,
    };

    // Event Handler Functions.
    void OnPaint(wxPaintEvent& event);
    void OnKey(wxKeyEvent& event);
    void OnMotion(wxMouseEvent& event);
    void OnMouseLeftDown(wxMouseEvent& event);
    void OnMouseRightDown(wxMouseEvent& event);
    void mkMenu(wxMenu &menu);
    void OnFont(wxCommandEvent& event);
    void OnPen(wxCommandEvent& event);

  protected:
    PlotHandler *handler;
    int mode;
    int mask;
    int no;
DECLARE_EVENT_TABLE()
};

/**
 * Define a new Printer
 */

class PlotPrintout: public wxPrintout
{
 public:
  PlotPrintout(PlotHandler *hdl, wxChar *title = _T("My printout")):wxPrintout(title), handler(hdl) {}
  bool OnPrintPage(int page);
  bool HasPage(int page);
  bool OnBeginDocument(int startPage, int endPage);
  void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);
 protected:
  PlotHandler *handler;
;

  void DrawPageOne(wxDC *dc);
};


/** @fn Coordinate exchange.
  */
void AxisDirMap(AXISDIR axis_dir, int &dirx, int &diry, int &posx, int &posy, int &rangex, int &rangey, bool & exchange, const wxCoord &x, const wxCoord &y, const wxCoord &w, const wxCoord &h);

/** @struct Value and Power collection.
  */
struct VP
{
  VP(double v=0, double p=1):value(v),power(p){}
  double value;
  double power;
};

/** @typedef Value-Power RB-tree.
  * key is double type and value is VP
  */
typedef map<double, VP> VPRBTree;

/** @typedef Value-Power RB-tree iterator.
  */
typedef VPRBTree::iterator VPRBTree_Ptr;

/** @fn Make a new series of points from list x & y.
  * Note that the new number of the points will be the @b num.
  * @param x the list of coord x.
  * @param y the list of coord y.
  * @param num the size for the destinate list.
  */
void make_order(vector<double> &x, vector<double> &y, size_t num);

/** @fn Colour Loader, getting info from XML file
  */
void LoadColour(Element * element, wxColour &colour);

/** @fn Pen Loader, getting info from XML file
  */
void LoadPen(Element * element, wxPen &pen);

/** @fn Font Loader, getting info from XML file
  */
void LoadFont(Element * element, wxFont &fnt);

/** @fn Painter Loader, getting info from XML file
  */
void LoadPainter(Element * element, wxPen *pen, wxFont *fnt=NULL);

/** @fn Read numbers from a string.
  */
template <class T>
void readNumsfromString(vector<T> &list, const wxString & str);


class PenSampleCtrl: public wxWindow {
public :
    PenSampleCtrl(const wxPen& pen, wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxPanelNameStr);
    void SetPen(const wxPen& pen) {m_pen = pen;}
    wxPen GetPen() const {return m_pen;}
    wxPen& GetPen() {return m_pen;}

    void OnPaint(wxPaintEvent& event);
protected :
    wxPen m_pen;

    DECLARE_EVENT_TABLE();
};

class PenSettingDialog: public wxDialog {
public:
    /// Constructor
    PenSettingDialog(wxWindow* parent, const wxPen & pen);

    wxPen GetPen() const {return pen_ctrl->GetPen();}
    void OnWidth(wxCommandEvent& event);
    void OnStyle(wxCommandEvent& event);
    void OnColour(wxCommandEvent& event);

protected:
    PenSampleCtrl* pen_ctrl;
    wxComboBox* combo_box_width;
    wxChoice* choice_style;

    DECLARE_EVENT_TABLE();
};


