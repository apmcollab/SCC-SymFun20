
//
//##################################################################
//  		       SCC_SymFunUtilities.h 
//##################################################################
//
//
// BETA version of SymFun utilities. In particular, this class
// provides a member function to evaluate the derivative of a SymFun instance and
// return the result as a SymFun instance.
//
// Author: Chris Anderson
// (C) UCLA 2012-2020
//           
// Version : 02/04/2020
//
/*
#############################################################################
#
# Copyright 2012-2020 Chris Anderson
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
#include "SCC_SymFun.h"

//
// strcpy_s is not implemented as part of C++11 (arrgh) so this macro
// inserts strcpy calls.
//

#ifdef _MSC_VER
#define COPYSTR(src,count,dst) strcpy_s(src,count,dst)
#else
#define COPYSTR(src,count,dst) strcpy(src,dst)
#endif


#include <string>
#include <cstring>
#include <cstdlib>
#include <sstream>
using namespace std;


#ifndef SYMUTILITY_
#define SYMUTILITY_

namespace SCC
{

/*!
 \class SCC::SymFunUtility
  \brief A class whose member functions differentiate SCC::SymFun instances

   SCC::SymUtility is a BETA version of a class that  provides member functions to evaluate the
   derivative of a SymFun instance symbolically and return the result as a SymFun instance.

   Required version of C++ : >=  C++11

  \headerfile SCC_SymFunUtility.h "SCC_SymFunUtility.h"
*/


class SymFunUtility
{
public:

/**
     Null constructor.
*/

SymFunUtility(){};

//
//#####################################################################
//                  differentiate
//#####################################################################
//
/*!
	Differentiates the input SCC::SymFun instance symbolically with 
    respect to the variable whose name is specified in var and
    returns the result as an SCC::SymFun instance. 

    @F        : The SCC::SymFun instance to be differentiated
    @varName  : The name of the variable that the input function is being differentiated with respect to. 

    <HR>
    Sample usage for differentiating a function of two variables x and y.
	\code

	vector<string>  V = {"x","y"};                       // specify variables

    std::string S = "2.0*x+ sin(x) + y^2";               // specify function

    SCC::SymFun F(V,S);                                   // create instance

    SCC::SymFun   DFx;                                    // Instances for partial derivatives
    SCC::SymFun   DFy;

    SCC::SymFunUtility symFunUtility;

    DFx = symFunUtility.differentiate(F,"x");
    DFy = symFunUtility.differentiate(F,"y");

    cout << "SymFun F(x,y) " << endl << endl;
    cout << F << endl << endl;

    cout << "SymFun derivative of F(x,y) with respect to x " << endl << endl;
    cout << DFx << endl << endl;


    cout << "SymFun derivative of F(x,y) with respect to y " << endl << endl;
    cout << DFy << endl << endl;
    \endcode

*/
SCC::SymFun differentiate(SCC::SymFun& F,const string& varName)
{
	return symbolicDifferentiate(F,varName.c_str());
}


protected:

SCC::SymFun symbolicDifferentiate(SCC::SymFun& F,const char* var)
{
    const char**V    = 0; // variable names  pointer
    const char**C    = 0; // constant names  pointer
    double* cV = 0; // constant values pointer

    V    = (const char**)F.getVariableNamePtr();
    C    = (const char**)F.getConstantNamePtr();
    cV   = F.getConstantValuePtr();


    long variableCount = F.variableCount;
    long constantCount = F.constantCount;
    long symbolCount   = F.symbolCount;

    SCC::SymFun D;   // return argument

    SCC::RealOperatorLib  L;

    long    i;
    int     initReturn;
    long    functionIndex;
    long    argCount;
    long    stringSize;
    long    resultIndex;

    ostringstream sbuf;
    sbuf.str("");

//
//  Step #1 create an evaluation array for the original function
//
    char** evaluationStrings  = new char*[F.evaluationDataSize];
    long*  evaluationPriority = new long[F.evaluationDataSize];

    createEvaluationStrings(F,evaluationStrings,evaluationPriority);
//
//  Identify index of variable being differentiated.
//
    long diffIndex = -1;
    for(i = 0; i < variableCount; i++)
    {
    if(!strcmp(var,V[i])) diffIndex = i;
    }

    if(diffIndex == -1)
    {
    initReturn = D.initialize(V,variableCount,C,constantCount,cV,"0");
    return D;
    }
//
//  Now compose derivative string
//
//
//  Initialize evaluation strings
//
    char** devaluationStrings  = new char*[F.evaluationDataSize];
    //
    // not used?
//  long*  devaluationPriority = new long[F.evaluationDataSize];

    char dfunctionString[16];

    for(i=0; i < symbolCount; i++)
    {
    devaluationStrings[i] = new char[1];
    COPYSTR(devaluationStrings[i],1,"");
    }

    delete [] devaluationStrings[diffIndex];
    devaluationStrings[diffIndex] = new char[2];
    COPYSTR(devaluationStrings[diffIndex],2,"1");

    long arg1Index;
    long arg2Index;
    long sSize;

    int    iexp       = 0;
    double dexp       = 0.0;
    char*  decimalPtr = 0;

    long executionIndex = 0;
    while(executionIndex < F.executionArraySize)
    {

    functionIndex = F.executionArray[executionIndex]; executionIndex++;
    argCount      = F.executionArray[executionIndex]; executionIndex++;
    resultIndex   = F.executionArray[executionIndex+(argCount-1)];

    if(argCount == 2)
    {
    arg1Index = F.executionArray[executionIndex];
    arg2Index = -1;
    }
    else if(argCount == 3)
    {
    arg1Index = F.executionArray[executionIndex];
    arg2Index = F.executionArray[executionIndex+1];
    }
    //
    //**************************************************************
    //
    // Estimate derivative evaluation string size,
    // resize composition buffer if necessary
    //
    //**************************************************************
    //
    if(argCount == 2)
    {
    if      ((!strcmp(L.Symbols[functionIndex],"+"))||
             (!strcmp(L.Symbols[functionIndex],"-")))
    {
     sSize = 4;
     sSize += (long)strlen(devaluationStrings[arg1Index]);
    }
    else if((!strcmp(L.Symbols[functionIndex],"sin")) ||
            (!strcmp(L.Symbols[functionIndex],"cos")) ||
            (!strcmp(L.Symbols[functionIndex],"exp")) ||
            (!strcmp(L.Symbols[functionIndex],"cosh"))||
            (!strcmp(L.Symbols[functionIndex],"sinh"))
           )
    {
     sSize  = 20;
     sSize += (long)strlen(evaluationStrings[arg1Index]);
     sSize += (long)strlen(devaluationStrings[arg1Index]);
    }
    else if((!strcmp(L.Symbols[functionIndex],"asin")) ||
            (!strcmp(L.Symbols[functionIndex],"acos")) ||
            (!strcmp(L.Symbols[functionIndex],"atan")))
    {
     sSize  = 30;
     sSize += (long)strlen(evaluationStrings[arg1Index]);
     sSize += (long)strlen(devaluationStrings[arg1Index]);
    }
    else if(!strcmp(L.Symbols[functionIndex],"log"))
    {
     sSize  = 20;
     sSize += (long)strlen(evaluationStrings[arg1Index]);
     sSize += (long)strlen(devaluationStrings[arg1Index]);
    }
    else if(!strcmp(L.Symbols[functionIndex],"log10"))
    {
     sSize  = 30;
     sSize += (long)strlen(evaluationStrings[arg1Index]);
     sSize += (long)strlen(devaluationStrings[arg1Index]);
    }
    else if(!strcmp(L.Symbols[functionIndex],"sqrt"))
    {
     sSize  = 30;
     sSize += (long)strlen(evaluationStrings[arg1Index]);
     sSize += (long)strlen(devaluationStrings[arg1Index]);
    }
    else if(!strcmp(L.Symbols[functionIndex],"tan"))
    {
     sSize  = 30;
     sSize += (long)strlen(evaluationStrings[arg1Index]);
     sSize += (long)strlen(devaluationStrings[arg1Index]);
    }
    }
    if(argCount == 3)
    {
    if     ((!strcmp(L.Symbols[functionIndex],"+"))||
           (!strcmp(L.Symbols[functionIndex],"-")))
    {
    sSize  = 5;
    sSize += (long)strlen(devaluationStrings[arg1Index]);
    sSize += (long)strlen(devaluationStrings[arg2Index]);
    }
    else if(!strcmp(L.Symbols[functionIndex],"*"))
    {
    sSize  = 30;
    sSize += (long)strlen(devaluationStrings[arg1Index]);
    sSize += (long)strlen(devaluationStrings[arg2Index]);
    sSize += (long)strlen(evaluationStrings[arg1Index]);
    sSize += (long)strlen(evaluationStrings[arg2Index]);
    }
    else if(!strcmp(L.Symbols[functionIndex],"/"))
    {
    sSize  = 30;
    sSize +=   (long)strlen(devaluationStrings[arg1Index]);
    sSize +=   (long)strlen(devaluationStrings[arg2Index]);
    sSize +=   (long)strlen(evaluationStrings[arg1Index]);
    sSize += 2*(long)strlen(evaluationStrings[arg2Index]);
    }
    else if((!strcmp(L.Symbols[functionIndex],"^"))||
            (!strcmp(L.Symbols[functionIndex],"pow")))
    {
    sSize =  (long)strlen(devaluationStrings[arg1Index]);
    sSize += (long)strlen(devaluationStrings[arg2Index]);
    sSize += 3*(long)strlen(evaluationStrings[arg1Index]);
    sSize += 3*(long)strlen(evaluationStrings[arg2Index]);
    sSize += 20;
    }
    }


//
//  Compose the string representation of
//  the derivative. This is hand coded; I don't worry about having
//  too many paranthesis; these get cleaned up when the derivative
//  string is expressed as a symbolic function.
//
//
    if(argCount == 2)
    {
    //
    //*********************************************
    //                    + -
    //*********************************************
    //
    if((!strcmp(L.Symbols[functionIndex],"+"))||
    (!strcmp(L.Symbols[functionIndex],"-")))
    {
        if(strlen(devaluationStrings[arg1Index])==0)
        {sbuf << ends;}
        else
        {
        sbuf << L.Symbols[functionIndex]      << "("
                << devaluationStrings[arg1Index] << ")" << ends;
        }
    }
    //
    //*********************************************
    //  sin, cos, exp, cosh, sinh
    //*********************************************
    //
    else if((!strcmp(L.Symbols[functionIndex],"sin")) ||
            (!strcmp(L.Symbols[functionIndex],"cos")) ||
            (!strcmp(L.Symbols[functionIndex],"exp")) ||
            (!strcmp(L.Symbols[functionIndex],"cosh"))||
            (!strcmp(L.Symbols[functionIndex],"sinh"))
           )
    {

    if      (!strcmp(L.Symbols[functionIndex],"sin"))   COPYSTR(dfunctionString,4, "cos");
    else if (!strcmp(L.Symbols[functionIndex],"cos"))   COPYSTR(dfunctionString,5,"-sin");
    else if (!strcmp(L.Symbols[functionIndex],"exp"))   COPYSTR(dfunctionString,4,"exp");
    else if (!strcmp(L.Symbols[functionIndex],"cosh"))  COPYSTR(dfunctionString,5,"sinh");
    else if (!strcmp(L.Symbols[functionIndex],"sinh"))  COPYSTR(dfunctionString,5,"cosh");

        if(strlen(devaluationStrings[arg1Index])==0)
        {sbuf << ends;}
        else
        {
        if((!strcmp(devaluationStrings[arg1Index],"1"))
        ||(!strcmp(devaluationStrings[arg1Index],"(1)")))
        {
        sbuf << "(" << dfunctionString << "("  << evaluationStrings[arg1Index]
                << "))" << ends;
        }
        else
        {
         sbuf << "(" << dfunctionString << "(" << evaluationStrings[arg1Index]
                 << "))*(" << devaluationStrings[arg1Index] << ")" << ends;
        }}
    }
    //
    //*********************************************
    //  asin, acos, atan
    //*********************************************
    //
    else if((!strcmp(L.Symbols[functionIndex],"asin")) ||
            (!strcmp(L.Symbols[functionIndex],"acos")) ||
            (!strcmp(L.Symbols[functionIndex],"atan")))
    {
        if(strlen(devaluationStrings[arg1Index])==0)
        {sbuf << ends;}
        else
        {
          if((!strcmp(devaluationStrings[arg1Index],"1"))
          ||(!strcmp(devaluationStrings[arg1Index],"(1)")))
          {

          if     (!strcmp(L.Symbols[functionIndex],"asin")) {(sbuf) << "(1./sqrt(1.-(";}
          else if(!strcmp(L.Symbols[functionIndex],"acos")) {(sbuf) << "(-1./sqrt(1-(";}
          else if(!strcmp(L.Symbols[functionIndex],"atan")) {(sbuf) << "(1./(1.+(";}

          sbuf << evaluationStrings[arg1Index] << ")^2))" << ends;
          }
          else
          {

          if     (!strcmp(L.Symbols[functionIndex],"asin")) {(sbuf) <<"((1./sqrt(1.-(";}
          else if(!strcmp(L.Symbols[functionIndex],"acos")) {(sbuf) <<"((-1./sqrt(1-(";}
          else if(!strcmp(L.Symbols[functionIndex],"atan")) {(sbuf) <<"((1./(1.+(";}

          sbuf <<evaluationStrings[arg1Index] << ")^2))" << "*"
          << "(" << devaluationStrings[arg1Index] << "))" << ends;
         }}
    }

    //
    //*********************************************
    //                   log(x)
    //*********************************************
    //
    else if(!strcmp(L.Symbols[functionIndex],"log"))
    {
        if(strlen(devaluationStrings[arg1Index])==0)
        {sbuf << ends;}
        else
        {
        if((!strcmp(devaluationStrings[arg1Index],"1"))
        ||(!strcmp(devaluationStrings[arg1Index],"(1)")))
        {
        sbuf << "(1./(" << evaluationStrings[arg1Index] << "))" << ends;
        }
        else
        {
        sbuf << "(1./(" << evaluationStrings[arg1Index]
                << "))*(" << devaluationStrings[arg1Index] << ")" << ends;
        }}
    }
    //
    //*********************************************
    //                   log10(x)
    //*********************************************
    //
    else if(!strcmp(L.Symbols[functionIndex],"log10"))
    {
        if(strlen(devaluationStrings[arg1Index])==0)
        {sbuf << ends;}
        else
        {
        if((!strcmp(devaluationStrings[arg1Index],"1"))
        ||(!strcmp(devaluationStrings[arg1Index],"(1)")))
        {
        sbuf << "(1./((" << evaluationStrings[arg1Index]
                << ")*log(10.0)))" << ends;
        }
        else
        {
        sbuf <<"((1./(("
                << evaluationStrings[arg1Index]  << ")*log(10.0)))*("
                << devaluationStrings[arg1Index] << "))" << ends;
        }}
    }
    //
    //*********************************************
    //                   sqrt(x)
    //*********************************************
    //
    else if(!strcmp(L.Symbols[functionIndex],"sqrt"))
    {
        if(strlen(devaluationStrings[arg1Index])==0)
        {sbuf << ends;}
        else
        {
        if((!strcmp(devaluationStrings[arg1Index],"1"))
        ||(!strcmp(devaluationStrings[arg1Index],"(1)")))
        {
        sbuf << "(0.5/(" << evaluationStrings[arg1Index]
                << ")^0.5)" << ends;
        }
        else
        {
        sbuf << "((0.5/(" << evaluationStrings[arg1Index] << ")^0.5)*("
                << devaluationStrings[arg1Index] << "))" << ends;
       }}
    }
    //
    //*********************************************
    //                   tan(x)
    //*********************************************
    //
    else if(!strcmp(L.Symbols[functionIndex],"tan"))
    {
        if(strlen(devaluationStrings[arg1Index])==0)
        {sbuf << ends;}
        else
        {
        if((!strcmp(devaluationStrings[arg1Index],"1"))
        ||(!strcmp(devaluationStrings[arg1Index],"(1)")))
        {
        sbuf << "(1/(cos(" << evaluationStrings[arg1Index]
                << ")^2))" << ends;
        }
        else
        {
        sbuf << "((1/(cos(" << evaluationStrings[arg1Index]
                << ")^2))*(" << devaluationStrings[arg1Index] << "))"
                << ends;
        }}
    }
    }

    //
    // BINARY OPERATORS
    //

    if(argCount == 3)
    {
    //
    //**************************************************
    //                  +   -
    //**************************************************
    //
    if((!strcmp(L.Symbols[functionIndex],"+"))||
       (!strcmp(L.Symbols[functionIndex],"-")))
    {

    if((strlen(devaluationStrings[arg1Index])==0)&&
       (strlen(devaluationStrings[arg2Index])==0))
       {sbuf << ends;}
    else
    if(strlen(devaluationStrings[arg2Index]) != 0)
    {
        sbuf << "(" << devaluationStrings[arg1Index] << L.Symbols[functionIndex] <<
        devaluationStrings[arg2Index] << ")" << ends;
    }
    else
    {
        sbuf << "(" << devaluationStrings[arg1Index] << ")" << ends;
    }}
    //
    //**************************************************
    //                      *
    //**************************************************
    //
    else if(!strcmp(L.Symbols[functionIndex],"*"))
    {
    if((strlen(devaluationStrings[arg1Index])==0)&&
       (strlen(devaluationStrings[arg2Index])==0))
       {sbuf << ends;}
    else
    {
    sbuf << "(";

    if(strlen(devaluationStrings[arg1Index]) != 0)
    {

        if((!strcmp(devaluationStrings[arg1Index],"1"))
        ||(!strcmp(devaluationStrings[arg1Index],"(1)")))
        {
        sbuf << evaluationStrings[arg2Index];
        }
        else
        {
        sbuf << devaluationStrings[arg1Index] << "*("
            << evaluationStrings[arg2Index] << ")";
        }
    }

    if(strlen(devaluationStrings[arg2Index]) != 0)
    {
        if(strlen(devaluationStrings[arg1Index]) != 0)
        {
        sbuf << "+";
        }
        if((!strcmp(devaluationStrings[arg2Index],"1"))
        ||(!strcmp(devaluationStrings[arg2Index],"(1)")))
        {
        sbuf << evaluationStrings[arg1Index];
        }
        else
        {
        sbuf << "(" << evaluationStrings[arg1Index] << ")*"
                << devaluationStrings[arg2Index];
        }
    }

    sbuf << ")" << ends;

    }}
    //
    //**************************************************
    //                      /
    //**************************************************
    //
    else if(!strcmp(L.Symbols[functionIndex],"/"))
    {
    if((strlen(devaluationStrings[arg1Index])==0)&&
       (strlen(devaluationStrings[arg2Index])==0))
    {sbuf << ends;}
    else
    {
    sbuf << "((";

    if(strlen(devaluationStrings[arg1Index]) != 0)
    {
        if((!strcmp(devaluationStrings[arg1Index],"1"))
        ||(!strcmp(devaluationStrings[arg1Index],"(1)")))
        {
            sbuf << evaluationStrings[arg2Index];
        }
        else
        {
            sbuf << devaluationStrings[arg1Index] << "*("
            <<  evaluationStrings[arg2Index] << ")";
        }
    }

    if(strlen(devaluationStrings[arg2Index]) != 0)
    {
    if((!strcmp(devaluationStrings[arg2Index],"1"))
    ||(!strcmp(devaluationStrings[arg2Index],"(1)")))
    {
    sbuf << "-" <<"(" << evaluationStrings[arg1Index]
            << ")";
    }
    else
    {
    sbuf << "-" <<"(" << evaluationStrings[arg1Index]
            << ")*" << devaluationStrings[arg2Index];
    }

    }
    sbuf << ")/((" <<  evaluationStrings[arg2Index] << ")^2))" << ends;
    }}

    //
    //**************************************************
    //                    pow,^
    //**************************************************
    //
    //
    // need to clean up things when the exponent is an
    // integer
    //
    else if((!strcmp(L.Symbols[functionIndex],"^"))||
            (!strcmp(L.Symbols[functionIndex],"pow")))
    {
    if((strlen(devaluationStrings[arg1Index])==0)&&
    (strlen(devaluationStrings[arg2Index])==0))
    {sbuf << ends;}
    else
    {
    if(strlen(devaluationStrings[arg1Index])!=0)
    {

    //
    // check to see if the evaluationString is a numeric constant
    //
    if((arg2Index < F.symbolCount)&&
       (arg2Index >= F.constantCount + F.variableCount))
    {

     // decimal value

      decimalPtr = 0;
      decimalPtr = strpbrk(evaluationStrings[arg2Index],".");

      if(decimalPtr != 0)
      {
      dexp = atof(evaluationStrings[arg2Index]) - 1.0;
      if(dexp ==  0.0)
      {
      sbuf << devaluationStrings[arg1Index];
      }
      else if(dexp == 1.0)
      {
         if((!strcmp(devaluationStrings[arg1Index],"1"))
         ||(!strcmp(devaluationStrings[arg1Index],"(1)")))
         {
         sbuf << "(" << evaluationStrings[arg2Index] << "*(" <<
         evaluationStrings[arg1Index] << "))";
         }
         else
        {
        sbuf << "(" << evaluationStrings[arg2Index] << "*(" <<
         evaluationStrings[arg1Index] << ")*(" << devaluationStrings[arg1Index] << "))";
        }
      }
      else
      {
        sbuf.precision((long)strlen(evaluationStrings[arg2Index]));

         if((!strcmp(devaluationStrings[arg1Index],"1"))
         ||(!strcmp(devaluationStrings[arg1Index],"(1)")))
         {
          sbuf << "(" << evaluationStrings[arg2Index] << ")*pow(" <<
          evaluationStrings[arg1Index] << "," << dexp <<
          ")";
         }
         else
         {
          sbuf << "(" << evaluationStrings[arg2Index] << ")*pow(" <<
          evaluationStrings[arg1Index] << "," << dexp <<
          ")" << "*(" << devaluationStrings[arg1Index] << ")";
         }
      }
           // integer
      }
      else
      {
        iexp = atoi(evaluationStrings[arg2Index]) -1;
        if(iexp == 0)
        {
            sbuf << devaluationStrings[arg1Index];
        }
        else if(iexp == 1)
        {
          if((!strcmp(devaluationStrings[arg1Index],"1"))
          ||(!strcmp(devaluationStrings[arg1Index],"(1)")))
          {
            sbuf << "(" << evaluationStrings[arg2Index] << "*(" <<
            evaluationStrings[arg1Index] << "))";
          }
          else
          {
            sbuf << "(" << evaluationStrings[arg2Index] << "*(" <<
            evaluationStrings[arg1Index] << ")*(" << devaluationStrings[arg1Index] << "))";
          }
        }
        else
        {
          if((!strcmp(devaluationStrings[arg1Index],"1"))
          ||(!strcmp(devaluationStrings[arg1Index],"(1)")))
          {
            sbuf << "(" << evaluationStrings[arg2Index] << "*pow(" <<
            evaluationStrings[arg1Index] << "," <<atoi(evaluationStrings[arg2Index]) - 1 <<
            "))";
          }
          else
          {
            sbuf << "(" << evaluationStrings[arg2Index] << "*pow(" <<
            evaluationStrings[arg1Index] << "," <<atoi(evaluationStrings[arg2Index]) - 1 <<
            "))" << "*(" << devaluationStrings[arg1Index] << ")";
          }
        }
      }
    }
    else
    {
         if((!strcmp(devaluationStrings[arg1Index],"1"))
         ||(!strcmp(devaluationStrings[arg1Index],"(1)")))
         {
           sbuf << "((" << evaluationStrings[arg2Index] << ")*(" <<
           evaluationStrings[arg1Index] << ")^(" << evaluationStrings[arg2Index] <<
           "-1.))";
         }
         else
         {
           sbuf << "((" << evaluationStrings[arg2Index] << ")*(" <<
           evaluationStrings[arg1Index] << ")^(" << evaluationStrings[arg2Index] <<
           "-1.))" << "*(" << devaluationStrings[arg1Index] << ")";
         }

    }
    }

    if(strlen(devaluationStrings[arg2Index])!=0)
    {
    if(strlen(devaluationStrings[arg1Index])!=0)
    {
    sbuf << "+";
    }
    sbuf << "((" << devaluationStrings[arg2Index] << ")*("
            << evaluationStrings[arg1Index]  << ")^("
            << evaluationStrings[arg2Index]  << "))" << "*log("
            << evaluationStrings[arg1Index]  << ")";
    }

    sbuf << ends;
    }}

    //
    //**************************************************
    //
    }
    //
    // Capture string
    //

    stringSize = (long)strlen((sbuf.str()).c_str());
    devaluationStrings[resultIndex] = new char[stringSize+1];
    COPYSTR(devaluationStrings[resultIndex], stringSize + 1,(sbuf.str()).c_str());
    sbuf.str("");

    executionIndex += argCount;
    }
//
// **********************************************************
//
//  Clean up the derivative string by creating a symbolic function,
//  and then an evaluation string
//

    // cout << devaluationStrings[resultIndex] << endl;

    if(strlen(devaluationStrings[resultIndex]) == 0)
    {
    initReturn = D.initialize(V,variableCount,C,constantCount,cV,"0");
    }
    else
    {
    initReturn = D.initialize(V,variableCount,C,constantCount,
    cV,devaluationStrings[resultIndex]);
    }

    if(initReturn) {cout << " Error " << endl;}

    char** dStrings  = new char*[D.evaluationDataSize];
    long*  dPriority = new long[D.evaluationDataSize];

    /* Fix later : problem string F = a0 + a1*(x/h), diff w.r.t. a0
    cout << endl;
    cout << D.evaluationDataSize << endl;
    cout << "DDD" << endl;
    cout << D.getConstructorString() << endl;
    cout << endl;
    */


    createEvaluationStrings(D,dStrings,dPriority);

    //
    // Capture the derivative as the last evaluation string
    //
    char* derivativeString = new char[strlen(dStrings[D.evaluationDataSize-1])+1];
    COPYSTR(derivativeString, strlen(dStrings[D.evaluationDataSize - 1]) + 1,dStrings[D.evaluationDataSize-1]);

    for(i = 0; i < D.evaluationDataSize; i++) {delete [] dStrings[i];}
    delete [] dStrings;
    delete [] dPriority;

    //
    // Reinitialize with the new derivative string
    //
    initReturn = 0;

    if(strlen(derivativeString) == 0)
    {
    initReturn = D.initialize(V,variableCount,C,constantCount,cV,"0");
    }
    else
    {
    initReturn = D.initialize(V,variableCount,C,constantCount,cV,
    derivativeString);
    }

    if(initReturn) {cout << " Error " << endl;}
//
//  clean up

    delete [] derivativeString;

    for(i = 0; i < F.evaluationDataSize; i++) {delete [] devaluationStrings[i];}
    delete [] devaluationStrings;

    for(i = 0; i < F.evaluationDataSize; i++) {delete [] evaluationStrings[i];}
    delete [] evaluationStrings;

    delete [] evaluationPriority;


    return D;
}


//
//#####################################################################
//                    createEvaluationStrings
//#####################################################################
//
void createEvaluationStrings(SCC::SymFun& F, char** evaluationStrings,
long* evaluationPriority)
{

    SCC::RealOperatorLib  L;

    ostringstream sbuf;
    sbuf.str("");

    long i;

//  long variableCount = F.variableCount;
//  long constantCount = F.constantCount;
    long symbolCount   = F.symbolCount;
//
//  Initialize evaluation strings
//
    for(i=0; i < symbolCount; i++)
    {
    evaluationStrings[i] = new char[strlen(F.sNames[i])+1];
    COPYSTR(evaluationStrings[i], strlen(F.sNames[i]) + 1,F.sNames[i]);
    evaluationPriority[i] = -1;
    }
//
//  Create evaluation strings by evaluating the expression
//  symbolically.
//
    long j;

    long    functionIndex;
    long         argIndex;
    long         argCount;

    long    stringSize;
    long    resultIndex;

    long    leftPriority;
    long    rghtPriority;
    long    centPriority;

    int executionIndex = 0;
    while(executionIndex < F.executionArraySize)
    {

    functionIndex = F.executionArray[executionIndex]; executionIndex++;
    argCount      = F.executionArray[executionIndex]; executionIndex++;
    resultIndex   = F.executionArray[executionIndex+(argCount-1)];

    //cout << "Fix this problem later ZZZZ " << endl;
    //cout << functionIndex << endl;
    //cout << argCount      << endl;
    //cout << resultIndex   << endl;
    //cout << endl;

    centPriority = L.Priority[functionIndex];
//
//  ++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  Compute the string size for the function evaluation
//
    stringSize = 0;
    for(j =0; j < argCount-1; j++)
    {
     stringSize += (long)strlen(evaluationStrings[F.executionArray[executionIndex+j]]);
    }

    stringSize  += (long)strlen(L.Symbols[functionIndex]);
    stringSize  += 4; //for ()'s
//
//  Special case binary operators
//
    if(!strcmp(L.Symbols[functionIndex],"atan2")) stringSize++; // for ,
    if(!strcmp(L.Symbols[functionIndex],"pow"))   stringSize++; // for ,
    if(!strcmp(L.Symbols[functionIndex],"fmod"))  stringSize++; // for ,
//
//  +++++++++++++++++++++++++++++++++++++++++++++++++
//
    if(argCount == 2)
    {
    argIndex = F.executionArray[executionIndex];

    if
    (
       ((argIndex < F.symbolCount)&&
       (argIndex >= F.constantCount + F.variableCount))
       &&
       (
       (!strcmp(L.Symbols[functionIndex],"+"))||
       (!strcmp(L.Symbols[functionIndex],"-"))||
       (!strcmp(L.Symbols[functionIndex],"*"))||
       (!strcmp(L.Symbols[functionIndex],"/"))
       )
    )
    {
    sbuf << L.Symbols[functionIndex]
    << evaluationStrings[F.executionArray[executionIndex]] << ends;
    }
    else
    {
    sbuf << L.Symbols[functionIndex]
    << "(" << evaluationStrings[F.executionArray[executionIndex]] << ")" << ends;
    }

    }

    else if(argCount == 3)
    {
    if((!strcmp(L.Symbols[functionIndex],"atan2"))||
       (!strcmp(L.Symbols[functionIndex],"pow"))||
       (!strcmp(L.Symbols[functionIndex],"fmod")))
    {
    sbuf << L.Symbols[functionIndex] << "("
    << evaluationStrings[F.executionArray[executionIndex]] << ","
    << evaluationStrings[F.executionArray[executionIndex+1]] << ")" << ends;
    }
    else
    {
        if(!strcmp(L.Symbols[functionIndex],"^")) // convert ^ to pow symbol

        {
            sbuf << "pow("
            << evaluationStrings[F.executionArray[executionIndex]] << ","
            << evaluationStrings[F.executionArray[executionIndex+1]] << ")";
        }
        else
        {

        leftPriority = evaluationPriority[F.executionArray[executionIndex]];
        rghtPriority = evaluationPriority[F.executionArray[executionIndex+1]];
        centPriority = L.Priority[functionIndex];

        if(leftPriority >= centPriority)
        {
        sbuf << "(" << evaluationStrings[F.executionArray[executionIndex]] << ")";
        }
        else
        {
        sbuf << evaluationStrings[F.executionArray[executionIndex]];
        }

        sbuf << L.Symbols[functionIndex];

        if(rghtPriority >= centPriority)
        {
            sbuf << "(" << evaluationStrings[F.executionArray[executionIndex+1]] << ")";
        }
        else
        {
            sbuf << evaluationStrings[F.executionArray[executionIndex+1]];
        }
        }
        sbuf << ends;
    }
    }
    evaluationStrings[resultIndex] = new char[strlen((sbuf.str()).c_str()) + 1];
    COPYSTR(evaluationStrings[resultIndex],strlen((sbuf.str()).c_str()) + 1,(sbuf.str()).c_str());

    sbuf.str("");
    evaluationPriority[resultIndex] = centPriority;

    executionIndex += argCount;
    }
//
//  Diagnostic Output
//
/*
    cout << endl << "Evaluation Array " << endl;

    for(i=0; i < F.evaluationDataSize; i++)
    {
       cout << evaluationStrings[i] << endl;
    }
*/

    }


};

}

#endif

