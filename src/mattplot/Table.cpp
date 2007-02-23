/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 Module:       Table.cpp
 Author:       Matthew Gong
 Date started: 3/9/2006
 Purpose:      Models a lookup table

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

FUNCTIONAL DESCRIPTION
--------------------------------------------------------------------------------
Models a lookup table

HISTORY
--------------------------------------------------------------------------------
Matt  3/9/2006          Created

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include "Table.h"

#if defined ( sgi ) && !defined( __GNUC__ ) && (_COMPILER_VERSION < 740)
# include <iomanip.h>
#else
# include <iomanip>
#endif
using std::istringstream;
using JSBSim::FGPropertyManager;
namespace Matt{
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS IMPLEMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

template <class T>
void readVecfromString(vector<T> &list, const string & str)
{
  istringstream mis(str);
  list.clear();
  T tmp;
  mis >> tmp;
  while (!(!mis))
  {
    list.push_back(tmp);
    mis >> tmp;
  }
}

Table& Table::operator [](int i)
{
  if (i == -1)
  {
    i = max_range_index;
  }
  else if (i == -2)
  {
    return avg_table[0];
  }
   
  return Tables[i].table;
}

  double Table::GetValue(int i)
  {
     if (dimension ==1u) 
       return Data[i].value; 
     else if (dimension == 0u)
       return 0;
     else
       return Tables[i].table.GetValue();
  } 

  double Table::GetKey(int i) const 
  {
     if (dimension ==1u) 
       return Data[i].key; 
     else if (dimension == 0u)
       return 0;
     else
       return Tables[i].key;
  } 

TableList Table::Load(Element* el, FGPropertyManager * prop)
{
  Element * indep = el->FindElement("independentVar");
  if (! indep)
  {
    TableList rslt;
    size_t n = el->GetNumDataLines();
    if (n < 3u)
       return rslt;
    vector<string> title_list; 
    readVecfromString(title_list, el->GetDataLine());
    
    vector<string>::iterator i = title_list.begin();
    string indepVar = *i;
    string indepVarLabel;
    size_t pos;
    if ( (pos = indepVar.find_first_of('(')) == string::npos)
    {
      indepVarLabel = indepVar;
    }
    else
    {
      indepVarLabel = indepVar.substr(pos+1);
      indepVar = indepVar.substr(0, pos);
      pos = indepVarLabel.find_first_of(')');
      indepVarLabel = indepVarLabel.substr(0, pos);
    }
    FGPropertyManager *lookupProperty=NULL;
    if (prop)
        lookupProperty = prop->GetNode(indepVar);
    ++i;
    for (; i != title_list.end(); ++i)
    {
      Table tmp(prop);
      tmp.dimension = 1u;
      tmp.name = *i;
      if ( (pos = tmp.name.find_first_of('(')) == string::npos)
      {
        tmp.label = tmp.name;
      }
      else
      {
        tmp.label = tmp.name.substr(pos+1);
        tmp.name = tmp.name.substr(0, pos);
        pos = tmp.label.find_first_of(')');
        tmp.label = tmp.label.substr(0, pos);
      }
      tmp.indepVar = indepVar;
      tmp.indepVarLabel = indepVarLabel;
      tmp.lookupProperty = lookupProperty;
      if (prop)
        tmp.lookupProperty = prop->GetNode(tmp.name);
      rslt.push_back(tmp);
    }
    vector<double> value_list;
    for (size_t j = 1; j < n; ++j)
    {
      readVecfromString(value_list, el->GetDataLine(j));
      if (value_list.size() < rslt.size()+1u)
      {
        cerr << "Fatal Error number less than expected.";
	throw "Fatal Error number less than expected.";
      }
      vector<double>::iterator k = value_list.begin();
      TableListIter iter = rslt.begin();
      double key = *k;
      ++k;
      bool first = true;
      for (; k != value_list.end(); ++k, ++iter)
      {
        iter->Data.push_back(ValuePair(key, *k));
	if (first)
	{
	  first = false;
          iter->max_data = iter->min_data = *k;
	  continue;
	}
	if (*k > iter->max_data)
	{
	   iter->max_data = *k;
	}
	if (*k < iter->min_data)
	{
	   iter->min_data = *k;
	}
      }
      for (iter = rslt.begin(); iter != rslt.end(); ++iter)
      {
        iter->range_data = iter->max_data - iter->min_data;
      }
    }
    return rslt;
  }
  else
  {
    Element * subtab = el->FindElement("table");
    if (!subtab)
    {
        cerr << "Fatal Error sub tab expected.";
	throw "Fatal Error sub tab expected.";
    }
    string indepVar = indep->GetDataLine();
    string indepVarLabel;
    size_t pos;
    if ( (pos = indepVar.find_first_of('(')) == string::npos)
    {
      indepVarLabel = indepVar;
    }
    else
    {
      indepVarLabel = indepVar.substr(pos+1);
      indepVar = indepVar.substr(0, pos);
      pos = indepVarLabel.find_first_of(')');
      indepVarLabel = indepVarLabel.substr(0, pos);
    }
    FGPropertyManager *lookupProperty=NULL;
    if (prop)
        lookupProperty = prop->GetNode(indepVar);
    TableList rslt;
    double key = subtab->GetAttributeValueAsNumber("key");
    TableList sl = Load(subtab, prop);
    for (TableListIter i = sl.begin(); i != sl.end(); ++i)
    {
      Table tmp(prop);
      tmp.dimension = i->dimension +1;
      tmp.name = i->name;
      tmp.label = i->label;
      tmp.indepVar = indepVar;
      tmp.indepVarLabel = indepVarLabel;
      tmp.lookupProperty = lookupProperty;
      tmp.Tables.push_back(TablePair(key, *i));
      tmp.avg_table.push_back(*i);
      tmp.max_data = i->max_data;
      tmp.min_data = i->min_data;
      tmp.range_data = i->range_data;
      tmp.max_range_index = 0u;

      rslt.push_back(tmp);
    }
    subtab = el->FindNextElement("table");
    unsigned int tmp_ptr = 1u;
    while (subtab)
    {
      double key = subtab->GetAttributeValueAsNumber("key");
      TableList sl = Load(subtab, prop);
      if (sl.size() == rslt.size())
      {
       TableListIter k = rslt.begin();
       for (TableListIter i = sl.begin(); i != sl.end(); ++i, ++k)
       {
        k->Tables.push_back(TablePair(key, *i));
        k->avg_table.back() += *i;
	if (i->max_data > k->max_data)
	{
	  k->max_data = i->max_data;
	}
	if (i->min_data < k->min_data)
	{
	  k->min_data = i->min_data;
	}
	if (i->range_data > k->range_data)
	{
	  k->range_data = i->range_data;
	  k->max_range_index = tmp_ptr;
	}
       }
       ++tmp_ptr;
      }
      subtab = el->FindNextElement("table");
    }
    for (TableListIter i = rslt.begin(); i != rslt.end(); ++i)
    {
      i->range_data = i->max_data - i->min_data;
      i->avg_table.back() /= i->Tables.size();
    }
    return rslt;
  }
}

double Table::GetValue(DoubleList keys, bool *flag)
{
  if (dimension == 0u || keys.size() != dimension)
    return 0;
  
  double key = keys.back();
  keys.pop_back();
  if (dimension == 1u)  
  {
    return getData(key, flag);
  }
  else
  {
    return getTableData(key, keys, flag);
  }
}

double Table::getTableData(const double & key, DoubleList keys, bool * flag)
{
  double Factor, Value, Span;
  int r=lastIndex;
  int nSize = Tables.size();

  //if the key is off the end of the table, just return the
  //end-of-table value, do not extrapolate
  if( key <= Tables[0].key ) {
    lastIndex=1;
    //cout << "Key underneath table: " << key << endl;
  } else if ( key >= Tables.back().key ) {
    lastIndex=nSize-1;
    //cout << "Key over table: " << key << endl;
  }

  // the key is somewhere in the middle, search for the right breakpoint
  // assume the correct breakpoint has not changed since last frame or
  // has only changed very little

  if ( r >= nSize ) r = nSize-1;
  else if (r < 1 ) r = 1;
  if ( r > 1 && Tables[r-1].key > key ) {
    while( r > 1 && Tables[r-1].key > key ) { --r; }
  } else if ( r < nSize-1 && Tables[r].key < key ) {
    while( r < nSize-1 && Tables[r].key <= key ) { ++r; }
  }

  lastIndex=r;
  // make sure denominator below does not go to zero.

  Span = Tables[r].key - Tables[r-1].key;
  if (Span != 0.0) {
    Factor = (key - Tables[r-1].key) / Span;
  } else {
    Factor = 1.0;
  }
  if (flag)
  {
    *flag = true;
  }
  Value = Factor*Tables[r].table.GetValue(keys, flag) + (1 - Factor)*Tables[r-1].table.GetValue(keys, flag);
  if (flag && (Factor > 1.01 || Factor < -0.01))
  {
    *flag = false;
  }

  return Value;
}

double Table::getData(const double & key, bool * flag)
{
  double Factor, Value, Span;
  int r=lastIndex;
  int nSize = Data.size();

  //if the key is off the end of the table, just return the
  //end-of-table value, do not extrapolate
  if( key <= Data[0].key ) {
    lastIndex=1;
    //cout << "Key underneath table: " << key << endl;
  } else if ( key >= Data.back().key ) {
    lastIndex=nSize-1;
    //cout << "Key over table: " << key << endl;
  }

  // the key is somewhere in the middle, search for the right breakpoint
  // assume the correct breakpoint has not changed since last frame or
  // has only changed very little

  if ( r >= nSize ) r = nSize-1;
  else if (r < 1 ) r = 1;
  if ( r > 1 && Data[r-1].key > key ) {
    while( r > 1 && Data[r-1].key > key ) { --r; }
  } else if ( r < nSize-1 && Data[r].key < key ) {
    while( r < nSize-1 && Data[r].key <= key ) { ++r; }
  }

  lastIndex=r;
  // make sure denominator below does not go to zero.

  Span = Data[r].key - Data[r-1].key;
  if (Span != 0.0) {
    Factor = (key - Data[r-1].key) / Span;
  } else {
    Factor = 1.0;
  }

  if (flag)
  {
    *flag = true;
  }
  Value = Factor*(Data[r].value - Data[r-1].value) + Data[r-1].value;
  if (flag && (Factor > 1.01 || Factor < -0.01))
  {
    *flag = false;
  }

  return Value;
}


double Table::GetValue(void)
{
  if (dimension == 0u || !lookupProperty)
    return 0;
  
  double key = lookupProperty->getDoubleValue();
  if (dimension == 1u)  
    return getData(key);
  else
  {
    return getTableData(key);
  }
}

double Table::getTableData(const double & key)
{
  double Factor, Value, Span;
  int r=lastIndex;
  int nSize = Tables.size();

  //if the key is off the end of the table, just return the
  //end-of-table value, do not extrapolate
  if( key <= Tables[0].key ) {
    lastIndex=1;
    //cout << "Key underneath table: " << key << endl;
  } else if ( key >= Tables.back().key ) {
    lastIndex=nSize-1;
    //cout << "Key over table: " << key << endl;
  }

  // the key is somewhere in the middle, search for the right breakpoint
  // assume the correct breakpoint has not changed since last frame or
  // has only changed very little

  if ( r >= nSize ) r = nSize-1;
  else if (r < 1 ) r = 1;
  if ( r > 1 && Tables[r-1].key > key ) {
    while( r > 1 && Tables[r-1].key > key ) { --r; }
  } else if ( r < nSize-1 && Tables[r].key < key ) {
    while( r < nSize-1 && Tables[r].key <= key ) { ++r; }
  }

  lastIndex=r;
  // make sure denominator below does not go to zero.

  Span = Tables[r].key - Tables[r-1].key;
  if (Span != 0.0) {
    Factor = (key - Tables[r-1].key) / Span;
  } else {
    Factor = 1.0;
  }

  Value = Factor*Tables[r].table.GetValue() + (1 - Factor)*Tables[r-1].table.GetValue();

  return Value;
}


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

Table & Table::operator += (const Table & t)
{
  if (dimension == t.dimension)
  {
    if (dimension == 1u && Data.size() == t.Data.size())
    {
      bool first = true;
      ValuePairListIter i = Data.begin();
      ValuePairListCIter j = t.Data.begin();
      for (; i != Data.end(); ++i,++j)
      {
         i->value += j->value;
	 if (first)
	 {
	   first = false;
	   max_data = min_data = i->value;
	   continue;
	 }
	 if (i->value > max_data)
	   max_data = i->value;
	 if (i->value < min_data)
	   min_data = i->value;
      }
      range_data = max_data - min_data;
    }
    if (dimension > 1u && Tables.size() == t.Tables.size())
    {
      bool first = true;
      TablePairListIter i = Tables.begin();
      TablePairListCIter j = t.Tables.begin();
      for (; i != Tables.end(); ++i,++j)
      {
         i->table += j->table;
	 if (first)
	 {
	   first = false;
	   max_data = i->table.max_data;
	   min_data = i->table.min_data;
	   continue;
	 }
	 if (i->table.max_data > max_data)
	   max_data = i->table.max_data;
	 if (i->table.min_data < min_data)
	   min_data = i->table.min_data;
      }
      avg_table[0] += t.avg_table[0];      
      range_data = max_data - min_data;
    }
  }

  return *this;
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


Table & Table::operator /= (const double & d)
{
    if (dimension == 1u )
    {
      ValuePairListIter i = Data.begin();
      for (; i != Data.end(); ++i)
      {
         i->value /= d;
      }
    }
    else
    {
      bool first = true;
      TablePairListIter i = Tables.begin();
      for (; i != Tables.end(); ++i)
      {
         i->table /= d;
      }
      avg_table[0] /= d;      
    }
    range_data /= d;
    max_data /= d;
    min_data /= d;
  
  return *this;
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

} // namespace

