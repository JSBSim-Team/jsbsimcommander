/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 Header:       Table.h
 Author:       Matthew Gong
 Date started: 3/9/2006

 ------ Copyright (C) 2006  Matthew Gong (matthewzhenggong@gmail.com) -------

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

HISTORY
--------------------------------------------------------------------------------
Matt 3/9/2006        Create.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
SENTRY
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#ifndef TABLE_H
#define TABLE_H

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include "input_output/FGXMLElement.h"
#include "input_output/FGPropertyManager.h"
#include <sstream>
#include <vector>

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
DEFINITIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
FORWARD DECLARATIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

using std::vector;
using std::stringstream;
using JSBSim::Element;

namespace Matt{

class Table;
struct TablePair;

typedef vector<Table> TableList;
typedef TableList::iterator TableListIter;

typedef vector<double> DoubleList;
typedef DoubleList::iterator DoubleListIter;

  struct ValuePair{
    ValuePair(const double &k, const double &v):key(k),value(v){}
    double key;
    double value;
  };
  typedef vector<ValuePair> ValuePairList;
typedef ValuePairList::iterator ValuePairListIter;
typedef ValuePairList::const_iterator ValuePairListCIter;
  
  typedef vector<TablePair> TablePairList;
typedef TablePairList::iterator TablePairListIter;
typedef TablePairList::const_iterator TablePairListCIter;

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DOCUMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/** Lookup table class.

    @author Matthew Gong
    @version $Id$
*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DECLARATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

class Table
{
public:
	
  /// Constructor
  Table(JSBSim::FGPropertyManager* prop=NULL):
	  dimension(0u), 
	  lookupProperty(NULL),
	  lastIndex(1),
	  PropertyManager(prop)
  {}
  
  /// Destructor
  //~Table();

  /** This is the very important copy constructor.
      @param table a const reference to a table.*/
  //Table(const Table& table);

  /// The constructor for tables
  static TableList Load(Element* el, JSBSim::FGPropertyManager * prop=NULL);
  
  double GetValue(DoubleList keys, bool *flag=NULL);

  double GetValue(void);


  unsigned GetDimension() const {return dimension;}

  string GetName() const {return name;}

  string GetNameLabel() const {return label;}

  size_t GetSize() const 
  {
     if (dimension ==1u) 
	  return Data.size(); 
     else if (dimension == 0u) 
	  return 0u; 
     else 
	  return Tables.size();
  }

  double GetValue(int i);
  
  double GetKey(int i) const;
  
  Table& operator [](int i);

  string GetIndepVar() const {return indepVar;}

  string GetIndepVarLabel() const {return indepVarLabel;}

  double GetMax() const;

  double GetMin() const;

  Table & operator += (const Table & t);
  Table & operator /= (const double & d);
  
private:
  unsigned dimension;
  
  string name;
  string label;
  ValuePairList Data;
  double max_data;
  double min_data;
  double range_data;
  double getData(const double & key, bool * flag=NULL);

  string indepVar;
  string indepVarLabel;
  JSBSim::FGPropertyManager *lookupProperty;

  TablePairList Tables;
  TableList avg_table;
  unsigned int max_range_index;
  double getTableData(const double & key);
  double getTableData(const double & key, DoubleList keys, bool * flag= NULL);

  unsigned lastIndex;

  JSBSim::FGPropertyManager* PropertyManager;
};

  struct TablePair{
    TablePair(const double k, const Table &v):key(k),table(v){}
    double key;
    Table table;
  };


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
} // namespace
#endif

