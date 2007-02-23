/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 Module:       FGTable.cpp
 Author:       Jon S. Berndt
 Date started: 1/9/2001
 Purpose:      Models a lookup table

 ------------- Copyright (C) 2001  Jon S. Berndt (jsb@hal-pc.org) -------------

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA  02111-1307, USA.

 Further information about the GNU Lesser General Public License can also be found on
 the world wide web at http://www.gnu.org.

FUNCTIONAL DESCRIPTION
--------------------------------------------------------------------------------
Models a lookup table

HISTORY
--------------------------------------------------------------------------------
JSB  1/9/00          Created

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include "FGTable.h"

#if defined ( sgi ) && !defined( __GNUC__ ) && (_COMPILER_VERSION < 740)
# include <iomanip.h>
#else
# include <iomanip>
#endif

using namespace std;

namespace JSBSim {

static const char *IdSrc = "$Id$";
static const char *IdHdr = ID_TABLE;

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS IMPLEMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

FGTable::FGTable(int NRows) : nRows(NRows), nCols(1), PropertyManager(0)
{
  Type = tt1D;
  colCounter = 0;
  rowCounter = 1;
  nTables = 0;

  Data = Allocate();
  Debug(0);
  lastRowIndex=lastColumnIndex=2;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

FGTable::FGTable(const FGTable& t) : PropertyManager(t.PropertyManager)
{
  Type = t.Type;
  colCounter = t.colCounter;
  rowCounter = t.rowCounter;
  tableCounter = t.tableCounter;
  nRows = t.nRows;
  nCols = t.nCols;
  nTables = t.nTables;
  dimension = t.dimension;
  internal = t.internal;
  Name = t.Name;
  lookupProperty[0] = t.lookupProperty[0];
  lookupProperty[1] = t.lookupProperty[1];
  lookupProperty[2] = t.lookupProperty[2];

  Tables = t.Tables;
  Data = Allocate();
  for (int r=0; r<=nRows; r++) {
    for (int c=0; c<=nCols; c++) {
      Data[r][c] = t.Data[r][c];
    }
  }
  lastRowIndex = t.lastRowIndex;
  lastColumnIndex = t.lastColumnIndex;
  lastTableIndex = t.lastTableIndex;
  // Matt's change start
  interpolation_level = t.interpolation_level;
  interpolation_level_property = t.interpolation_level_property;
  bs1 = t.bs1;
  bs2 = t.bs2;
  bs  = bs;
  lookupProperty[3] = t.lookupProperty[3];
  // Matt's change end
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

FGTable::FGTable(FGPropertyManager* propMan, Element* el) : PropertyManager(propMan)
{
  int i;

  stringstream buf;
  string property_string;
  string lookup_axis;
  string call_type;
  string parent_type;
  string brkpt_string;
  FGPropertyManager* node;
  Element *tableData=0;
  Element *parent_element=0;
  Element *axisElement=0;
  string operation_types = "function, product, sum, difference, quotient,"
                           "pow, abs, sin, cos, asin, acos, tan, atan, table";

  nTables = 0;

  //Matt's change start
  interpolation_level_property = PropertyManager->GetNode( "flag/interpolation_level", true );
  //Matt's change end

  // Is this an internal lookup table?

  internal = false;
  Name = el->GetAttributeValue("name"); // Allow this table to be named with a property
  call_type = el->GetAttributeValue("type");
  if (call_type == string("internal")) {
    parent_element = el->GetParent();
    parent_type = parent_element->GetName();
    if (operation_types.find(parent_type) == string::npos) {
      internal = true;
    } else {
      // internal table is a child element of a restricted type
      cerr << endl << fgred << "  An internal table cannot be nested within another type," << endl;
      cerr << "  such as a function. The 'internal' keyword is ignored." << fgdef << endl << endl;
    }
// Matt's change start
//  } else if (!call_type.empty()) {
//    cerr << endl << fgred << "  An unknown table type attribute is listed: " << call_type
//                 << ". Execution cannot continue." << fgdef << endl << endl;
//    exit(-2);
//Matt's change end
  }

  // Determine and store the lookup properties for this table unless this table
  // is part of a 3D table, in which case its independentVar property indexes will
  // be set by a call from the owning table during creation

  dimension = 0;

  axisElement = el->FindElement("independentVar");
  if (axisElement) {

    // The 'internal' attribute of the table element cannot be specified
    // at the same time that independentVars are specified.
    if (internal) {
      cerr << endl << fgred << "  This table specifies both 'internal' call type" << endl;
      cerr << "  and specific lookup properties via the 'independentVar' element." << endl;
      cerr << "  These are mutually exclusive specifications. The 'internal'" << endl;
      cerr << "  attribute will be ignored." << fgdef << endl << endl;
      internal = false;
    }

    // Matt's change start
    //for (i=0; i<3; i++) lookupProperty[i] = 0;
    for (i=0; i<4; i++) lookupProperty[i] = 0;
    // Matt's change end

    while (axisElement) {
      property_string = axisElement->GetDataLine();
      // The property string passed into GetNode() must have no spaces or tabs.
      node = PropertyManager->GetNode(property_string);

      if (node == 0) {
        cerr << "IndependenVar property, " << property_string
             << " in Table definition is not defined." << endl;
        exit(-2);
      }

      lookup_axis = axisElement->GetAttributeValue("lookup");
      if (lookup_axis == string("row")) {
        lookupProperty[eRow] = node;
      } else if (lookup_axis == string("column")) {
        lookupProperty[eColumn] = node;
      } else if (lookup_axis == string("table")) {
        lookupProperty[eTable] = node;
      // Matt's change start
      } else if (lookup_axis == string("frame")) {
        lookupProperty[eFrame] = node;
      // Matt's change end
      } else { // assumed single dimension table; row lookup
        lookupProperty[eRow] = node;
      }
      dimension++;
      axisElement = el->FindNextElement("independentVar");
    }

  } else if (internal) { // This table is an internal table

  // determine how many rows, columns, and tables in this table (dimension).

    if (el->GetNumElements("tableData") > 1) {
      dimension = 3; // this is a 3D table
    } else {
      tableData = el->FindElement("tableData");
      string test_line = tableData->GetDataLine(1);  // examine second line in table for dimension
      if (FindNumColumns(test_line) == 2) dimension = 1;    // 1D table
      else if (FindNumColumns(test_line) > 2) dimension = 2; // 2D table
      else {
        cerr << "Invalid number of columns in table" << endl;
      }
    }

  } else {
    brkpt_string = el->GetAttributeValue("breakPoint");
    if (brkpt_string.empty()) {
     // no independentVars found, and table is not marked as internal, nor is it a 3D table
      cerr << endl << fgred << "No independent variable found for table."  << fgdef << endl << endl;
      exit(-2);
    }
  }
  // end lookup property code

  if (brkpt_string.empty()) {                  // Not a 3D table "table element"
    tableData = el->FindElement("tableData");
  } else {                                     // This is a table in a 3D table
    tableData = el;
    // Matt's change start
    if (el->FindElement("tableData"))
      dimension = 3;
    else
    // Matt's change end
    dimension = 2;                             // Currently, infers 2D table
  }

  for (i=0; i<tableData->GetNumDataLines(); i++) {
    buf << tableData->GetDataLine(i) << string(" ");
  }
  switch (dimension) {
  case 1:
    nRows = tableData->GetNumDataLines();
    nCols = 1;
    Type = tt1D;
    colCounter = 0;
    rowCounter = 1;
    Data = Allocate();
    Debug(0);
    lastRowIndex = lastColumnIndex = 2;
    *this << buf;
    // Matt's change start
    Data2BSplines();
    // Matt's change end
    break;
  case 2:
    nRows = tableData->GetNumDataLines()-1;

    if (nRows >= 2) nCols = FindNumColumns(tableData->GetDataLine(0));
    else {
      cerr << endl << fgred << "Not enough rows in this table." << fgdef << endl;
      exit(-2);
    }

    Type = tt2D;
    colCounter = 1;
    rowCounter = 0;

    Data = Allocate();
    lastRowIndex = lastColumnIndex = 2;
    *this << buf;
    // Matt's change start
    Data2BSplines();
    // Matt's change end
    break;
  case 3:
    nTables = el->GetNumElements("tableData");
    nRows = nTables;
    nCols = 1;
    Type = tt3D;
    colCounter = 1;
    rowCounter = 1;
    lastRowIndex = lastColumnIndex = 2;

    Data = Allocate(); // this data array will contain the keys for the associated tables
    Tables.reserve(nTables); // necessary?
    tableData = el->FindElement("tableData");
    for (i=0; i<nTables; i++) {
      Tables.push_back(new FGTable(PropertyManager, tableData));
      Data[i+1][1] = tableData->GetAttributeValueAsNumber("breakPoint");
      Tables[i]->SetRowIndexProperty(lookupProperty[eRow]);
      Tables[i]->SetColumnIndexProperty(lookupProperty[eColumn]);
      tableData = el->FindNextElement("tableData");
    }

    Debug(0);
    break;
  // Matt's change start
  case 4:
    nTables = el->GetNumElements("tableData");
    nRows = nTables;
    nCols = 1;
    Type = tt4D;
    colCounter = 1;
    rowCounter = 1;
    lastRowIndex = lastColumnIndex = 2;

    Data = Allocate(); // this data array will contain the keys for the associated tables
    Tables.reserve( nTables ); // necessary?
    tableData = el->FindElement( "tableData" );
    for ( int i = 0; i < nTables; i++ )
    {
      Tables.push_back( new FGTable( PropertyManager, tableData ) );
      Data[i+1][1] = tableData->GetAttributeValueAsNumber( "breakPoint" );
      tableData = el->FindNextElement( "tableData" );
    }

    Debug( 0 );
    break;
// Matt's change end
  default:
    cout << "No dimension given" << endl;
    break;
  }

  //Matt's change start
  //bind();
  //Matt's change end

  if (debug_lvl & 1) Print();
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

double** FGTable::Allocate(void)
{
  Data = new double*[nRows+1];
  for (int r=0; r<=nRows; r++) {
    Data[r] = new double[nCols+1];
    for (int c=0; c<=nCols; c++) {
      Data[r][c] = 0.0;
    }
  }
  return Data;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

FGTable::~FGTable()
{
  //Matt's change start
  //if (!Name.empty() && !internal) {
  //  string tmp = PropertyManager->mkPropertyName(Name, false); // Allow upper case
  //  PropertyManager->Untie(tmp);
  //}
  //Matt's change end

  if (nTables > 0) {
    for (int i=0; i<nTables; i++) delete Tables[i];
    Tables.clear();
  }
  for (int r=0; r<=nRows; r++) delete[] Data[r];
  delete[] Data;

  Debug(1);
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int FGTable::FindNumColumns(string test_line)
{
  // determine number of data columns in table (first column is row lookup - don't count)
  int position=0;
  int nCols=0;
  while ((position = test_line.find_first_not_of(" \t", position)) != string::npos) {
    nCols++;
    position = test_line.find_first_of(" \t", position);
  }
  return nCols;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

double FGTable::GetValue(void) const
{
  double temp = 0;
  double temp2 = 0;

  //Matt's change start
  interpolation_level = interpolation_level_property->getIntValue();
  //Matt's change end
  switch (Type) {
  case tt1D:
    temp = lookupProperty[eRow]->getDoubleValue();
    temp2 = GetValue(temp);
    return temp2;
  case tt2D:
    return GetValue(lookupProperty[eRow]->getDoubleValue(),
                    lookupProperty[eColumn]->getDoubleValue());
  case tt3D:
    return GetValue(lookupProperty[eRow]->getDoubleValue(),
                    lookupProperty[eColumn]->getDoubleValue(),
                    lookupProperty[eTable]->getDoubleValue());
  // Matt's change start
  case tt4D:
    return GetValue( lookupProperty[eRow]->getDoubleValue(),
                     lookupProperty[eColumn]->getDoubleValue(),
                     lookupProperty[eTable]->getDoubleValue(),
                     lookupProperty[eFrame]->getDoubleValue() );
  // Matt's change end
  default:
    cerr << "Attempted to GetValue() for invalid/unknown table type" << endl;
    throw(string("Attempted to GetValue() for invalid/unknown table type"));
  }
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

double FGTable::GetValue(double key) const
{
  //Matt's change start
  if ( interpolation_level > 0 && bs[0].ok())  
  {
    return bs[0].evaluate2(key);
  }
  //Matt's change end

  double Factor, Value, Span;
  int r=lastRowIndex;

  //if the key is off the end of the table, just return the
  //end-of-table value, do not extrapolate
  if( key <= Data[1][0] ) {
    //Matt's change start
    r=lastRowIndex=2;
    //cout << "Key underneath table: " << key << endl;
    //return Data[1][1];
    //Matt's change end
  } else if ( key >= Data[nRows][0] ) {
    //Matt's change start
    r=lastRowIndex=nRows;
    //cout << "Key over table: " << key << endl;
    //return Data[nRows][1];
    //Matt's change end
  }

  // the key is somewhere in the middle, search for the right breakpoint
  // assume the correct breakpoint has not changed since last frame or
  // has only changed very little

  if ( r > 2 && Data[r-1][0] > key ) {
    while( Data[r-1][0] > key && r > 2) { r--; }
  //Matt's change start
  } else if ( r < nRows && Data[r][0] <= key ) {
    while( Data[r][0] <= key && r < nRows) { r++; }
  }
  //Matt's change end

  lastRowIndex=r;
  // make sure denominator below does not go to zero.

  Span = Data[r][0] - Data[r-1][0];
  if (Span != 0.0) {
    Factor = (key - Data[r-1][0]) / Span;
    // Matt's change here ; Warning!
    //if (Factor > 1.0) Factor = 1.0;
    //else if (Factor < 0.0) Factor = 0.0;
  } else {
    Factor = 1.0;
  }

  Value = Factor*(Data[r][1] - Data[r-1][1]) + Data[r-1][1];

  return Value;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

double FGTable::GetValue(double rowKey, double colKey) const
{
  //Matt's change start
  if ( nCols == 1 )
  {
    return GetValue( rowKey );
  }
  /*
  else 
  if ( interpolation_level > 1 && bs2.ok())
  {
      double rslt = bs2.evaluate( colKey, rowKey );
      if ( bs2.ReturnCode() == 0 )
      {
        return rslt;
      }
  }
  */
  //Matt's change end

  double rFactor, cFactor, col1temp, col2temp, Value;
  int r=lastRowIndex;
  int c=lastColumnIndex;

  if ( r > 2 && Data[r-1][0] > rowKey ) {
    while ( Data[r-1][0] > rowKey && r > 2) { r--; }
  } else if ( Data[r][0] < rowKey ) {
    while ( r <= nRows && Data[r][0] <= rowKey ) { r++; }
    if ( r > nRows ) r = nRows;
  }

  if ( c > 2 && Data[0][c-1] > colKey ) {
    while( Data[0][c-1] > colKey && c > 2) { c--; }
  } else if ( Data[0][c] < colKey ) {
    while( Data[0][c] <= colKey && c <= nCols) { c++; }
    if ( c > nCols ) c = nCols;
  }

  lastRowIndex=r;
  lastColumnIndex=c;

  rFactor = (rowKey - Data[r-1][0]) / (Data[r][0] - Data[r-1][0]);
  cFactor = (colKey - Data[0][c-1]) / (Data[0][c] - Data[0][c-1]);

  // Matt's change here ; Warning!
  //if (rFactor > 1.0) rFactor = 1.0;
  //else if (rFactor < 0.0) rFactor = 0.0;

  // Matt's change here ; Warning!
  //if (cFactor > 1.0) cFactor = 1.0;
  //else if (cFactor < 0.0) cFactor = 0.0;

  //Matt's change start
  if (interpolation_level > 1)
  {
    vector<double> x, y;
    x.reserve(nCols);
    y.reserve(nCols);
    for (int cc=1; cc <= nCols; ++cc)
    {
      x.push_back(Data[0][cc]);
      y.push_back(bs[cc-1].evaluate2(rowKey));
    }
    Matt::BSplineD tmp;
    tmp.setDomain(x, y);
    tmp.SetIter2(c-2);
    Value = tmp.evaluate2(colKey);
  }
  else
  {
  col1temp = rFactor*(Data[r][c-1] - Data[r-1][c-1]) + Data[r-1][c-1];
  col2temp = rFactor*(Data[r][c] - Data[r-1][c]) + Data[r-1][c];

  Value = col1temp + cFactor*(col2temp - col1temp);
  }
  //Matt's change end

  return Value;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

double FGTable::GetValue(double rowKey, double colKey, double tableKey) const
{
  double Factor, Value, Span;
  int r=lastRowIndex;

  //Matt's change start
  /*
  if ( interpolation_level > 2)
    {
      double *x = new double[nRows];
      double *y = new double[nRows];
      for (unsigned i = 0; i < nRows; ++i)
      {
        x[i] = Data[i+1][1];
        y[i] = Tables[i]->GetValue(rowKey, colKey);
      }
      BSpline1D tmp(x, nRows, y);
      delete []x;
      delete []y;
      if (tmp.ok())
      {
        double rslt = tmp.evaluate( tableKey );
        if ( tmp.ReturnCode() == 0 )
        {
          return rslt;
        }
      }
    }
    */
  //Matt's change end

  //if the key is off the end  (or before the beginning) of the table,
  // just return the boundary-table value, do not extrapolate

  if( tableKey <= Data[1][1] ) {
    lastRowIndex=2;
    //return Tables[0]->GetValue(rowKey, colKey);
  } else if ( tableKey >= Data[nRows][1] ) {
    lastRowIndex=nRows;
    //return Tables[nRows-1]->GetValue(rowKey, colKey);
  }

  // the key is somewhere in the middle, search for the right breakpoint
  // assume the correct breakpoint has not changed since last frame or
  // has only changed very little

  if ( r > 2 && Data[r-1][1] > tableKey ) {
    while( Data[r-1][1] > tableKey && r > 2) { r--; }
  } else if ( Data[r][1] < tableKey ) {
    while( Data[r][1] <= tableKey && r < nRows) { r++; }
  }

  lastRowIndex=r;
  // make sure denominator below does not go to zero.

  //Matt's change start
  if (interpolation_level > 2)
  {
    vector<double> x, y;
    for (int cc=1; cc <= nRows; ++cc)
    {
      x.reserve(nRows);
      y.reserve(nRows);
      x.push_back(Data[cc][1]);
      y.push_back(Tables[cc-1]->GetValue(rowKey, colKey));
    }
    Matt::BSplineD tmp;
    tmp.setDomain(x, y);
    tmp.SetIter2(r-2);
    Value = tmp.evaluate2(tableKey);
  }
  else
  {
  Span = Data[r][1] - Data[r-1][1];
  if (Span != 0.0) {
    Factor = (tableKey - Data[r-1][1]) / Span;
    // Matt's change here ; Warning!
    //if (Factor > 1.0) Factor = 1.0;
    //else if (Factor < 0.0) Factor = 0.0;
  } else {
    Factor = 1.0;
  }

  Value = Factor*(Tables[r-1]->GetValue(rowKey, colKey) - Tables[r-2]->GetValue(rowKey, colKey))
                              + Tables[r-2]->GetValue(rowKey, colKey);
  }
  //Matt's change end

  return Value;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

double FGTable::GetValue( double rowKey, double colKey, double tableKey, double frameKey ) const
{
  double Factor, Value, Span;
  int r = lastRowIndex;

  //Matt's change start
  /*
  if ( interpolation_level > 3)
    {
      double *x = new double[nRows];
      double *y = new double[nRows];
      for (unsigned i = 0; i < nRows; ++i)
      {
        x[i] = Data[i+1][1];
        y[i] = Tables[i]->GetValue( rowKey, colKey, tableKey );
      }
      BSpline1D tmp(x, nRows, y);
      delete []x;
      delete []y;
      if (tmp.ok())
      {
        double rslt = tmp.evaluate( frameKey );
        if ( tmp.ReturnCode() == 0 )
        {
          return rslt;
        }
      }
    }
  */
  //Matt's change end

  //if the key is off the end  (or before the beginning) of the table,
  // just return the boundary-table value, do not extrapolate

  if ( frameKey <= Data[1][1] )
  {
    r = lastRowIndex = 2;
    //return Tables[0]->GetValue(rowKey, colKey, tableKey);
  }
  else if ( frameKey >= Data[nRows][1] )
  {
    r = lastRowIndex = nRows;
    //return Tables[nRows-1]->GetValue(rowKey, colKey, tableKey);
  }

  // the key is somewhere in the middle, search for the right breakpoint
  // assume the correct breakpoint has not changed since last frame or
  // has only changed very little

  if ( r > 2 && Data[r-1][1] > frameKey )
  {
    while ( Data[r-1][1] > frameKey && r > 2 )
    {
      r--;
    }
  }
  else if ( Data[r][1] < frameKey )
  {
    //Matt's change here
    while ( r < nRows && Data[r][1] <= frameKey )
    {
      r++;
    }
  }

  lastRowIndex = r;
  // make sure denominator below does not go to zero.

  //Matt's change here
  if (interpolation_level > 3)
  {
    vector<double> x, y;
    for (int cc=1; cc <= nRows; ++cc)
    {
      x.push_back(Data[cc][1]);
      y.push_back(Tables[cc-1]->GetValue(rowKey, colKey, tableKey));
    }
    Matt::BSplineD tmp;
    tmp.setDomain(x, y);
    tmp.SetIter2(r-2);
    Value = tmp.evaluate2(frameKey);
  }
  else
  {
  Span = Data[r][1] - Data[r-1][1];
  if ( Span != 0.0 )
  {
    Factor = ( frameKey - Data[r-1][1] ) / Span;
    // Matt's change here ; Warning!
    //if (Factor > 1.0) Factor = 1.0;
    //else if (Factor < 0.0) Factor = 0.0;
  }
  else
  {
    Factor = 1.0;
  }

  Value = Factor * ( Tables[r-1]->GetValue( rowKey, colKey, tableKey ) - Tables[r-2]->GetValue( rowKey, colKey, tableKey ) )
          + Tables[r-2]->GetValue( rowKey, colKey, tableKey );
  }

  return Value;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void FGTable::operator<<(stringstream& in_stream)
{
  int startRow=0;
  int startCol=0;

  //Matt's change start
  if (Type == tt1D || Type == tt3D || Type == tt4D) startRow = 1;
  if (Type == tt3D || Type == tt4D) startCol = 1;
  //Matt's change end

  for (int r=startRow; r<=nRows; r++) {
    for (int c=startCol; c<=nCols; c++) {
      if (r != 0 || c != 0) {
        in_stream >> Data[r][c];
      }
    }
  }
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

FGTable& FGTable::operator<<(const double n)
{
  Data[rowCounter][colCounter] = n;
  if (colCounter == nCols) {
    colCounter = 0;
    rowCounter++;
  } else {
    colCounter++;
  }
  return *this;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

FGTable& FGTable::operator<<(const int n)
{
  *this << (double)n;
  return *this;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void FGTable::Print(void)
{
  int startRow=0;
  int startCol=0;

  if (Type == tt1D || Type == tt3D) startRow = 1;
  if (Type == tt3D) startCol = 1;

#if defined (sgi) && !defined(__GNUC__) && (_COMPILER_VERSION < 740)
  unsigned long flags = cout.setf(ios::fixed);
#else
  ios::fmtflags flags = cout.setf(ios::fixed); // set up output stream
#endif

  switch(Type) {
    case tt1D:
      cout << "    1 dimensional table with " << nRows << " rows." << endl;
      break;
    case tt2D:
      cout << "    2 dimensional table with " << nRows << " rows, " << nCols << " columns." << endl;
      break;
    case tt3D:
      cout << "    3 dimensional table with " << nRows << " rows, "
                                          << nCols << " columns "
                                          << nTables << " tables." << endl;
      break;
      //Matt's change start
    case tt4D:
      cout << "    4 dimensional table with " << nRows << " rows, "
                                          << nCols << " columns "
                                          << nTables << " tables." << endl;
      break;
      //Matt's change end
  }
  cout.precision(4);
  for (int r=startRow; r<=nRows; r++) {
    cout << "	";
    for (int c=startCol; c<=nCols; c++) {
      if (r == 0 && c == 0) {
        cout << "	";
      } else {
        cout << Data[r][c] << "	";
	//Matt's change here
        if (Type == tt3D || Type == tt4D) {
          cout << endl;
          Tables[r-1]->Print();
        }
      }
    }
    cout << endl;
  }
  cout.setf(flags); // reset
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void FGTable::bind(void)
{
  typedef double (FGTable::*PMF)(void) const;
  if ( !Name.empty() && !internal) {
    string tmp = PropertyManager->mkPropertyName(Name, false); // Allow upper
    PropertyManager->Tie( tmp, this, (PMF)&FGTable::GetValue);
  }
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void FGTable::Data2BSplines()
{
  if ( nCols == 1 )
  {
    double *x = new double[nRows];
    double *y = new double[nRows];
    for ( int i = 0; i < nRows; ++i )
    {
      x[i] = Data[i+1][0];
      y[i] = Data[i+1][1];
    }
    bs1.setDomain( x, nRows, y );
    delete []x;
    delete []y;
  }
  else
  {
    double *x = new double[nCols];
    double *y = new double[nRows];
    double *z = new double[nRows*nCols];
    for ( int i = 0; i < nCols; ++i )
    {
      x[i] = Data[0][i+1];
    }
    for ( int i = 0; i < nRows; ++i )
    {
      y[i] = Data[i+1][0];
    }
    int k = 0;
    for ( int i = 1; i <= nRows; ++i )
      for ( int j = 1; j <= nCols; ++j )
        z[k++] = Data[i][j];
    bs2.setDomain( x, nCols, y, nRows, z );
    delete []x;
    delete []y;
    delete []z;
  }
  {
  bs.clear();
  double *x = new double[nRows];
  for (int i=0; i<nRows; ++i)
  {
    x[i] = Data[i+1][0];
  }
  double *y = new double[nRows];
  for (int j=1; j<=nCols; ++j)
  {
    for (int i=0; i<nRows; ++i)
    {
      y[i] = Data[i+1][j];
    }
    bs.push_back(Matt::BSplineD(x, nRows, y));
    bs.back().sample();
  }
  delete []x;
  delete []y;
  }
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//    The bitmasked value choices are as follows:
//    unset: In this case (the default) JSBSim would only print
//       out the normally expected messages, essentially echoing
//       the config files as they are read. If the environment
//       variable is not set, debug_lvl is set to 1 internally
//    0: This requests JSBSim not to output any messages
//       whatsoever.
//    1: This value explicity requests the normal JSBSim
//       startup messages
//    2: This value asks for a message to be printed out when
//       a class is instantiated
//    4: When this value is set, a message is displayed when a
//       FGModel object executes its Run() method
//    8: When this value is set, various runtime state variables
//       are printed out periodically
//    16: When set various parameters are sanity checked and
//       a message is printed out when they go out of bounds

void FGTable::Debug(int from)
{
  if (debug_lvl <= 0) return;

  if (debug_lvl & 1) { // Standard console startup message output
    if (from == 0) { // Constructor

    }
  }
  if (debug_lvl & 2 ) { // Instantiation/Destruction notification
    if (from == 0) cout << "Instantiated: FGTable" << endl;
    if (from == 1) cout << "Destroyed:    FGTable" << endl;
  }
  if (debug_lvl & 4 ) { // Run() method entry print for FGModel-derived objects
  }
  if (debug_lvl & 8 ) { // Runtime state variables
  }
  if (debug_lvl & 16) { // Sanity checking
  }
  if (debug_lvl & 64) {
    if (from == 0) { // Constructor
      cout << IdSrc << endl;
      cout << IdHdr << endl;
    }
  }
}
}
