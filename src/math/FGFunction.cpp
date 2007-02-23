/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Module: FGFunction.cpp
Author: Jon Berndt
Date started: 8/25/2004
Purpose: Stores various parameter types for functions

 ------------- Copyright (C) 2004  Jon S. Berndt (jsb@hal-pc.org) -------------

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

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include <stdio.h>

#include "FGFunction.h"
#include "FGTable.h"
#include "FGPropertyValue.h"
#include "FGRealValue.h"

namespace JSBSim {

static const char *IdSrc = "$Id$";
static const char *IdHdr = ID_FUNCTION;

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS IMPLEMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

FGFunction::FGFunction(FGPropertyManager* propMan, Element* el, string prefix)
                                      : PropertyManager(propMan), Prefix(prefix)
{
  int i;
  Element* element;
  string operation, property_name;
  int size = el->GetNumElements();
  cached = false;
  cachedValue = -HUGE_VAL;

  Name = el->GetAttributeValue("name");
  operation = el->GetName();

  if (operation == string("function")) {
    Type = eTopLevel;
  } else if (operation == string("product")) {
    Type = eProduct;
  } else if (operation == string("difference")) {
    Type = eDifference;
  } else if (operation == string("sum")) {
    Type = eSum;
  } else if (operation == string("quotient")) {
    Type = eQuotient;
  } else if (operation == string("pow")) {
    Type = ePow;
  } else if (operation == string("abs")) {
    Type = eAbs;
  } else if (operation == string("sin")) {
    Type = eSin;
  } else if (operation == string("exp")) {
    Type = eExp;
  } else if (operation == string("cos")) {
    Type = eCos;
  } else if (operation == string("tan")) {
    Type = eTan;
  } else if (operation == string("asin")) {
    Type = eASin;
  } else if (operation == string("acos")) {
    Type = eACos;
  } else if (operation == string("atan")) {
    Type = eATan;
  } else if (operation == string("atan2")) {
    Type = eATan2;
  //Matt's change start:
  } else if (operation == string("derivative")) {
    Type = eDerivative;
    time = PropertyManager->GetNode("sim-time-sec");
  } else if (operation == string("integral")) {
    Type = eIntegral;
    time = PropertyManager->GetNode("sim-time-sec");
  } else if (operation == string("lt")) {
    Type = eLT;
  } else if (operation == string("le")) {
    Type = eLE;
  } else if (operation == string("gt")) {
    Type = eGT;
  } else if (operation == string("ge")) {
    Type = eGE;
  } else if (operation == string("eq")) {
    Type = eEQ;
  } else if (operation == string("ne")) {
    Type = eNE;
  } else if (operation == string("and")) {
    Type = eAND;
  } else if (operation == string("or")) {
    Type = eOR;
  } else if (operation == string("not")) {
    Type = eNOT;
  } else if (operation == string("ifthen")) {
    Type = eIFTHEN;
  //Matt's change end.
  } else if (operation != string("description")) {
    cerr << "Bad operation " << operation << " detected in configuration file" << endl;
  }

  element = el->GetElement();
  while (element) {
    operation = element->GetName();

    // data types
    if (operation == string("property")) {
      property_name = element->GetDataLine();
      FGPropertyManager* newNode = PropertyManager->GetNode(property_name);
      if (newNode == 0) {
        cerr << "The property " << property_name << " is undefined." << endl;
        exit(-1);
      } else {
        Parameters.push_back(new FGPropertyValue( newNode ));
      }
    } else if (operation == string("value")) {
      Parameters.push_back(new FGRealValue(element->GetDataAsNumber()));
    } else if (operation == string("table")) {
      Parameters.push_back(new FGTable(PropertyManager, element));
    // operations
    } else if (operation == string("product") ||
               operation == string("difference") ||
               operation == string("sum") ||
               operation == string("quotient") ||
               operation == string("pow") ||
               operation == string("exp") ||
               operation == string("abs") ||
               operation == string("sin") ||
               operation == string("cos") ||
               operation == string("tan") ||
               operation == string("asin") ||
               operation == string("acos") ||
               operation == string("atan") ||
	       //Matt's change start
	       operation == string("function") ||
               operation == string("derivative") ||
               operation == string("integral") ||
	       operation == string("lt") ||
	       operation == string("le") ||
	       operation == string("gt") ||
	       operation == string("ge") ||
	       operation == string("eq") ||
	       operation == string("ne") ||
	       operation == string("and") ||
	       operation == string("or") ||
	       operation == string("not") ||
	       operation == string("ifthen") ||
	       //Matt's change end.
               operation == string("atan2"))
    {
      Parameters.push_back(new FGFunction(PropertyManager, element));
    } else if (operation != string("description")) {
      cerr << "Bad operation " << operation << " detected in configuration file" << endl;
    }
    element = el->GetNextElement();
  }

  bind(); // Allow any function to save its value

  Debug(0);
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

FGFunction::~FGFunction(void)
{
  if (!Name.empty()) {
    string tmp = PropertyManager->mkPropertyName(Prefix + Name, false); // Allow upper case
    PropertyManager->Untie(tmp);
  }

  for (int i=0; i<Parameters.size(); i++) {
    delete Parameters[i];
  }
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void FGFunction::cacheValue(bool cache)
{
  cached = false; // Must set cached to false prior to calling GetValue(), else
                  // it will _never_ calculate the value;
  if (cache) {
    cachedValue = GetValue();
    cached = true;
  }
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

double FGFunction::GetValue(void) const
{
  int i;

  if (cached) return cachedValue;

  double temp = Parameters[0]->GetValue();

  switch (Type) {
  case eTopLevel:
    break;
  case eProduct:
    for (i=1;i<Parameters.size();i++) temp *= Parameters[i]->GetValue();
    break;
  case eDifference:
    for (i=1;i<Parameters.size();i++) temp -= Parameters[i]->GetValue();
    break;
  case eSum:
    for (i=1;i<Parameters.size();i++) temp += Parameters[i]->GetValue();
    break;
  case eQuotient:
    temp /= Parameters[1]->GetValue();
    break;
  case ePow:
    temp = pow(temp,Parameters[1]->GetValue());
    break;
  case eExp:
    temp = exp(temp);
    break;
  case eAbs:
    temp = fabs(temp);
    break;
  case eSin:
    temp = sin(temp);
    break;
  case eCos:
    temp = cos(temp);
    break;
  case eTan:
    temp = tan(temp);
    break;
  case eACos:
    temp = acos(temp);
    break;
  case eASin:
    temp = asin(temp);
    break;
  case eATan:
    temp = atan(temp);
    break;
  case eATan2:
    temp = atan2(temp, Parameters[1]->GetValue());
    break;
  //Matt's change start
  case eDerivative:
    {
      double new_time = time->getDoubleValue();
      if (new_time <= 0 || new_time < old_time)
      {
        temp = current_value = old_value = 0.0;
        old_time = new_time;
      }
      else if ( new_time == old_time)
      {
        temp = current_value;
	cerr << "error: can not get derivative. new_time = " << new_time << "  old_time = " << old_time << '\n'; 
      }
      else
      {
        current_value = (temp-old_value)/(new_time-old_time);
        old_time = new_time;
        old_value = temp;
        temp = current_value;
      }
    }
    break;
  case eIntegral:
    {
      double new_time = time->getDoubleValue();
      if (new_time == 0)
      {
        current_value = 0.0;
        old_time = new_time;
        temp = current_value;
       }
      else if (new_time==old_time)
      {
        temp = current_value;
      }
      else
      {
        current_value += temp*(new_time-old_time);
        old_time = new_time;
        temp = current_value;
      }
    }
    break;
  case eLT:
    temp = (temp < Parameters[1]->GetValue())?1.0:0.0;
    break;
  case eLE:
    temp = (temp <= Parameters[1]->GetValue())?1.0:0.0;
    break;
  case eGT:
    temp = (temp > Parameters[1]->GetValue())?1.0:0.0;
    break;
  case eGE:
    temp = (temp >= Parameters[1]->GetValue())?1.0:0.0;
    break;
  case eEQ:
    temp = (temp == Parameters[1]->GetValue())?1.0:0.0;
    break;
  case eNE:
    temp = (temp != Parameters[1]->GetValue())?1.0:0.0;
    break;
  case eAND:
    {
      bool flag = (int(temp+0.5) != 0);
      for (i=1;i<Parameters.size() && flag;i++)
      {
        flag = flag && (int(Parameters[i]->GetValue()+0.5) != 0);
      }
      temp = flag?1.0:0.0;
    }
    break;
  case eOR:
    {
      bool flag = (int(temp+0.5) != 0);
      for (i=1;i<Parameters.size() && !flag;i++)
      {
        flag = flag || (int(Parameters[i]->GetValue()+0.5) != 0);
      }
      temp = flag?1.0:0.0;
    }
    break;
  case eNOT:
    temp = (int(temp+0.5) != 0)?0.0:1.0;
    break;
  case eIFTHEN:
    {
      int i = Parameters.size();
      if (int(temp+0.5) != 0)
      {
        if (i >= 2)
	{
	  temp = Parameters[1]->GetValue();
	}
	else
	{
	  temp = 0.0;
	}
      }
      else
      {
        if (i >= 3)
	{
	  temp = Parameters[2]->GetValue();
	}
	else
	{
	  temp = 0.0;
	}
      }
    }
    break;
  //Matt's change end.
  default:
    cerr << "Unknown function operation type" << endl;
    break;
  }

  return temp;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

string FGFunction::GetValueAsString(void) const
{
  char buffer[20];
  string value;

  sprintf(buffer,"%9.6f",GetValue());
  value = string(buffer);
  return value;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void FGFunction::bind(void)
{
  if ( !Name.empty() ) {
    string tmp = PropertyManager->mkPropertyName(Prefix + Name, false); // Allow upper case
    PropertyManager->Tie( tmp, this, &FGFunction::GetValue);
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

void FGFunction::Debug(int from)
{
  if (debug_lvl <= 0) return;

  if (debug_lvl & 1) { // Standard console startup message output
    if (from == 0) { // Constructor
      if (Type == eTopLevel)
        cout << "    Function: " << Name << endl;
    }
  }
  if (debug_lvl & 2 ) { // Instantiation/Destruction notification
    if (from == 0) cout << "Instantiated: FGFunction" << endl;
    if (from == 1) cout << "Destroyed:    FGFunction" << endl;
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
