/////////////////////////////////////////////////////////////////////////////
// Name:        TreeItem.h
// Purpose:     Manager of the Tree Item
// Author:      Matthew Gong
// Created:     05/0/2005
// Copyright:   (c) Matthew Gong
// Licence:     GPL licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
// #pragma interface
#endif

# include <sstream>

#include <wx/treectrl.h>
#include <wx/txtstrm.h>
#include "FGXMLElement.h"

#ifndef TREEITEM_H
#define TREEITEM_H

class AeroDynamicsDialog;
class FunctionDialog;
class OptScriptDialog;
class TurbineDialog;
class FCSFunction;
class FCSFunctionDialog;

enum{
      TREE = 10000,
      EXPRESSION,
      ADD_FUNC,
      ADD_PROP,
      ADD_VAL,
      ADD_TABLE,
      ADD_AXIS
};

class TreeItemEvtHandler : public wxEvtHandler {
  public:
    TreeItemEvtHandler(wxTreeCtrl * t): FuncPrefix(wxT("function/func")), flag_copy_cut(0)
    {tree = t;}
    ~TreeItemEvtHandler(){}
    wxString FuncPrefix;

  protected:
    wxTreeCtrl * tree;
    int flag_copy_cut;
    wxTreeItemId id_copy_cut;
    void Copy(wxTreeItemId & from, wxTreeItemId & to);

    void OnRightClick(wxTreeEvent & event);
    void OnDoubleClick(wxTreeEvent & event);
    void OnProperties(wxCommandEvent & event);
    void OnExpression(wxCommandEvent & event);
    void OnDelete(wxCommandEvent & event);
    void OnCopy(wxCommandEvent & event);
    void OnCut(wxCommandEvent & event);
    void OnPaste(wxCommandEvent & event);
    void OnMoveUp(wxCommandEvent & event);
    void OnMoveDown(wxCommandEvent & event);
    void OnAddAxis(wxCommandEvent & event);
    void OnAddFunc(wxCommandEvent & event);
    void OnAddProp(wxCommandEvent & event);
    void OnAddVal(wxCommandEvent & event);
    void OnAddTable(wxCommandEvent & event);
    void OnShowPaste(wxUpdateUIEvent & event);
    void OnShowAddItem(wxUpdateUIEvent & event);

    DECLARE_EVENT_TABLE() 
};

class TreeItemData_base : public wxTreeItemData {
  public:
    virtual void Init(wxTreeCtrl* t){tree = t;}
    virtual void Load(JSBSim::Element * el){}
    virtual void Save(wxTextOutputStream & out, const wxString & prefix){}
    virtual void PrepareMenu(wxMenu & menu){}
    virtual int ShowDialog(void * data = NULL){}
    wxTreeCtrl* tree;
};

/*****************************************************************************

class TreeItemData_flight_control: public TreeItemData_base {
  public:
    void Init(wxTreeCtrl* t);
    void Load(JSBSim::Element * el);
    void Save(wxTextOutputStream & out, const wxString & prefix);
    void PrepareMenu(wxMenu & menu);
    int ShowDialog(void * data = NULL);
    bool check_fcs_file(const wxString & fcsfile, const wxString & diafile);

    enum Type {tUndef=0, tFCS, tAP} type;
    wxString file_name;
    wxString filename;
};
*******************************************************************************/

class TreeItemData_aerodynamics: public TreeItemData_base {
  public:
    void Init(wxTreeCtrl* t);
    void Load(JSBSim::Element * el);
    void Save(wxTextOutputStream & out, const wxString & prefix);
    void PrepareMenu(wxMenu & menu);
    int ShowDialog(void * data = NULL);

  protected:
    bool has_alphalimits;
    double alphaclmin;
    double alphaclmax;
    bool has_hysteresis_limits;
    double alphahystmin;
    double alphahystmax;

    friend class AeroDynamicsDialog;
};

class TreeItemData_ebase: public TreeItemData_base {
  public:
    virtual wxString GetExpression() {return wxT("()");}
};

class TreeItemData_function: public TreeItemData_ebase {
  public:

    enum functionType {eTopLevel=0, eProduct, eDifference, eSum, eQuotient, 
                       ePow, eAbs, eSin, eCos, eTan, eASin, eACos, eATan,
		       eDerivative, eIntegral,
                       eLT, eLE, eGE, eGT, eEQ, eNE,
                       eAND, eOR, eNOT,
                       eIFTHEN, eUndef
    };
    
    void Init(wxTreeCtrl* t);
    void Load(JSBSim::Element * el);
    void Save(wxTextOutputStream & out, const wxString & prefix);
    void PrepareMenu(wxMenu & menu);
    int ShowDialog(void * data = NULL);
    wxString GetExpression();
    functionType GetType() const {return Type;}
    static bool cmpPrior(functionType parent, functionType child);
    static int getPrior(functionType child);

  protected:
    wxString name;
    wxString description;
    functionType Type;
    wxString type;

    friend class TreeItemEvtHandler;
    friend class FunctionDialog;
    friend class OptScriptDialog;
    friend class TurbineDialog;
    friend class FCSFunction;
    friend class FCSFunctionDialog;
};

class TreeItemData_Property: public TreeItemData_ebase {
  public:
    void Init(wxTreeCtrl* t);
    void Load(JSBSim::Element * el);
    void Save(wxTextOutputStream & out, const wxString & prefix);
    void PrepareMenu(wxMenu & menu);
    int ShowDialog(void * data = NULL);
    wxString GetExpression();

    wxString value;
};

class TreeItemData_Value: public TreeItemData_ebase {
  public:
    void Init(wxTreeCtrl* t);
    void Load(JSBSim::Element * el);
    void Save(wxTextOutputStream & out, const wxString & prefix);
    void PrepareMenu(wxMenu & menu);
    wxString GetExpression();
    int ShowDialog(void * data = NULL);

    double value;
};

class Table2D{
  public:
    Table2D(const unsigned int &nRows = 1u, const unsigned int &nCols = 1u)
    {
      if (nRows < 1u)
        throw "nRows < 1u";
      if (nCols < 1u)
        throw "nCols < 1u";
      std::vector<double> tmp(nCols+1, 0.0);
      for (unsigned int i = 0u; i <= nRows; ++i)
        value.push_back(tmp);
    }
    void SetTable(const unsigned int &nRows = 1u, const unsigned int &nCols = 1u)
    {
      if (nRows < 1u)
        throw "nRows < 1u";
      if (nCols < 1u)
        throw "nCols < 1u";
      std::vector<double> tmp(nCols+1, 0.0);
      value.clear();
      for (unsigned int i = 0u; i <= nRows; ++i)
        value.push_back(tmp);
    }
    void SetNumRows(const unsigned int &nRows)
    {
      if (nRows < 1u)
        throw "nRows < 1u";
      unsigned int nCols = value[0].size()-1u;
      value.clear();
      std::vector<double> tmp(++nCols, 0.0);
      for (unsigned int i = 0u; i <= nRows; ++i)
        value.push_back(tmp);
    }
    void SetNumCols(const unsigned int &nCols)
    {
      if (nCols < 1u)
        throw "nCols < 1u";
      unsigned int nRows = value.size()-1u;
      value.clear();
      std::vector<double> tmp(nCols+1, 0.0);
      for (unsigned int i = 0u; i <= nRows; ++i)
        value.push_back(tmp);
    }
    unsigned int GetNumRows() const
    {
      return value.size()-1u;
    }
    unsigned int GetNumCols() const
    {
      return value[0].size()-1u;
    }
    double GetValue(const unsigned int &row, const unsigned int &col)
    {
      return value[row][col];
    }
    void SetValue(const unsigned int &row, const unsigned int &col, 
        const double &val)
    {
      value[row][col] = val;
    }
  protected:
    std::vector< std::vector<double> > value;
};

class Table3D{
  public:
    typedef std::map< double, Table2D > VALUE_TYPE;
    Table3D(const unsigned int &nRows = 1u, const unsigned int &nCols = 1u,
        const unsigned int &nTables = 1u)
    {
      Table2D tmp(nRows,nCols);
      for (unsigned int i = 0u; i < nTables; ++i)
      {
        value.insert(VALUE_TYPE::value_type((double)i, tmp));      
      }
    }
    Table2D * GetTable(const double &key)
    {
      VALUE_TYPE::iterator it = value.find(key);
      if (it != value.end())
        return &(it->second);
      else
        return (Table2D *)NULL;
    }
    std::vector<double> GetKeyList() const
    {
      std::vector<double> tmp;
      for (VALUE_TYPE::const_iterator iter = value.begin(); 
          iter != value.end(); ++iter)
      {
        tmp.push_back(iter->first);
      }
      return tmp;
    }
    void AddTable(const double & key, const Table2D & table)
    {
      VALUE_TYPE::iterator it = value.find(key);
      if (it != value.end())
        it->second = table;
      else
        value.insert(VALUE_TYPE::value_type(key, table));
    }
    void RemoveTable(const double & key)
    {
      VALUE_TYPE::iterator it = value.find(key);
      if (it != value.end())
        value.erase(it);
    }
    void Clear(void)
    {
      value.clear();
    }
    VALUE_TYPE & GetTable() 
    {
      return value;
    }
  protected:
      VALUE_TYPE value;
};

class Table4D{
  public:
    typedef std::map< double, Table3D > VALUE_TYPE;
    Table4D(const unsigned int &nRows = 1u, const unsigned int &nCols = 1u,
        const unsigned int &nTables = 1u, const unsigned int &nFrms = 1u)
    {
      Table3D tmp(nRows,nCols, nTables);
      for (unsigned int i = 0u; i < nTables; ++i)
      {
        value.insert(VALUE_TYPE::value_type((double)i, tmp));      
      }
    }
    Table3D * GetTable(const double &key)
    {
      VALUE_TYPE::iterator it = value.find(key);
      if (it != value.end())
        return &(it->second);
      else
        return (Table3D *)NULL;
    }
    std::vector<double> GetKeyList() const
    {
      std::vector<double> tmp;
      for (VALUE_TYPE::const_iterator iter = value.begin(); 
          iter != value.end(); ++iter)
      {
        tmp.push_back(iter->first);
      }
      return tmp;
    }
    void AddTable(const double & key, const Table3D & table)
    {
      VALUE_TYPE::iterator it = value.find(key);
      if (it != value.end())
        it->second = table;
      else
        value.insert(VALUE_TYPE::value_type(key, table));
    }
    void RemoveTable(const double & key)
    {
      VALUE_TYPE::iterator it = value.find(key);
      if (it != value.end())
        value.erase(it);
    }
    void Clear(void)
    {
      value.clear();
    }
    VALUE_TYPE & GetTable() 
    {
      return value;
    }
  protected:
      VALUE_TYPE value;
};

void T1D_in(Table2D & table, std::stringstream& buf);
void T2D_in(Table2D & table, std::stringstream& buf);
void T3D_in(Table3D & table, JSBSim::Element * el);
void T4D_in(Table4D & table, JSBSim::Element * tableDate);
    
void T1D_out(Table2D & table, wxTextOutputStream & out,
    const wxString & prefix);
void T2D_out(Table2D & table, wxTextOutputStream & out,
    const wxString & prefix);
void T3D_out(Table3D & table, wxTextOutputStream & out,
    const wxString & prefix);
void T4D_out(Table4D & table, wxTextOutputStream & out,
    const wxString & prefix);


class TreeItemData_Table: public TreeItemData_ebase {
  public:
    void Init(wxTreeCtrl* t);
    void Load(JSBSim::Element * el);
    void Save(wxTextOutputStream & out, const wxString & prefix);
    void PrepareMenu(wxMenu & menu);
    wxString GetExpression();
    int ShowDialog(void * data = NULL);
    
    enum type {tUndef, tt1D, tt2D, tt3D, tt4D};

  protected:
    wxString name;
    type Type;
    wxString independentVar_row;
    wxString independentVar_column;
    wxString independentVar_table;
    wxString independentVar_frame;
    
    Table2D value_2d;
    Table3D value_3d;
    Table4D value_4d;

    friend class TableDialog;
    friend class TreeItemEvtHandler;
};

class TreeItemData_axis: public TreeItemData_ebase {
  public:
    void Init(wxTreeCtrl* t);
    void Load(JSBSim::Element * el);
    void Save(wxTextOutputStream & out, const wxString & prefix);
    void PrepareMenu(wxMenu & menu);
    wxString GetExpression();
    int ShowDialog(void * data = NULL);

    wxString name;
    wxString unit;
};

class TreeItemData_func_root: public TreeItemData_base {
  public:
    void Init(wxTreeCtrl* t);
    void PrepareMenu(wxMenu & menu);
};

class TreeItemData_script_root: public TreeItemData_base {
  public:
    void Init(wxTreeCtrl* t);
    void PrepareMenu(wxMenu & menu);
};


#endif // TREEITEM_H
