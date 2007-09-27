/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Header: FGFunction.h
Author: Jon Berndt
Date started: August 25 2004

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

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
SENTRY
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#ifndef FGFUNCTION_H
#define FGFUNCTION_H

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include <vector>
#include <string>
#include "FGParameter.h"
#include <input_output/FGXMLElement.h>
#include <input_output/FGPropertyManager.h>

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
DEFINITIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#define ID_FUNCTION "$Id$"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
FORWARD DECLARATIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

namespace JSBSim {

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DOCUMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/** Represents a mathematical function.
The FGFunction class is a powerful and versatile resource that allows
algebraic functions to be defined in a JSBSim configuration file. It is
similar in concept to MathML (Mathematical Markup Language, www.w3.org/Math/),
but simpler and more terse.
A function definition consists of an operation, a value, a table, or a property
(which evaluates to a value). The currently supported operations are:
- sum (takes n args)
- difference (takes n args)
- product (takes n args)
- quotient (takes 2 args)
- pow (takes 2 args)
- exp (takes 2 args)
- abs (takes n args)
- sin (takes 1 arg)
- cos (takes 1 arg)
- tan (takes 1 arg)
- asin (takes 1 arg)
- acos (takes 1 arg)
- atan (takes 1 arg)
- atan2 (takes 2 args)

An operation is defined in the configuration file as in the following example:

@code
  <sum>
    <value> 3.14159 </value>
    <property> velocities/qbar </property>
    <product>
      <value> 0.125 </value>
      <property> metrics/wingarea </property>
    </product>
  </sum>
@endcode

A full function definition, such as is used in the aerodynamics section of a
configuration file includes the function element, and other elements. It should
be noted that there can be only one non-optional (non-documentation) element -
that is, one operation element - in the top-level function definition.
Multiple value and/or property elements cannot be immediate child
members of the function element. Almost always, the first operation within the
function element will be a product or sum. For example:

@code
<function name="aero/coefficient/Clr">
    <description>Roll moment due to yaw rate</description>
    <product>
        <property>aero/qbar-area</property>
        <property>metrics/bw-ft</property>
        <property>aero/bi2vel</property>
        <property>velocities/r-aero-rad_sec</property>
        <table>
            <independentVar>aero/alpha-rad</independentVar>
            <tableData>
                 0.000  0.08
                 0.094  0.19
            </tableData>
        </table>
    </product>
</function>
@endcode

The "lowest level" in a function is always a value or a property, which cannot
itself contain another element. As shown, operations can contain values,
properties, tables, or other operations. In the first above example, the sum
element contains all three. What is evaluated is written algebraically as:

@code 3.14159 + qbar + (0.125 * wingarea) @endcode

Some operations can take only a single argument. That argument, however, can be
an operation (such as sum) which can contain other items. The point to keep in
mind is that it evaluates to a single value - which is just what the trigonometric
functions require (except atan2, which takes two arguments).

@author Jon Berndt
*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
DECLARATION: FGFunction
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

class FGFunction : public FGParameter
{
public:

/** Constructor.
    When this constructor is called, the XML element pointed to in memory by the
    element argument is traversed. If other FGParameter-derived objects (values,
    functions, properties, or tables) are encountered, this instance of the
    FGFunction object will store a pointer to the found object and pass the relevant
    Element pointer to the constructor for the new object. In other words, each
    FGFunction object maintains a list of "child" FGParameter-derived objects which
    in turn may each contain its own list, and so on. At runtime, each object
    evaluates its child parameters, which each may have its own child parameters to
    evaluate.
    @param PropertyManager a pointer to the property manager instance.
    @param element a pointer to the Element object containing the function definition.
    @param prefix an optional prefix to prepend to the name given to the property
           that represents this function (if given).
*/
  FGFunction(FGPropertyManager* PropertyManager, Element* element, string prefix="");
  /// Destructor.
  virtual ~FGFunction();

/** Retrieves the value of the function object.
    @return the total value of the function. */
  double GetValue(void) const;

/** The value that the function evaluates to, as a string.
  @return the value of the function as a string. */
  string GetValueAsString(void) const;

/// Retrieves the name of the function.
  string GetName(void) const {return Name;}

/** Specifies whether to cache the value of the function, so it is calculated only
    once per frame.
    If shouldCache is true, then the value of the function is calculated, and
    a flag is set so further calculations done this frame will use the cached value.
    In order to turn off caching, cacheValue must be called with a false argument.
    @param shouldCache specifies whether the function should cache the computed value. */
  void cacheValue(bool shouldCache);

  //Matt's change start:
  FGParameter* GetParameter(size_t i) {return Parameters[i];}
  const FGParameter* GetParameter(size_t i) const {return Parameters[i];}
  size_t GetParaCount() const {return Parameters.size();}
  enum functionType {eTopLevel=0, eProduct, eDifference, eSum, eQuotient, ePow,
                     eExp, eAbs, eSin, eCos, eTan, eASin, eACos, eATan, eATan2, 
                     eDerivative, eIntegral, 
                     eLT, eLE, eGE, eGT, eEQ, eNE,
                     eAND, eOR, eNOT,
                     eIFTHEN
  };
  functionType GetType() const {return Type;}
  string GetDescription() const {return description;}
  //Matt's change end.

private:
  vector <FGParameter*> Parameters;
  FGPropertyManager* const PropertyManager;
  bool cached;
  string Prefix;
  double cachedValue;
  //Matt's change start:
  //enum functionType {eTopLevel=0, eProduct, eDifference, eSum, eQuotient, ePow,
  //                   eExp, eAbs, eSin, eCos, eTan, eASin, eACos, eATan, eATan2} Type;
  functionType Type;
  const FGPropertyManager* time;
  mutable double current_value;
  mutable double old_time;
  mutable double old_value;
  string description;
  //Matt's change end.
  string Name;
  void bind(void);
  void Debug(int from);
};

} // namespace JSBSim

#endif
