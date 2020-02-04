//
//##################################################################
//  		           SCC_SymFun.h 
//##################################################################
//
//	Chris Anderson 9/10/96 - 2020  (C) UCLA
//
//  Version 02/04/2020 : 
//  Based upon 2016 version.
//  Restricted constructors to use std::strings
//  Fixed bug in resetting of symbolic constant values
//  Added samples for initialization with exception handling
//  Renamed header file to SCC_SymFun.h to distinguish 
//  from earlier incompatible versions. 
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


#include <iostream>
#include <functional>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
using namespace std;


#ifndef SYMBOLIC_FUNCTION_
#define SYMBOLIC_FUNCTION_

#include "SCC_OperatorLib.h"
#include "SCC_RealOperatorLib.h"
#include "SCC_ExpressionTransform.h"
#include "SCC_SymFunException.h"

//
// strcpy_s is not implemented as part of C++11 (arrgh) so this macro
// inserts strcpy calls.
//

#ifdef _MSC_VER
#define COPYSTR(src,count,dst) strcpy_s(src,count,dst)
#else
#define COPYSTR(src,count,dst) strcpy(src,dst)
#endif

namespace SCC
{

/*!
 \class SCC::SymFun
 \brief A class whose instances evaluate double valued functions of double variables specified symbolically

 Instances of SCC::SymFun provide means of evaluating functions of double variables that
 are specified symbolically, i.e. as string. The names of the
 variables and symbolic constants used to express the function symbolically are specified by the
 programmer. The operations allowed in the function specification include the standard algebraic operations as well as
 many of the standard functions available for double values in the C++ standard library.

 The operators allowed are

 {"+", "-", "+", "-", "*", "/", "^", "sin", "cos", "tan","asin","acos","atan","atan2", "sinh","cosh","tanh",
	    "ceil","exp","abs","floor","fmod","log","log10","sqrt","pow"};

The "^" is interpreted as the exponentiation operator, i.e. x^2 is x squared.

Required version of C++ : >=  C++11

\headerfile SCC_SymFun.h "SCC_SymFun.h"
*/

class  SymFun
{

public  :

    //
    //###############################################
    //   Constructors and initializers
    //###############################################
    //

    /**
      Null constructor. The instance created must be initialized with
      one of the initialize(...) member functions.
    */

    SymFun()
    {
    	 bool nullInstanceFlag = true;
    	 destroy(nullInstanceFlag);
    	 initialize();
    }

    /**
       Copy constructor. Creates a duplicate of F
     */
     SymFun(const SymFun& F)
     {
     	bool nullInstanceFlag = true;
     	destroy(nullInstanceFlag);
     //
     //  Allocate Storage and Initalize
     //
         int i;

         if(F.constructorString != 0)
         {
         constructorString = new char[strlen(F.constructorString) + 1];
         COPYSTR(constructorString, strlen(F.constructorString) + 1,F.constructorString);
         }

         if(F.variableCount != 0)
         {
         variableNames  = new char*[F.variableCount];
         variableCount  = F.variableCount;

         for(i = 0; i < F.variableCount; i++)
         {
         variableNames[i] = new char[strlen(F.variableNames[i]) + 1];
         COPYSTR(variableNames[i], strlen(F.variableNames[i]) + 1,F.variableNames[i]);
         }
         }

         if(F.constantCount != 0)
         {
         constantNames  = new char*[F.constantCount];
         constantCount  = F.constantCount;
         constantValues = new double[F.constantCount];

         for(i = 0; i < F.constantCount; i++)
         {
         constantNames[i] = new char[strlen(F.constantNames[i]) + 1];
         COPYSTR(constantNames[i], strlen(F.constantNames[i]) + 1,F.constantNames[i]);
         constantValues[i] = F.constantValues[i];
         }
         }

         evaluationDataSize = F.evaluationDataSize;

         if(F.evaluationData != 0)
         {
         evaluationData     = new double[evaluationDataSize];
         for(i=0; i< evaluationDataSize; i++)
         {
         evaluationData[i] = F.evaluationData[i];
         }}

         executionArraySize = F.executionArraySize;

         if(F.executionArray != 0)
         {
         executionArray     = new long[executionArraySize];
         for(i=0; i< executionArraySize; i++)
         {
         executionArray[i] = F.executionArray[i];
         }}

         symbolCount = F.symbolCount;
         sNames      = new char*[symbolCount];
         for(i=0; i< symbolCount; i++)
         {
         sNames[i] = new char[strlen(F.sNames[i])+1];
         COPYSTR(sNames[i], strlen(F.sNames[i]) + 1,F.sNames[i]);
         }

         LibFunctions       = RealOpLib.FunctionArray;
     }


    /**
    Creates a SCC::SymFun instance to be a function in one variable, x, where the function is
    specified by the string S. If the construction process fails, program execution stops and an error message is output.
    See initialize(...) member functions for creation with exception handling.

    @arg S: std::string in the variable x that specifies the function.

    <HR>
    Sample specification and use of a function expressed in a variable x:
	\code

    std::string S = "2.0*x+ sin(x)";                      // specify function

    SCC::SymFun F(S);                                     // create instance

    cout << "x^2 evaluated at 2.0 = " << F(2.0) << endl;  // evaluate and output
    \endcode
    */

    SymFun(const string& S)
    {
    	 bool nullInstanceFlag = true;
    	 destroy(nullInstanceFlag);
    	 initialize(S);
    }

    /**
       Creates a SCC::SymFun instance from the initialization string S in which variables are specified in the std::vector of strings V.
       If the construction process fails, program execution stops and an error message is output.
       See initialize(...) member functions for creation with exception handling.

       @arg V      : vector<string> specifying independent variable names
       @arg S      : std::string specifying the function

       <HR>
       Sample specification and use of a function of two variables x and y:

       \code
       vector<string> V     = {"x","y"};         // x,y  = independent variable names
       string S             = "x^2 + 2*y";       // specify a function

       SCC::SymFun F(V,S);                       // create instance

       cout << F(2.0,3.0) << endl;               // evaluate and output result at (x,y) = (2.0,3.0)
       \endcode
    */

    SymFun(const vector<string>& V, const string& S)
    {
    	bool nullInstanceFlag = true;
    	destroy(nullInstanceFlag);

    	initialize(V,S);
    }

    /**
    Creates a SymFun instance from the initialization string S. Variables names are specified by the elements of V
    and symbolic constant names are specified in C. The initial values of the symbolic constants are specified
    in the vector of double values Cvalues.

    If the construction process fails, program execution stops and an error message is output.
    See initialize(...) member functions for creation with exception handling.

    @arg V       : vector<string> specifying independent variable names
    @arg S       : std::string specifying the function
    @arg C       : vector<string> specifying symbolic constant names
    @arg Cvalues : vector<double> specifying symbolic constants values

    <HR>
    Sample demonstrating creation and usage of an instance that implements a*x^2 + b*x + c.
    \code
	//
	//  Create a SCC::SymFun that implements a*x^2 + b*x + c;
	//  a, b, c being symbolic constants.
	//
	    vector<string>        V = {"x"};                 // specify variable name
	    vector<string>        C = {"a","b","c"};         // specify constant names
	    vector<double> Cvalues  = {1.0, 2.0, 1.0};       // initial values of a,b,c

	    string S = "a*x^2 + b*x + c";                    // initialization string

	    SCC::SymFun F(V,C,Cvalues, S);
	    cout << F << endl << endl;                       // print out function

	    cout << "The value of the function at x = 1.0 is "
	         << F(1.0) << endl << endl;

	    F.setConstantValue("a",1000.0);                 // reset the symbolic constants a and c
	    F.setConstantValue("c",2000.0);

	    cout << F << endl << endl;                       // print out function

	    cout << "The value of the function at x = 1.0 is  "
	         << F(1.0) << endl;
    \endcode
    */

    SymFun(const vector<string>& V, const vector<string>& C, const vector<double>& Cvalues, const string& S)
    {
    	bool nullInstanceFlag = true;
    	destroy(nullInstanceFlag);

    	initialize(V,C,Cvalues,S);
    }

    /**
      Initializes a SCC::SymFun instance to a null instance.
    */
    int initialize()
    {
        destroy();
        return 0;
    }

    /**
      Initializes the SCC::SymFun instance with F (creates a duplicate)
    */

    int initialize(const SymFun& F)
    {
        destroy();
    //
    //  Allocate Storage and Initalize
    //
        int i;
        if(F.constructorString != 0)
        {
        constructorString = new char[strlen(F.constructorString) + 1];
        COPYSTR(constructorString, strlen(F.constructorString) + 1,F.constructorString);
        }

        if(F.variableCount != 0)
        {
        variableNames  = new char*[F.variableCount];
        variableCount  = F.variableCount;

        for(i = 0; i < F.variableCount; i++)
        {
        variableNames[i] = new char[strlen(F.variableNames[i]) + 1];
        COPYSTR(variableNames[i], strlen(F.variableNames[i]) + 1,F.variableNames[i]);
        }
        }

        if(F.constantCount != 0)
        {
        constantNames  = new char*[F.constantCount];
        constantCount  = F.constantCount;
        constantValues = new double[F.constantCount];

        for(i = 0; i < F.constantCount; i++)
        {
        constantNames[i] = new char[strlen(F.constantNames[i]) + 1];
        COPYSTR(constantNames[i], strlen(F.constantNames[i]) + 1,F.constantNames[i]);
        constantValues[i] = F.constantValues[i];
        }
        }

        evaluationDataSize = F.evaluationDataSize;

        if(F.evaluationData != 0)
        {
        evaluationData     = new double[evaluationDataSize];
        for(i=0; i< evaluationDataSize; i++)
        {
        evaluationData[i] = F.evaluationData[i];
        }}

        executionArraySize = F.executionArraySize;

        if(F.executionArray != 0)
        {
        executionArray     = new long[executionArraySize];
        for(i=0; i< executionArraySize; i++)
        {
        executionArray[i] = F.executionArray[i];
        }}

        symbolCount = F.symbolCount;
        sNames      = new char*[symbolCount];
        for(i=0; i< symbolCount; i++)
        {
        sNames[i] = new char[strlen(F.sNames[i])+1];
        COPYSTR(sNames[i], strlen(F.sNames[i]) + 1,F.sNames[i]);
        }

        LibFunctions       = RealOpLib.FunctionArray;
        return 0;
    }

    /**
    Initializes an existing SCC::SymFun instance to be a function in one variable, x, where the function is
    specified by the string S. If the initialization is not executed within a try/catch block
    and the construction process fails, then program execution stops and an error message is output.
    If executed within a try/catch block and the construction process fails, an exception is generated
    and appropriate exception handling can be implemented.


    @arg S: std::string in the variable x that specifies the function.

    <HR>
    Sample demonstrating initialization within a try/catch block.
    \code
    string S             = "x^2";            // specifying a function in x

    SCC::SymFun F;                           // Create instance

    try
    {
    F.initialize(S);                         // initialize instance
    }
    catch (const SCC::SymFunException& e)    // exception handling
    {
          cerr << e.what() << endl;;
          cerr << "XXXX Execution Terminated XXXXX" << endl;
          exit(1);
    }


    cout << "x^2 evaluated at 2.0 = " << F(2.0) << endl;  // evaluate and output
    \endcode
    */

    int initialize(const string& S)
    {
    	bool nullInstanceFlag = true;
    	destroy(nullInstanceFlag);

    	return initialize(S.c_str());
    }


    /**
       Initializes an existing SCC::SymFun instance to be the function specified by the string S in which variables are
       specified in the std::vector of strings V.

        If the initialization is not executed within a try/catch block
    and the construction process fails, then program execution stops and an error message is output.
    If executed within a try/catch block and the construction process fails, an exception is generated
    and appropriate exception handling can be implemented.

       @arg V      : vector<string> specifying independent variable names
       @arg S      : std::string specifying the function

       <HR>
       Sample initialization and use of a function of two variables x and y with
       exception handling.

       \code
        vector<string> V     = {"x","y"};            // x,y  = independent variable names
        string S             = "x^2 + 2*y";          // specify a function

        SCC::SymFun F;                               // Create instance

        try
        {
           F.initialize(V,S);                        // initialize instance
        }
        catch (const SCC::SymFunException& e)        // exception handling
        {
          cerr << e.what() << endl;;
          cerr << "XXXX Execution Terminated XXXXX" << endl;
          exit(1);
        }
        cout << F(2.0,3.0) << endl;               // evaluate and output result at (x,y) = (2.0,3.0)
       \endcode
    */
    int initialize(const vector<string>& V, const string& S)
    {
    	bool nullInstanceFlag = true;
    	destroy(nullInstanceFlag);

    	int Vcount = V.size();
    	vector<const char*> Varray(Vcount);
    	for(int i = 0; i < Vcount; i++)
    	{
    		Varray[i] = V[i].c_str();
    	}
    	return initialize(&Varray[0],Vcount,S.c_str());
    }


    /**
      Initializes an existing SCC::SymFun instance to be the function
      specified in the  initialization string S. Variables names are specified by the elements of V and
      symbolic constant names are specified in C. The initial values of the symbolic constants are specified
      in the vector of doubles Cvalues.

      If the initialization is not executed within a try/catch block
    and the construction process fails, then program execution stops and an error message is output.
    If executed within a try/catch block and the construction process fails, an exception is generated
    and appropriate exception handling can be implemented.

      @arg V       : vector<string> specifying independent variable names
      @arg S       : std::string specifying the function
      @arg C       : vector<string> specifying symbolic constant names
      @arg Cvalues : vector<double> specifying symbolic constants values

      <HR>
      Sample demonstrating initialization and usage of an instance that implements a*x^2 + b*x + c.
      \code
      //
      //  Create a SCC::SymFun that implements a*x^2 + b*x + c;
      //  a, b, c being symbolic constants.
      //
          vector<string>  V       = {"x"};                    // specify variable name
          vector<string>  C       = {"a","b","c"};            // specify constant names
          vector<double> Cvalues  = {1.0, 2.0, 1.0};          // initial values of a,b,c

          string S = "a*x^2 + b*x + c";                       // initialization string

          SCC::SymFun F;

          try
          {                                                   // Create instance
             F.initialize(V, C, Cvalues, S);                  // Initialize
          }
          catch (const SCC::SymFunException& e)               // exception handling
          {
          cerr << e.what() << endl;;
          cerr << "XXXX Execution Terminated XXXXX" << endl;
          exit(1);
          }

          cout << F << endl << endl;                           // print out function

          cout << "The value of the function at x = 1.0 is "
               << F(1.0) << endl << endl;

          F.setConstantValue("a",1000.0);                      // reset the symbolic constants a and c
          F.setConstantValue("c",2000.0);

          cout << F << endl << endl;                           // print out function

          cout << "The value of the function at x = 1.0 is  "
               << F(1.0) << endl;
      \endcode
     */
    int initialize(const vector<string>& V, const vector<string>& C, const vector<double>& Cvalues, const string& S)
    {
    	bool nullInstanceFlag = true;
    	destroy(nullInstanceFlag);

    	int Vcount = V.size();
    	vector<const char*> Varray(Vcount);
    	for(int i = 0; i < Vcount; i++)
    	{
    		Varray[i] = V[i].c_str();
    	}

    	int Ccount = C.size();
    	vector<const char*> Carray(Ccount);
    	for(int i = 0; i < Ccount; i++)
    	{
    		Carray[i] = C[i].c_str();
    	}
    	return initialize(&Varray[0],Vcount,&Carray[0],Ccount,&Cvalues[0],S.c_str());
    }



    //
    //##################################################################
    //                     OPERATORS
    //##################################################################
    //

    /** Assignment operator. The instance is initialized using F. The
        data associated with the original instance is destroyed.
    */

    void operator=(const SymFun& F)
    {
        initialize(F);
    }


    //
    //##################################################################
    //                 EVALUATION OPERATORS
    //##################################################################
    //

    /**
         Returns the value of the SymFun using the variable value x.
         If an incorrect number of arguments specified and the program is compiled with the
         pre-processor directive  _DEBUG defined, then an
         error message is generated and execution halts.
    */


    double operator()(double x) const
    {
            if(variableCount != 1) argError(1, variableCount);
            evaluationData[0] = x;
            return evaluate();
    }

    /**
     Returns the value of the SymFun using the variable value (x1,x2).
     If an incorrect number of arguments specified and the program is compiled with the
         pre-processor directive  _DEBUG defined, then an
         error message is generated and execution halts.
    */
    double operator()(double x1, double x2) const
    {
        if(variableCount != 2) argError(2, variableCount);

        evaluationData[0] = x1;
        evaluationData[1] = x2;
        return evaluate();

    }

    /**
     Returns the value of the SymFun using the variable value (x1,x2,x3).
     If an incorrect number of arguments specified and the program is compiled with the
         pre-processor directive  _DEBUG defined, then an
         error message is generated and execution halts.
    */

    double operator()(double x1, double x2, double x3) const
    {
        if(variableCount != 3) argError(3, variableCount);

        evaluationData[0] = x1;
        evaluationData[1] = x2;
        evaluationData[2] = x3;
        return evaluate();
    }

    /**
     Returns the value of the SymFun using the variable value (x1,x2,x3,x4).
     If an incorrect number of arguments specified and the program is compiled with the
         pre-processor directive  _DEBUG defined, then an
         error message is generated and execution halts.
    */

    double operator()(double x1, double x2, double x3, double x4) const
    {
         if(variableCount != 4) argError(4, variableCount);

        evaluationData[0] = x1;
        evaluationData[1] = x2;
        evaluationData[2] = x3;
        evaluationData[3] = x4;
        return evaluate();
    }

    /**
     Returns the value of the SymFun using the n variable values in the vector of doubles x.

     If an incorrect number of arguments specified and the program is compiled with the pre-processor directive  _DEBUG defined, then an
     error message is generated and execution halts.

     @arg x vector<double> array of values
    */
    double operator()(const vector<double>& x) const
    {
    	int n = x.size();
        if(variableCount != n) argError(n, variableCount);

        int i;
        for(i = 0; i < n; i++) evaluationData[i] = x[i];
        return evaluate();
    }

    /**
     Returns the value of the SymFun using the
     n variable values in the double array x.

     If an incorrect number of arguments specified and the program is compiled with the
         pre-processor directive  _DEBUG defined, then an
         error message is generated and execution halts.


     @arg x poiner to a double array
     @arg n the number of elements in x
    */
    double operator()(double*x, int n) const
    {
         if(variableCount != n) argError(n, variableCount);

        int i;
        for(i = 0; i < n; i++) evaluationData[i] = x[i];
        return evaluate();
    }


    //###############################################
    //                MUTATORS
    //###############################################


    /**
     Returns the string used to initialize the
     SCC::SymFun instance.
     */

    string getConstructorString() const
    {
        return string(constructorString);
    }

    /**
     Returns the number of variables associated with the
     SCC::SymFun instance.
    */
    int getVariableCount() const
    {
          return variableCount;
    }


    /**
    Returns the name of the ith variable associated with the
    SCC::SymFun instance.
    */

    string getVariableName(int i) const
    {
          return string(variableNames[i]);
    }

    /**
    Returns the number of symbolic constants associated with the
     SCC::SymFun instance.
     */

    int getConstantCount() const
    {
          return constantCount;
    }


    /**
     Returns the name of the ith symbolic constant associated
     with the SCC::SymFun instance.
    */

    string  getConstantName(int i) const
    {
          return string(constantNames[i]);
    }

    /**
     Returns the value of the ith symbolic constant
     associated with the  SCC::SymFun instance.
    */
    double  getConstantValue(int i) const
    {
          return constantValues[i];
    }

    /**
     Returns the value of the specified symbolic constant
     associated with the SCC::SymFun instance.

     @arg S: Character string with name of the symbolic constant.
    */

    double getConstantValue(const string& S) const
    {
        int i;
        for(i =0; i < constantCount; i++)
        {
        if(strcmp(S.c_str(),constantNames[i]) == 0) return constantValues[i];
        }



        return 0.0;
    }


    /**
     Sets the value of the ith symbolic constant
     associated with the SCC::SymFun instance.

     @arg S: std::string with name of the symbolic constant.
     @arg x: Double value specifying the new value of the constant.
    */

    void setConstantValue(const string& C,double x)
    {
           int i;
           for(i =0; i < constantCount; i++)
           {
           if(strcmp(C.c_str(),constantNames[i]) == 0) constantValues[i] = x;
           }

           // Reset evaluation data to reflect change in values

           for(int i = variableCount, j = 0; i < variableCount + constantCount; ++i,++j)
           {
        	   evaluationData[i] = constantValues[j];
           }
    }

    /**
     Returns the names of the variables in a vector of strings.
     */

    vector<string> getVariableNames() const
    {
    	vector<string> variables(variableCount);
    	for(int i = 0; i < variableCount; ++i)
    	{
    		variables[i] = string(variableNames[i]);
    	}
        return variables;
    }

    /**
     Returns the names of the constants in a vector of strings.
     */
    vector<string>  getConstantNames() const
    {
    	vector<string> constants(constantCount);
    	for(int i = 0; i < constantCount; ++i)
    	{
    		constants[i] = string(constantNames[i]);
    	}

        return constants;
    }


    /**
     Returns the names of the constant values in a vector of strings.
     */

    vector<double> getConstantValues() const
    {

    	vector<double> constantVals(constantCount);
    	for(int i = 0; i < constantCount; ++i)
    	{
    		constantVals[i] = constantValues[i];
    	}
        return  constantVals;
    }


    //###############################################
    //       Anonymous Function Interface
    //###############################################


    /**
     Returns a std::function instance of a double function of a single double
     that is bound to the evaluation operator of *this.
    */

	std::function<double(double)> getEvaluationPtr1d() const
	{
	std::function<double(double)> F = [this](double x1) {return this->operator()(x1);};
	return std::move(F);
	};


    /**
     Returns a std::function instance of a double function of a two double values
     that is bound to the evaluation operator of *this.
    */
	std::function<double(double,double)> getEvaluationPtr2d() const
	{
	std::function<double(double,double)> F = [this](double x1,double x2) {return this->operator()(x1,x2);};
	return std::move(F);
	};



    /**
     Returns a std::function instance of a double function of a threedouble values
     that is bound to the evaluation operator of *this.
    */
	std::function<double(double,double,double)> getEvaluationPtr3d() const
	{
	std::function<double(double,double,double)> F = [this](double x1,double x2,double x3) {return this->operator()(x1,x2,x3);};
	return std::move(F);
	};



    /**
     Returns a std::function instance of a double function of a four double values
     that is bound to the evaluation operator of *this.
    */

	std::function<double(double,double,double,double)> getEvaluationPtr4d() const
	{
	std::function<double(double,double,double,double)> F = [this](double x1,double x2,double x3,double x4) {return this->operator()(x1,x2,x3,x4);};
	return std::move(F);
	};


    /**
     Returns a std::function instance of a double function of a vector of double values
     that is bound to the evaluation operator of *this.
    */

	std::function<double(vector<double>&)> getEvaluationPtrNd() const
	{
	std::function<double(vector<double>&)> F = [this](vector<double>& x) {return this->operator()(x);};
	return std::move(F);
	};

    //
    //##################################################################
    //                      DESTRUCTORS
    //##################################################################
    //

    ~SymFun()
    {
        destroy();
    }



    //
    //##################################################################
    //                      OUTPUT
    //##################################################################
    //
    /**
     Outputs the initialization string, the variable names, the symbolic
     constant names and the symbolic constant values.
    */
    friend ostream&  operator <<(ostream& out_stream, const SCC::SymFun& F)
    {
        int i;
         out_stream << F.getConstructorString() << endl;
        out_stream << " Variables : " << endl;

        for(i = 0; i < F.getVariableCount(); i++)
        {
        out_stream << F.getVariableName(i) << endl;
        }

        if(F.getConstantCount() > 0)
        {
        out_stream << " Constant Values : " << endl;
        for(i = 0; i < F.getConstantCount(); i++)
        {
        out_stream << F.getConstantName(i) << "  " << F.getConstantValue(i) << endl;
        }}

    return out_stream;

    }




    friend class SymFunUtility;
//
//##################################################################
//                      PROTECTED MEMBER FUNCTIONS
//##################################################################
//


protected:

    void argError(int argC, int vCount) const
    {
    	#ifdef _DEBUG
        cerr << " Incorrect Number of Arguments in SymFun " << endl;
        cerr << " Called with " << argC << " arguments, expecting " << vCount;
      	cerr << " Fatal Error : Program Stopped " << endl;
    	exit(1);
        #endif
    }



    SymFun(char const* S)
    {
        const char* V []      = {"x"};
        int Vcount            = 1;
        const char** C        = 0;
        int Ccount            = 0;
        double* Cvalues       = 0;

        create(V,Vcount,C,Ccount, Cvalues, S);
    }


    SymFun(const char** V, int Vcount, const char* S)
    {

        const char** C        = 0;
        int Ccount            = 0;
        double* Cvalues       = 0;
        create(V,Vcount,C,Ccount, Cvalues, S);
    }

    SymFun(const char** V, int Vcount, const char** C,
    int Ccount, double const* Cvalues, char const* S)
    {
        create(V,Vcount,C,Ccount, Cvalues, S);
    }

    int create(const char** V, int Vcount, const char** C,
    int Ccount, double const* Cvalues, char const* S)
    {
    //
    //  Allocate Storage and Initalize
    //
        constructorString = new char[strlen(S) + 1];
        COPYSTR(constructorString, strlen(S) + 1,S);

        variableNames  = new char*[Vcount];
        variableCount  = Vcount;

        int i;
        for(i = 0; i < Vcount; i++)
        {
        variableNames[i] = new char[strlen(V[i]) + 1];
        COPYSTR(variableNames[i], strlen(V[i]) + 1,V[i]);
        }

        constantNames  = new char*[Ccount];
        constantCount  = Ccount;
        constantValues = new double[Ccount];

        for(i = 0; i < Ccount; i++)
        {
        constantNames[i] = new char[strlen(C[i]) + 1];
        COPYSTR(constantNames[i], strlen(C[i]) + 1,C[i]);
        constantValues[i] = Cvalues[i];
        }

        RealOperatorLib L;
          ExpressionTransform T;
        int expReturn;

        expReturn =  T.initialize(variableNames, variableCount, constantNames,
                     constantCount,constructorString,&L);
        if(expReturn != 0) {destroy(); return 1;}

        evaluationDataSize = T.getEvaluationDataSize();
        evaluationData     = new double[evaluationDataSize];

        executionArraySize = T.getExecutionArraySize();
        executionArray     = new long[executionArraySize];

        initializeEvaluationData(T);
        initializeExecutionArray(T);
    //
    //  Save symbols
    //
        symbolCount    = T.getSymbolCount();
        char** TsNames = T.getSymbolNamesPtr();
        sNames      = new char*[symbolCount];
        for(i=0; i< symbolCount; i++)
        {
        sNames[i] = new char[strlen(TsNames[i])+1];
        COPYSTR(sNames[i], strlen(TsNames[i]) + 1,TsNames[i]);
        }

        LibFunctions       = RealOpLib.FunctionArray;
        return 0;
    }



    void destroy(bool nullInstanceFlag = false)
    {
    	if(nullInstanceFlag)
    	{
        constructorString = 0;

        variableNames     = 0;
        variableCount     = 0;

        constantNames     = 0;
        constantCount     = 0;
        constantValues    = 0;

        symbolCount       = 0;
        sNames            = 0;

        evaluationData     = 0;
        evaluationDataSize = 0;

        executionArray     = 0;
        executionArraySize = 0;

        LibFunctions       = 0;
        return;
    	}

        int i;
        if(constructorString != 0) delete [] constructorString;

        if(variableNames != 0)
        {
        for(i =0; i < variableCount; i++) delete [] variableNames[i];
        delete [] variableNames;
        }

        if(constantNames != 0)
        {
        for(i =0; i < constantCount; i++) delete [] constantNames[i];
        delete [] constantNames;
        delete [] constantValues;
        }

        if(evaluationData != 0) delete [] evaluationData;
        if(executionArray != 0) delete [] executionArray;

        if(sNames         != 0)
        {
        for(i=0; i< symbolCount; i++) if(sNames[i] != 0) delete [] sNames[i];
        delete [] sNames;
        }

        constructorString = 0;

        variableNames     = 0;
        variableCount     = 0;

        constantNames     = 0;
        constantCount     = 0;
        constantValues    = 0;

        symbolCount       = 0;
        sNames            = 0;

        evaluationData     = 0;
        evaluationDataSize = 0;

        executionArray     = 0;
        executionArraySize = 0;

        LibFunctions       = 0;
    }

    //
    //##################################################################
    //                      INITIALIZATION
    //##################################################################
    //

    /**
    Initialize an SymFun instance to one
    of a single variable, x, where the function is
    specified by the null terminated string S. If the initialization fails,
    error diagnostics are output to the standard error stream
    (cerr) and the program returns an error value.

    @arg S: Null terminated character string in the variable x that
    specifies the function.

    @returns 0 (= no error) 1 (= error).
    */

    int initialize(char const* S)
    {
        destroy();
        const char*V []  = {"x"};
        int Vcount = 1;

        const char** C        = 0;
        int Ccount            = 0;
        const double* Cvalues = 0;

        int  cReturn;
        cReturn = create(V,Vcount,C,Ccount, Cvalues, S);
        if(cReturn != 0) return cReturn;
        return 0;
    }
    /**
    Initializes a SymFun instance to one
    of Vcount variables from the initialization string S.
    S is a null terminated character string. If the initialization fails,
    error diagnostics are output to the standard error stream
    (cerr) and the program returns an error value.

    @arg V      : Array of null terminated strings specifying independent variable names
    @arg Vcount : The number of independent variables
    @arg S      : Null terminated character string specifying the function

    @returns 0 (= no error) 1 (= error).

    Sample specification and use of a function of two variables x and y:

    \code
    SymFun F;              // Create null instance
    int Vcount = 2;                     // number of independent variables
    char*V []  = {"x","y"};             // x,y  = independent variable names
    char*S     = "x^2 + 2*y";           // specify a function

    int ierr = F.initialize(V,Vcount,S);// initialize
    if(ierr != 0)
    {
    cerr << "Initialization of SymFun Failed" << endl;
    exit(1);
    }


    cout << F(2.0,3.0) << endl;        // evaluate and output result at (x,y) = (2.0,3.0)
    \endcode
    */


    int initialize(const char** V, int Vcount, char const* S)
    {
        destroy();
        const char** C  = 0;
        int Ccount      = 0;
        double* Cvalues = 0;
        int  cReturn;
        cReturn = create(V,Vcount,C,Ccount, Cvalues, S);
        if(cReturn != 0) return cReturn;
        return 0;
    }
    /**
    Initializes a SymFun instance to one of Vcount variables and
    Ccount symbolic constants from the initialization string S.
    If the initialization fails, error diagnostics are output to the
    standard error stream (cerr) and the program returns an error value.

    @arg V      : Array of null terminated strings specifying independent variable names
    @arg Vcount : The number of independent variables
    @arg S      : Null terminated character string specifying the function
    @arg V      : Array of null terminated strings specifying symbolic constant names
    @arg Ccount : The number of symbolic constants
    @arg Cvalues: The values of the symbolic constants
    @arg S      : Null terminated character string specifying the function

    Sample:
    \code
    //
    //  Initializes a CAMsymbolic function instance to one that implements
    //  a*x^2 + b*x + c; a, b, c being symbolic constants.
    //
        SymFun f;                       // create instance

        int Vcount       = 1;                        // number of independent variables
        char*V []        = {"x"};                    // specify variable name
        int Ccount       = 3;                        // number of symbolic constants

        char*C []        = {"a","b","c"};            // specify constant names

        double Cvalues[] = {1.0, 2.0, 1.0};          // initial values of a,b,c

        char* S = "a*x^2 + b*x + c";                 // initialization string

        int ierr = f.initialize(V,Vcount,C,Ccount,Cvalues, S);
        if(ierr != 0)
        {
        cerr << "Initialization of SymFun Failed" << endl;
        exit(1);
        }

        cout << f << endl << endl;                    // print out function

        cout << "The value of the function at x = 1.0 is "
             << f(1.0) << endl << endl;

        f.setConstantValue("a",2.0);                  // reset the symbolic constant
                                                      // a to have the value 2.0
        cout << f << endl << endl;                    // print out function

        cout << "The value of the function at x = 1.0 is  "
             << f(1.0) << endl;
    \endcode
    */
    int  initialize(const char** V, int Vcount, const char** C,
    int Ccount, double const* Cvalues, char const* S)
    {
        destroy();
        int  cReturn;
        cReturn = create(V,Vcount,C,Ccount, Cvalues, S);
        if(cReturn != 0) return cReturn;
        return 0;
    }


    long getSymbolCount() const
    {
        return symbolCount;
    }


    char** getVariableNamePtr() const
    {
        return variableNames;
    }

    char** getConstantNamePtr() const
    {
        return constantNames;
    }

    double* getConstantValuePtr() const
    {
        return  constantValues;
    }
    void  initializeEvaluationData(const ExpressionTransform& T)
    {
        long symbolCount = T.getSymbolCount();
        char**sNames     = T.getSymbolNamesPtr();
        int i;  int j;

        if(variableCount != 0)
        for(i = 0; i < variableCount; i++)
        evaluationData[i] = 0.0;

        for(i = variableCount,j = 0; i < variableCount + constantCount; i++,j++)
        evaluationData[i] = constantValues[j];

        for(i = variableCount + constantCount; i < symbolCount; i++)
        {evaluationData[i] = atof(sNames[i]);}

    }

    void  initializeExecutionArray(const ExpressionTransform& T)
    {
        long* executionArrayPtr = T.getExecutionArrayPtr();
        int i;
        for(i=0; i < executionArraySize; i++)
        {executionArray[i] = executionArrayPtr[i];
        }
    }

    void  setConstantEvaluationData()
    {
        int i; int j;

        for(i = variableCount,j = 0; i < variableCount + constantCount; i++,j++)
        evaluationData[i] = constantValues[j];
    }


    double evaluate() const
    {
        int j;
        double* argData[10];   // limit of 10 args for now

        int functionIndex;
        int argCount;

        int executionIndex = 0;
        while(executionIndex < executionArraySize)
        {
        functionIndex = executionArray[executionIndex]; executionIndex++;
        argCount      = executionArray[executionIndex]; executionIndex++;
        for(j =0; j < argCount; j++)
        {
        argData[j] = &(evaluationData[executionArray[executionIndex]]);
        executionIndex++;
        }
        ((void(*)(double**))LibFunctions[functionIndex])(argData);
        }

        return evaluationData[evaluationDataSize - 1];
    }


    char*       constructorString;

    char**      variableNames;
    int         variableCount;

    char**      constantNames;
    int         constantCount;
    double*     constantValues;

    long        symbolCount;      // total number of variables, symbolic constants, 
                                  // and numeric constants 

    long*       executionArray;
    long        executionArraySize;

    double*     evaluationData;
    long        evaluationDataSize;

    void** LibFunctions;

    char   **sNames;

    /* void createCcode(); // experimenting 02/19/07 */

    RealOperatorLib RealOpLib;
};
}
#endif





