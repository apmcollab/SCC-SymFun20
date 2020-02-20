#include <exception>
#include <stdexcept>
#include <string>

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

#ifndef  SymFunException_
#define  SymFunException_
namespace SCC
{

/*! \class SCC::SymFunException
    \brief A class derived from std_runtime_error to facilitate exception handling for SCC::SymFun

	A class derived from std_runtime_error to facilitate exception handling for SCC::SymFun.

     Required version of C++ : >=  C++11

    \headerfile SCC_SymFunException.h "SCC_SymFunException.h"
    <HR>
    Sample code illustrating use of SymFunException within a try/catch block.

    \code
    std::string S        = "x^2";            // specifying a function in x

    SCC::SymFun F;                           // Create instance

    try
    {
    F.initialize(S);                         // initialize instance
    }
    catch (const SCC::SymFunException& e)    // exception handling
    {
          std::cerr << e.what() << std::endl;;
          std::cerr << "XXXX Execution Terminated XXXXX" << std::endl;
          exit(1);
    }


    std::cout << "x^2 evaluated at 2.0 = " << F(2.0) << std::endl;  // evaluate and output
    \endcode
 */
class SymFunException : public std::runtime_error
{

public:


  /**
    Returns the complete error message as a string.
   */

  virtual const char* what() const throw()
  {
    return errorReturn.c_str();
  }

/**
  Returns the base error message.
 */

  std::string getErrorMessage()      const { return errorMessage;};

  /**
    Returns specific information about the type of error.
  */

  std::string getErrorInformation () const { return errorInfo;};

  /**
    Returns function specification std::string that caused the error.
  */

  std::string getOffendingString  () const { return offendingString;}

protected :

   // Creation of instances limited to  SymFun and ExpressionTransform class

   friend class SymFun;
   friend class ExpressionTransform;
   friend class SymFunUtility;

    SymFunException () : std::runtime_error("SymFun error"),
    errorMessage("SymFun error"),errorInfo(""),offendingString("")
    {
	 setErrorReturn();
    }


  SymFunException (const std::string& msg, const std::string& errInfo, const std::string& offString)
  : std::runtime_error("SymFun error"),
  errorMessage(msg),errorInfo(errInfo), offendingString(offString)
  {
  setErrorReturn();
  }

  virtual ~SymFunException () throw() {}


  void setErrorReturn()
  {
	errorReturn =  errorMessage + "\n" +  errorInfo + "\n" +  offendingString + "\n";
  }

  std::string errorMessage;
  std::string errorInfo;
  std::string offendingString;

  std::string errorReturn;
};
}
#endif
