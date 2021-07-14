/*
 * SCC_SymFunXMLinputUtility.h
 *
 *  Created on: Jul 1, 2021
 *      Author: anderson
 */

//
//##################################################################
//                     SCC_SymFunXMLinputUtility.h
//##################################################################
//
// A class used to facilitate the input of SCC::SymFun instances
// from an XML specification.
//
// The parameter list required for specification is demonstrated
// in the following example. The name of the parameter list is user
// defined, but a functionString and list of variables and their coordinate
// index must be specified.
//
// A coordinate index is necessary when specifying variable names
// so that an evaluation invocation whose argument is (3.0,4.0,5.0) assigns
///the value 3.0 to the coordinate with index 1, 4.0 the value 4.0 is assigned
// to the coordinate with index 2, etc.
//
// If there are symbolic constants, then these must be specified along
// with default values as child parameters of the symbolicConstants parameter.
/*
<A_TestFunction>

<functionString value = "a*x^2 + b*y^2 + c*z^2 + gamma + 3.0" type ="string" />

<!--  variables : specify variable name and value = coordinate index  -->

<variables>
<x value = "1" />
<y value = "2" />
<z value = "3" />
</variables>

<!-- symbolic constants (i.e. coefficients) with values -->

<symbolicConstants>
<a value      = "1.0"  />
<b value      = "0.5"  />
<c value      = "0.5"  />
<gamma value  = "1.0"  />
</symbolicConstants>
</A_TestFunction>
*/
/*
#############################################################################
#
# Copyright 1996-2020 Chris Anderson
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the Lesser GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# For a copy of the GNU General Public License see
# <http://www.gnu.org/licenses/>.
#
#############################################################################
*/
#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include "XML_ParameterList/XML_ParameterListArray.h"
#include "XML_ParameterList/XML_ParameterCheck.h"
#include "SymFun20/SCC_SymFun.h"

#ifndef SYM_FUN_XML_INPUT_UTILITY_
#define SYM_FUN_XML_INPUT_UTILITY_

namespace SCC
{
class SymFunXMLinputUtility
{

public:

	SymFunXMLinputUtility(){};
	virtual ~SymFunXMLinputUtility(){};

	void initSymFunFromXML(const std::string& functionParamListName,const XML_ParameterListArray& paramList,
	SCC::SymFun& F, bool verboseFlag = true)
	{
   // Capture function string and replace any carriage returns and/or line feeds with spaces

    std::string funString = (std::string)paramList.getParameterValue("functionString",functionParamListName);
	std::replace(funString.begin(),funString.end(), '\n', ' ');
	std::replace(funString.begin(),funString.end(), '\r', ' ');

    std::vector < std::string > variableNamesInput;
    std::vector < std::string > variableNames;
    long variableCount;
    if(paramList.isParameter("variables",functionParamListName))
    {
    	paramList.getParameterChildNames(0, "variables",functionParamListName,variableNamesInput);
    	variableCount = (long)variableNamesInput.size();
    	variableNames.resize(variableCount);

    	// Order variable names by coordinate index

    	int index;
    	for(long k = 0; k < variableCount; k++)
    	{
    		index = paramList.getParameterChildValue(variableNamesInput[k],"variables",functionParamListName);
    		if((index < 1)||(index > variableCount))
		    {
    			std::string errMsg = "\nSymFunXMLinputUtility.initSymFunFromXML error : illegal variable coordinate index \n";
    			errMsg.append("Offending variable :");
    			errMsg.append(variableNamesInput[k] + "\n");
    			errMsg.append("Offending index    :");
    			errMsg.append(std::to_string(index) + "\n");
    			throw std::runtime_error(errMsg);
		    }
    		variableNames[index-1] = variableNamesInput[k];
    	}
    	if(verboseFlag) {for(long k = 0; k < variableCount; k++){printf("%s \n",variableNames[k].c_str());}}
    }

    // Capture the coefficient names and initial coefficient values

    std::vector <std::string > coefficientNames;
    std::vector <double>      coefficientValues;

    long coefficientCount;

    if(paramList.isParameter("symbolicConstants",functionParamListName))
    {
    paramList.getParameterChildNames(0, "symbolicConstants",functionParamListName,coefficientNames);
    coefficientCount = (long)coefficientNames.size();
    coefficientValues.resize(coefficientCount);

    for(long k = 0; k < coefficientCount; k++)
    {
    	coefficientValues[k] = paramList.getParameterChildValue(coefficientNames[k],"symbolicConstants",functionParamListName);
    	if(verboseFlag)
    	{printf("%s  = %15.5e \n",coefficientNames[k].c_str(),coefficientValues[k]);}
    }
    }

    try
    {F.initialize(variableNames,coefficientNames,coefficientValues,funString);}
    catch (const SCC::SymFunException& e)
    {
    	  std::string errMsg = "\n";
    	  errMsg.append(e.what());
    	  errMsg.append("\n");
          throw std::runtime_error(errMsg);
    }

	}

    void getCoefficientMapFromXML(std::string& functionParamListName, XML_ParameterListArray& paramList,
	std::map< std::string , double >& coefficientMap)
	{
    // Capture the coefficient names and coefficient values

    std::vector <std::string > coefficientNames;
    std::vector <double>      coefficientValues;

    long coefficientCount;

    coefficientMap.clear();
    if(paramList.isParameter("symbolicConstants",functionParamListName))
    {
    	paramList.getParameterChildNames(0, "symbolicConstants",functionParamListName,coefficientNames);
    	coefficientCount = (long)coefficientNames.size();
    	coefficientValues.resize(coefficientCount);

    	for(long k = 0; k < coefficientCount; k++)
    	{
    		coefficientValues[k] = paramList.getParameterChildValue(coefficientNames[k],"coefficients",functionParamListName);
    		coefficientMap[coefficientNames[k]] = coefficientValues[k];
    	}
    }
	}


    // Returns true on error found

    bool checkParametersError(const XML_ParameterListArray& paramList,
    const std::string& functionParamListName, std::string& errMsg)
    {
	XML_ParameterCheck xmlCheck;
	bool errorFlag      = false;

	std::string routineName = "SCC::SymFunXMLinputUtility";

    // Single particle operator parameter check


	errorFlag = xmlCheck.checkParameterError(routineName, paramList, "functionString",functionParamListName,errMsg)  || errorFlag;
    errorFlag = xmlCheck.checkParameterError(routineName, paramList,   "variables",   functionParamListName,errMsg)  || errorFlag;

	return errorFlag;
}
};
} // Namespace SCC

#endif
