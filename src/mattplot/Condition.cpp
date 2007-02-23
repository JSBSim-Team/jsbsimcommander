/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 Origin Module:       Condition.cpp
 Author:       Jon S. Berndt
 Date started: 1/2/2003

 -------------- Copyright (C) 2003 Jon S. Berndt (jsb@hal-pc.org) --------------
 
 Module:       Condition.cpp
 Modified By: Matthew Gong
 Date : 9/30/2006

 -------- Copyright (C) 2006 Matthew Gong (matthewzhenggong@gmail.com) --------

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

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
COMMENTS, REFERENCES,  and NOTES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include "Condition.h"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS IMPLEMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
namespace Matt
{
string Condition::indent = "        ";


Condition::Condition(Element* element, FGPropertyManager* PropertyManager) :
  PropertyManager(PropertyManager), isGroup(true)
{
  string property1, property2, logic;
  Element* condition_element;

  InitializeConditionals();

  TestParam1  = TestParam2 = 0L;
  //Matt's change here
  sign1 = sign2 = 1;
  TestValue1   = 0.0;
  TestValue2   = 0.0;
  Comparison  = ecUndef;
  Logic       = elUndef;
  conditions.clear();

  if (element)
  {
  logic = element->GetAttributeValue("logic");
  if (logic == "OR") Logic = eOR;
  else if (logic == "AND") Logic = eAND;
  condition_element = element->GetElement();
  while (condition_element) {
    conditions.push_back(Condition(condition_element, PropertyManager));
    condition_element = element->GetNextElement();
  }
  for (int i=0; i<element->GetNumDataLines(); i++) {
    conditions.push_back(Condition(element->GetDataLine(i), PropertyManager));
  }
  }
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Condition::Condition(string test, FGPropertyManager* PropertyManager) :
  PropertyManager(PropertyManager), isGroup(false)
{
  string property1, property2, compare_string;
  Element* condition_element;

  InitializeConditionals();

  TestParam1  = TestParam2 = 0L;
  //Matt's change here
  sign1 = sign2 = 1;
  TestValue1  = 0.0;
  TestValue2  = 0.0;
  Comparison  = ecUndef;
  Logic       = elUndef;
  conditions.clear();

  int start = 0, end = 0;
  start = test.find_first_not_of(" ");
  end = test.find_first_of(" ", start+1);
  property1 = test.substr(start,end-start);
  start = test.find_first_not_of(" ",end);
  end = test.find_first_of(" ",start+1);
  conditional = test.substr(start,end-start);
  start = test.find_first_not_of(" ",end);
  end = test.find_first_of(" ",start+1);
  property2 = test.substr(start,end-start);

  //Matt's change start
  //TestParam1 = PropertyManager->GetNode(property1, true);
  if (property1.find_first_not_of("-.0123456789eE") == string::npos) {
    TestValue1 = atof(property1.c_str());
  } else {
     if (property1[0] == '-') {
       sign1 = -1.0;
       property1.erase(0,1);
     } else {
       sign1 = 1.0;
     }
     TestParam1 = PropertyManager->GetNode(property1, true);
  }
  //Matt's change end
  Comparison = mComparison[conditional];
  if (property2.find_first_not_of("-.0123456789eE") == string::npos) {
    //Matt's change here
    TestValue2 = atof(property2.c_str());
  } else {
    //Matt's change here
     if (property2[0] == '-') {
       sign2 = -1.0;
       property2.erase(0,1);
     } else {
       sign2 = 1.0;
     }
     TestParam2 = PropertyManager->GetNode(property2, true);
  }
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void Condition::InitializeConditionals(void)
{
  mComparison["EQ"] = eEQ;
  mComparison["NE"] = eNE;
  mComparison["GT"] = eGT;
  mComparison["GE"] = eGE;
  mComparison["LT"] = eLT;
  mComparison["LE"] = eLE;
  mComparison["eq"] = eEQ;
  mComparison["ne"] = eNE;
  mComparison["gt"] = eGT;
  mComparison["ge"] = eGE;
  mComparison["lt"] = eLT;
  mComparison["le"] = eLE;
  mComparison["=="] = eEQ;
  mComparison["!="] = eNE;
  mComparison[">"]  = eGT;
  mComparison[">="] = eGE;
  mComparison["<"]  = eLT;
  mComparison["<="] = eLE;
  mComparison["MOD"] = eMOD;
  mComparison["mod"] = eMOD;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Condition::~Condition(void)
{
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

bool Condition::Evaluate(void )
{
  vector <Condition>::iterator iConditions;
  bool pass = true;
  //Matt's change here
  double compareValue1;
  double compareValue2;

  iConditions = conditions.begin();
  if (Logic == eAND) {

    pass = true;
    while (iConditions < conditions.end()) {
      if (!iConditions->Evaluate())
      {
	      pass = false;
	      break;
      }
      *iConditions++;
    }

  } else if (Logic == eOR) {

    pass = false;
    while (iConditions < conditions.end()) {
      if (iConditions->Evaluate())
      {
	      pass = true;
	      break;
      }
      *iConditions++;
    }

  } else {

    //Matt's change here
    if (TestParam1 != 0L) compareValue1 = TestParam1->getDoubleValue();
    else compareValue1 = TestValue1;
    if (TestParam2 != 0L) compareValue2 = TestParam2->getDoubleValue();
    else compareValue2 = TestValue2;

    switch (Comparison) {
    case eEQ:
      pass = compareValue1 == compareValue2;
      break;
    case eNE:
      pass = compareValue1 != compareValue2;
      break;
    case eGT:
      pass = compareValue1 > compareValue2;
      break;
    case eGE:
      pass = compareValue1 >= compareValue2;
      break;
    case eLT:
      pass = compareValue1 < compareValue2;
      break;
    case eLE:
      pass = compareValue1 <= compareValue2;
      break;
    case eMOD:
      pass = ( (int)compareValue1 % (int)compareValue2) == 0 ? true : false;
      break;
    default:
      pass = true;
    }
  }

  return pass;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void Condition::PrintCondition(void )
{
  vector <Condition>::iterator iConditions;
  string scratch;

  if (isGroup) {
    switch(Logic) {
    case (elUndef):
      scratch = " UNSET";
      cerr << "unset logic for test condition" << endl;
      break;
    case (eAND):
      scratch = " if all of the following are true";
      break;
    case (eOR):
      scratch = " if any of the following are true:";
      break;
    default:
      scratch = " UNKNOWN";
      cerr << "Unknown logic for test condition" << endl;
    }

    iConditions = conditions.begin();
    cout << scratch << endl;
    while (iConditions < conditions.end()) {
      iConditions->PrintCondition();
      *iConditions++;
    }
  } else {
    if (TestParam2 != 0L)
      if (TestParam1 != 0L)
        cout << TestParam1->GetName() << " " << conditional << " " << TestParam2->GetName();
      else
        cout << TestValue1 << " " << conditional << " " << TestParam2->GetName();
    else
      if (TestParam1 != 0L)
        cout << TestParam1->GetName() << " " << conditional << " " << TestValue2;
      else
        cout << TestValue1 << " " << conditional << " " << TestValue2;
  }
}

}  // namespace
