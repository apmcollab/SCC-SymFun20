//
//##################################################################
//  		   CLASS  ExpressionTransform  Header
//##################################################################
//
// Chris Anderson 9/10/96 (C) UCLA
// Version 1/22/2009
//
/*
#############################################################################
#
# Copyright 1996-2019 Chris Anderson
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


//
// strcpy_s is not implemented as part of C++11 (arrgh) so this macro
// inserts strcpy calls.
//

#ifdef _MSC_VER
#define COPYSTR(src,count,dst) strcpy_s(src,count,dst)
#else
#define COPYSTR(src,count,dst) strcpy(src,dst)
#endif

#include <cstring>

#include "SCC_OperatorLib.h"
#include "SCC_SymFunException.h"

#ifndef  EXPRESSION_TRANSFORM_
#define  EXPRESSION_TRANSFORM_

namespace SCC
{
class  ExpressionTransform
{
    public :
	ExpressionTransform()
	{
	   variableCount = 0;
	   constantCount = 0;

	   executionArray     = 0;
	   executionArraySize = 0;

	   sNames             = 0;
	   symbolCount        = 0;

	   evaluationDataSize = 0;

	   opLib              = 0;

	}

	ExpressionTransform(const ExpressionTransform& E)
	{
		variableCount = E.variableCount;
		constantCount = E.constantCount;
	    symbolCount   = E.symbolCount;

	    evaluationDataSize = E.evaluationDataSize;

	    executionArraySize = E.executionArraySize;
	    executionArray     = new long[executionArraySize];

	    sNames             = new char*[symbolCount];

	    int i;
	    for(i=0; i< executionArraySize; i++)
	    executionArray[i] = E.executionArray[i];

	    for(i=0; i< symbolCount; i++)
	    {
	    sNames[i] = new char[strlen(E.sNames[i])+1];
	    COPYSTR(sNames[i], strlen(E.sNames[i]) + 1,E.sNames[i]);
	    }

	    opLib = E.opLib;
	}


	ExpressionTransform(char** V, int Vcount, char* S,
	OperatorLib* OLib)
	{
	    char** C   = 0;
	    int Ccount = 0;
	    opLib      = OLib;
		createTransform(V, Vcount, C, Ccount, S);
	}

	ExpressionTransform(char** V, int Vcount,
	char** C, int Ccount, char* S, OperatorLib* OLib)
	{
	    opLib      = OLib;
		createTransform(V, Vcount, C, Ccount, S);
	}
	//
	//##################################################################
	//  		            DESTRUCTOR
	//##################################################################
	//
	~ExpressionTransform()
	{
	 	destroy();
	}

	void destroy()
	{
	   if(executionArray != 0) delete [] executionArray;

	   int i;
	   if(sNames         != 0)
	   {
	   for(i=0; i< symbolCount; i++) if(sNames[i] != 0) delete [] sNames[i];
	   delete [] sNames;
	   }

	   variableCount = 0;
	   constantCount = 0;
	   symbolCount   = 0;

	   executionArray     = 0;
	   executionArraySize = 0;
	   evaluationDataSize = 0;

	   sNames    = 0;
	   opLib     = 0;
	}
	//
	//##################################################################
	//  		               INITIALIZE
	//##################################################################
	//
	int initialize()
	{
	    destroy();
	    return 0;
	}

	int initialize(char** V, int Vcount, char* S,
	OperatorLib* OLib)
	{
	    destroy();
	    char** C   = 0;
	    int Ccount = 0;
	    opLib      = OLib;

	    int    initReturn = createTransform(V, Vcount, C, Ccount, S);
	    return initReturn;
	}

	int initialize(char** V, int Vcount, char** C, int Ccount, char* S,
	OperatorLib* OLib)
	{
	    destroy();
	    opLib       = OLib;
	    int    initReturn = createTransform(V, Vcount, C, Ccount, S);
	    return initReturn;
	}
	//
	//##################################################################
	//  		            CREATE_TRANSFROM
	//##################################################################
	//
	int createTransform(char** V, int Vcount, char** C,
			int Ccount, char* expressionString)
	{
	//
	//  Capture Input Data
	//
	    variableCount = Vcount;
	    constantCount = Ccount;
	    char** vNames = V;
		char** cNames = C;
	    char* inputS  = expressionString;
	//
	//
	   int  i;  int j;  int k;
	   char* Sptr;
	//
	//######################################################################
	//
	//   SEPARATE EXPRESSION INTO TOKENS
	//
	//######################################################################
	//
	    char* S             = new char[2*strlen(inputS) + 1];
	    S[2*strlen(inputS)] = '\0';

	    int separateReturn;
	    try
	    {
	    	separateReturn = separateIntoTokens(inputS,S);
	    }
	    catch(SymFunException& e)
	    {
	    	delete [] S;
	    	e.offendingString = inputS;
	    	e.setErrorReturn();
	    	std::cout << e.what() << std::endl;
	    	throw e;
	    }

	    if(separateReturn != 0) {delete [] S; return separateReturn;}

	//   std::cout << S << std::endl;
	//
	// 	Insert string terminators between tokens
	//
	    long  Ssize = (long)strlen(S);
	    for(i=0; i < Ssize; i++) if(S[i] == ' ') S[i] = '\0';
	//
	//  Count tokens
	//
	    long tokenCount = 0;
	    Sptr = S;
	    while(Sptr < (S + Ssize - 1))
	    {
	    tokenCount++;
	    Sptr = Sptr + strlen(Sptr) + 1;
	    }
	    if(tokenCount == 0){delete [] S;  return 1;}
	//
	//######################################################################
	//
	//   ENCODE TOKENS INTO ITERMEDIATE EXPRESSION CODE
	//
	//######################################################################
	//
	    long* expressionCode = new long[2*tokenCount];
	//
	//  Count numerical constants
	//
	    Sptr = S;
	    long numericCount = 0;
	    while(Sptr < (S + Ssize - 1))
	    {
	    if((((int(Sptr[0]) >= 48)&&(int(Sptr[0]) <= 57)))
	    ||(Sptr[0] == '.'))
	    {numericCount++;}
	    Sptr = Sptr + strlen(Sptr) + 1;
	    }
	//
	//  Create an array of names for
	//  variables, symbolic constants, and numeric constants
	//
	//  The indices of these symbols match the corresponding
	//  index of the data array used to store their values.
	//
	    symbolCount   = variableCount + constantCount + numericCount;
	    sNames = new char*[symbolCount];

	    for(i = 0; i < variableCount; i++)
	    {
	      sNames[i] = new char[strlen(vNames[i]) + 1];
	      COPYSTR(sNames[i], strlen(vNames[i]) + 1,vNames[i]);
	    }
	    for(i = variableCount, j = 0; i < variableCount + constantCount; j++,i++)
	    {
	      sNames[i] = new char[strlen(cNames[j]) + 1];
	      COPYSTR(sNames[i], strlen(cNames[j]) + 1,cNames[j]);
	    }

	    Sptr = S;
	    i = variableCount + constantCount;
	    while(Sptr < (S + Ssize - 1))
	    {
	    if((((int(Sptr[0]) >= 48)&&(int(Sptr[0]) <= 57)))
	    ||(Sptr[0] == '.'))
	    {
	     sNames[i] = new char[strlen(Sptr) + 1];
	     COPYSTR(sNames[i], strlen(Sptr) + 1,Sptr);
	     i++;
	    }
	    Sptr = Sptr + strlen(Sptr) + 1;
	    }

	    long expressionCodeSize;

	    int encodeReturn;

	    try
	    {
	    encodeReturn = encodeExpression(S,Ssize,sNames,variableCount, constantCount,
	    symbolCount,expressionCode,expressionCodeSize);
	    }
	    catch(SymFunException& e)
	    {
	    e.offendingString = expressionString;
	    e.setErrorReturn();
	    delete [] S; delete [] expressionCode;
	    throw e;
	    }

	    if(encodeReturn != 0)
	    {delete [] S; delete [] expressionCode; return encodeReturn;}
	//
	//  code to check encoding : output should match input string
	//
	/*
	 	for(i = 0; i < expressionCodeSize; i=i+2)
	    {
	    if(expressionCode[i] == VAR)
	    { std::cout <<  expressionCode[i+1] << "  "; }
	    else if(expressionCode[i] == DELIM)
	    {
	    if(expressionCode[i+1] == LEFTP)  std::cout << "(";
	    if(expressionCode[i+1] == RIGHTP) std::cout << ")";
	    if(expressionCode[i+1] == COMMA)  std::cout << ",";
	    }
	    else if(expressionCode[i] > 0)
	    {
	    std::cout << opLib->getOperatorSymbol(expressionCode[i+1]);
	    std::cout << expressionCode[i+1] << "  ";
	    }}
	    std::cout << std::endl << std::endl << std::endl;
	*/
	//
	//
	//######################################################################
	//
	//   ERROR CHECKING
	//
	//######################################################################
	//
	    int parenthesisCheck = 0;
		for(i = 0; i < expressionCodeSize; i+=2)
	    {
	    if(expressionCode[i] == DELIM) parenthesisCheck  += expressionCode[i+1];
	    }
	    if(parenthesisCheck != 0)
	    {
	    SymFunException symFunException("Unbalanced Parenthesis","",inputS);
	    errorHandler(); delete [] S; delete [] expressionCode;
	    throw symFunException;
	    return 1;
	    }
	//
	//######################################################################
	//
	//   CREATE EXECUTION CODE : PACK INTO EXECUTIONARRAY
	//
	//######################################################################
	//
	//
	//  Count operators and arguments
	//
	//  Create storage space for executionArray
	//
	    long opCount  = 0;
	    long argCount = 0;
		for(i=0; i < expressionCodeSize; i+=2)
	    {
	    if(expressionCode[i] > 0)
	    {
	    opCount++;
	    argCount +=
	    opLib->getOperatorArgCount(expressionCode[i+1]) + 1;
	    }}

	    executionArraySize = 2*opCount + argCount;
	//
	//  Treat expressions with just one symbol
	//  (Modification  2/26/97)
	//  (Modificaiton 10/28/00)
	//
	    if(executionArraySize == 0)
	    {
	     executionArraySize = 4;
	     executionArray     = new long[executionArraySize];
	     executionArray[0]  = 0;  // set operand to unary +
	     executionArray[1]  = 2;  // two arguments

	     if(expressionCodeSize == 2)              // no parenthesis
	     {
	     executionArray[2]  = expressionCode[1];
	     }
	     if(expressionCodeSize == 6)              // parenthesis
	     {
	     executionArray[2]  = expressionCode[3];
	     }
	     executionArray[3]  = symbolCount;
	     evaluationDataSize = symbolCount + 1;
	     delete [] expressionCode;
	     delete [] S;
	     return 0;
	    }

	    executionArray     = new long[executionArraySize];

	    long  executionIndex = 0;
	    long  dataIndex      = symbolCount - 1;

	    int   istart;
	    int   iend;
	//
	//  Scan for paranthesis depth
	//
		int pDepth    = 0;
	    for(i=0; i < expressionCodeSize; i+=2)
	    {
	    if((expressionCode[i] == DELIM)&&(expressionCode[i+1]== LEFTP)) pDepth++;
	    }
	//
	//  Setup up execution sequences for each depth of paranthesis
	//
	    int pIndex;  int jp;
	    int sReturn;
	    for(k = pDepth; k >= 1; k--)
	    {
	     pIndex = 0;
	     for(j = 0; j < expressionCodeSize; j+=2)
	     {
	     if((expressionCode[j] == DELIM)&&(expressionCode[j+1]== LEFTP)) pIndex++;

	     if(pIndex == k)
	     {
	     expressionCode[j] = NOOP; expressionCode[j+1] = NOOP;  // consume (
	     istart = j;
	     jp = j+2;
	     while(!((expressionCode[jp] == DELIM)&&(expressionCode[jp+1] == RIGHTP)))
	     {jp+=2;}
	     iend   = jp;
	     expressionCode[jp] = NOOP;expressionCode[jp+1] = NOOP; // consume )

	     try
	     {
	     sReturn = setupEvaluation(expressionCode,istart,iend,  // process
	               executionArray,executionIndex, dataIndex);
	     }
	     catch(SymFunException& e)
	     {
	    	 e.offendingString = expressionString;
	    	 e.setErrorReturn();
	    	 delete [] S; delete [] expressionCode;
	    	 throw e;
	     }

	     if(sReturn != 0){delete [] S; delete [] expressionCode; return sReturn;}

	     pIndex--;
	     }}
	    }
	//
	//  Do pDepth == 0 directly
	//
	    istart   = 0;
	    iend     = expressionCodeSize;

	    try
	    {
	    sReturn = setupEvaluation(expressionCode,istart,iend,executionArray,
	             executionIndex, dataIndex);
	    }
	    catch(SymFunException& e)
	    {
	     e.offendingString = expressionString;
	     e.setErrorReturn();
	   	 delete [] S; delete [] expressionCode;
	   	 throw e;
	    }

	    if(sReturn != 0){delete [] S; delete [] expressionCode; return sReturn;}
	//
	//  set evaluationDataSize required
	//
	    evaluationDataSize = dataIndex+1;
	//
	//  Check expressioncode to see only one operand left
	//
	    int resultCount = 0;
		for(i = 0; i < expressionCodeSize; i+=2)
	    {
	    if(expressionCode[i] != NOOP) resultCount++;
	    }
	    if(resultCount != 1)
	    {
	        SymFunException symFunException("Illegal Expression","",inputS);
	        errorHandler(); delete [] S; delete [] expressionCode;
	        throw symFunException;
	        return 1;
	    }

	//
	//######################################################################
	//
	//   CLEAN UP
	//
	//######################################################################
	//
	    delete [] expressionCode;
	    delete [] S;
	    return 0;
	}
	//
	//##################################################################
	//  		           SETUPEVALUATION
	//##################################################################
	//

	int setupEvaluation(long* expressionCode,
	int istart, int iend,long* executionArray,long& executionIndex,
	long& dataIndex)
	{
	//
	//  This routine repeatedly scans the expressionCode from left
	//  to right and sets up the execution pointers and indices.
	//
	//  The expressionCode is scanned from i = istart to i < iend;
	//
	//  The execution sequence is coded into executionArray
	//  in the form :
	//
	//  [operatorIndex][argument Count][ ...argument indices ... ]
	//
	//
	//  It is assumed that the input expression code contains no
	//  paranthesis.
	//
	    int i; int j; int k;
	    long argIndex; long argIndexLeft; long argIndexRight;
	    int argCount;
	//
	//  remove ,'s
	//
		for(i = istart; i < iend; i=i+2)
	    {
	    if((expressionCode[i] == DELIM)&&(expressionCode[i+1]== COMMA))
	    {expressionCode[i] = NOOP; expressionCode[i+1] = NOOP;}
	    }
	//
	//  Function calls (priority 1)
	//
		for(i = istart; i < iend; i= i+2)
	    {
	    if(expressionCode[i] == 1)
	    {
	    //
	    // set function call
	    //
	    argCount = opLib->getOperatorArgCount(expressionCode[i+1]);
	    executionArray[executionIndex] = expressionCode[i+1]; executionIndex++;
	    executionArray[executionIndex] = argCount + 1;        executionIndex++;
	    //
	    // scan right for argCount arguments and pack indices
	    //
	    k = 0;
	 	for(j=i+2; j < iend; j=j+2)
	    {
	      if(expressionCode[j] != NOOP)
	      {
	      if(expressionCode[j] != VAR)
	      {
	      std::string errInfo = "Offending operator : ";
	      errInfo            += opLib->getOperatorSymbol(expressionCode[i+1]);
	      SymFunException symFunException("Incorrect Number of Arguments", errInfo,"");
	      errorHandler();
	      throw symFunException;
	      return 1;
	      }
	      else
	      {
	      argIndex            = expressionCode[j+1];   // get location
	      expressionCode[j]   = NOOP;                  // consume argument
	      expressionCode[j+1] = NOOP;
	      executionArray[executionIndex] = argIndex; executionIndex++;
	      k++;
	      if(k == argCount) j = iend;
	      }}
	    }
	    //
	    // ran off end looking for arguments
	    //
	    if(k!= argCount)
	    {
	      std::string errInfo = "Offending operator : ";
	      errInfo            += opLib->getOperatorSymbol(expressionCode[i+1]);
	      SymFunException symFunException("Incorrect Number of Arguments", errInfo,"");
	      errorHandler();
	      throw symFunException;
	      return 1;
	    }
	    //
	    // set pointer to return argument
	    //
	    dataIndex++;
	    executionArray[executionIndex] =  dataIndex;  executionIndex++;
	    //
	    // replace operator code by pointer to it's result
	    //
	    expressionCode[i]   = VAR;
	    expressionCode[i+1] = dataIndex;
	    }}

	//
	//  Exponentiation (priority 2)
	//
	    for(i=istart; i < iend; i= i+2)
	    {

	    if(expressionCode[i] == 2)
	    {
	    //
	    // set function call
	    //
	    executionArray[executionIndex] = expressionCode[i+1]; executionIndex++;
	    executionArray[executionIndex] = 3;                   executionIndex++;
	    //
	    // scan right for arguments
	    //
	    for(j=i+2; j < iend; j=j+2)
	    {
	      if(expressionCode[j] != NOOP)
	      {
	      if(expressionCode[j] != VAR)
	      {
	 	  std::string errInfo = "Offending operator : ";
		  errInfo            += opLib->getOperatorSymbol(expressionCode[i+1]);
	      SymFunException symFunException("Incorrect Number of Arguments", errInfo,"");
	      errorHandler();
	      throw symFunException;
	      return 1;
	      }
	      else
	      {
	      argIndexRight       = expressionCode[j+1];   // get location
	      expressionCode[j]   = NOOP;                  // consume argument
	      expressionCode[j+1] = NOOP;
	      j = iend;
	      }}
	    }
	    //
	    // scan left for arguments
	    //
	    for(j=i-2; j >= istart; j=j-2)
	    {
	      if(expressionCode[j] != NOOP)
	      {
	      if(expressionCode[j] != VAR)
	      {
	      std::string errInfo = "Offending operator : ";
	      errInfo            += opLib->getOperatorSymbol(expressionCode[i+1]);
	      SymFunException symFunException("Incorrect Number of Arguments", errInfo,"");
	      errorHandler();
	      throw symFunException;
	      return 1;
	      }
	      else
	      {
	      argIndexLeft        = expressionCode[j+1];   // get location
	      expressionCode[j]   = NOOP;                  // consume argument
	      expressionCode[j+1] = NOOP;
	      j = istart;
	      }}
	    }
	    //
	    // set up evaluation pointers
	    //
	    dataIndex++;
	    executionArray[executionIndex] = argIndexLeft;   executionIndex++;
	    executionArray[executionIndex] = argIndexRight;  executionIndex++;
	    executionArray[executionIndex] = dataIndex;      executionIndex++;
	    //
	    // replace operator code by pointer to it's result
	    //
	    expressionCode[i]  = VAR;
	    expressionCode[i+1]= dataIndex;
	    }}
	//
	//  Unary Operators (priority 3)
	//
	    for(i=istart; i < iend; i= i+2)
	    {

	    if(expressionCode[i] == 3)
	    {
	    //
	    // set function call
	    //
	    executionArray[executionIndex] = expressionCode[i+1]; executionIndex++;
	    executionArray[executionIndex] = 2;                   executionIndex++;
	    //
	    // scan right for arguments
	    //
	    for(j=i+2; j < iend; j=j+2)
	    {
	      if(expressionCode[j] != NOOP)
	      {
	      if(expressionCode[j] != VAR)
	      {
	 	  std::string errInfo = "Offending operator : ";
		  errInfo            += opLib->getOperatorSymbol(expressionCode[i+1]);
	      SymFunException symFunException("Incorrect Number of Arguments", errInfo,"");
	      errorHandler();
	      throw symFunException;
	      return 1;
	      }
	      else
	      {
	      argIndex = expressionCode[j+1];       // get location
	      expressionCode[j]   = NOOP;           // consume argument
	      expressionCode[j+1] = NOOP;
	      j = iend;
	      }}
	    }
	    //
	    // set up evaluation pointers
	    //
	    dataIndex++;
	    executionArray[executionIndex] = argIndex;  executionIndex++;
	    executionArray[executionIndex] = dataIndex; executionIndex++;
	    //
	    // replace operator code by pointer to it's result
	    //
	    expressionCode[i]  = VAR;
	    expressionCode[i+1]= dataIndex;
	    }}
	//
	//  Binary Operators (priority 4,5)
	//
	    for(k = 4; k <= 5; k++)
	    {

	    for(i=istart; i < iend; i= i+2)
	    {

	    if(expressionCode[i] == k)
	    {
	    //
	    // set function call
	    //
	    executionArray[executionIndex] = expressionCode[i+1]; executionIndex++;
	    executionArray[executionIndex] = 3;                   executionIndex++;
	    //
	    // scan right for arguments
	    //
		// #############################################################################
	    // Added this exeception when there are no arguments to the right to consume
	    // Feb. 8, 2009
	    //
	    if((i+2) >= iend)
	    {
	 	    std::string errInfo = "Offending operator : ";
		    errInfo            += opLib->getOperatorSymbol(expressionCode[i+1]);
	        SymFunException symFunException("Incorrect Number of Arguments", errInfo,"");
	        errorHandler();
	        throw symFunException;
	        return 1;
	    }
	    //
	    //#############################################################################
	    for(j=i+2; j < iend; j=j+2)
	    {
	      if(expressionCode[j] != NOOP)
	      {
	      if(expressionCode[j] != VAR)
	      {
	 	  std::string errInfo = "Offending operator : ";
		  errInfo            += opLib->getOperatorSymbol(expressionCode[i+1]);
	      SymFunException symFunException("Incorrect Number of Arguments", errInfo,"");
	      errorHandler();
	      throw symFunException;
	      return 1;
	      }
	      else
	      {
	      argIndexRight       = expressionCode[j+1];   // get location
	      expressionCode[j]   = NOOP;                  // consume argument
	      expressionCode[j+1] = NOOP;
	      j = iend;
	      }}
	    }
	    //
	    // scan left for arguments
	    //
	    for(j=i-2; j >= istart; j=j-2)
	    {
	      if(expressionCode[j] != NOOP)
	      {
	      if(expressionCode[j] != VAR)
	      {
	 	  std::string errInfo = "Offending operator : ";
		  errInfo            += opLib->getOperatorSymbol(expressionCode[i+1]);
	      SymFunException symFunException("Incorrect Number of Arguments", errInfo,"");
	      errorHandler();
	      throw symFunException;
	      return 1;
	      }
	      else
	      {
	      argIndexLeft        = expressionCode[j+1];   // get location
	      expressionCode[j]   = NOOP;                  // consume argument
	      expressionCode[j+1] = NOOP;
	      j = istart;
	      }}
	    }
	    //
	    // set up evaluation pointers
	    //
	    dataIndex++;
	    executionArray[executionIndex] = argIndexLeft;   executionIndex++;
	    executionArray[executionIndex] = argIndexRight;  executionIndex++;
	    executionArray[executionIndex] = dataIndex;      executionIndex++;
	    //
	    // replace operator code by pointer to it's result
	    //
	    expressionCode[i]  = VAR;
	    expressionCode[i+1]= dataIndex;
	    }}

	    }

	    return 0;
	}
	//
	//##################################################################
	//  		            ENCODE EXPRESSION
	//##################################################################
	//
	int encodeExpression(char* S,long Ssize,
	char** sNames, long vCount, long cCount, long sCount,
	long* expressionCode,long& expressionCodeSize)
	{
	//
	// This routine encodes the tokenized experession in S into an
	// intermediate expression code.
	//
	// Input parameters :
	//
	// S      = string in which tokens are separated by string terminators
	// Ssize  = total length of S in characters
	//
	// sNames = array of names of variables, symbolic and numeric constants
	// vCount = number of variable names
	// cCount = number of symbolic constant names
	// sCount = total number of symbol names
	//
	// Return Arguments :
	//
	// expressionCode     = array of longs to hold code (space allocated
	//					    externally)
	//
	// expressionCodeSize = size of expressionCode array
	//

	//
	// Declare and initialize encoding variables
	//
	    int i;
	    char* Sptr;
	    int tokenIndex = 0;
	    int opIndex;
	    int checkFlag;
	    int unaryFlag;

	    int unaryPlusIndex   = opLib->getUnaryOperatorIndex("+");
	    int unaryMinusIndex  = opLib->getUnaryOperatorIndex("-");
	    int binaryPlusIndex  = opLib->getBinaryOperatorIndex("+");
	    int binaryMinusIndex = opLib->getBinaryOperatorIndex("-");

	    int binaryTimesIndex  = opLib->getOperatorIndex("*");
	    int binaryDivideIndex = opLib->getOperatorIndex("/");
	    int binaryExpoIndex   = opLib->getOperatorIndex("^");

	    Sptr  = S;
	    char* Stoken;
	//
	//  Main encoding loop
	//
	    while(Sptr < S + Ssize - 1)
	    {
	    Stoken = Sptr;
	//
	// 	Delimiters : Coded (DELIM, Index of delimiter)
	//
	   	if (Stoken[0] == '(')
	   	{
	   	 expressionCode[tokenIndex] = DELIM; tokenIndex++;
	     expressionCode[tokenIndex] = LEFTP; tokenIndex++;
	   	}
	   	else if (Stoken[0] == ')')
	   	{
	     expressionCode[tokenIndex] =  DELIM; tokenIndex++;
	     expressionCode[tokenIndex] =  RIGHTP; tokenIndex++;
	    }
	   	else if (Stoken[0] == ',')
	   	{
	     expressionCode[tokenIndex] =  DELIM; tokenIndex++;
	     expressionCode[tokenIndex] =  COMMA; tokenIndex++;
	   	}
	//
	//  Numerical Constants : Coded (VAR, Index of evaluationData value)
	//
	    else if((((int(Stoken[0]) >= 48)&&(int(Stoken[0]) <= 57)))
	    ||(Stoken[0] == '.'))
	    {
	    for(i = cCount + vCount; i < sCount; i++)
	    {
	    if(strcmp(Stoken,sNames[i]) == 0)
	    {
	       expressionCode[tokenIndex] = VAR;  tokenIndex++;
	       expressionCode[tokenIndex] =   i;  tokenIndex++;
	       i = sCount;
	    }}
	   }
	//
	//  Symbolic Tokens
	//
	  	else if(    ((int(Stoken[0]) >= 65)&&(int(Stoken[0]) <= 90))
	             || ((int(Stoken[0]) >= 97)&&(int(Stoken[0]) <= 122))
	           )
	    {
	//
	//  Variables  : Coded (VAR, Index of evaluationData value)
	//
	    checkFlag = 0;
	    for(i = 0; i < vCount; i++)
	    {
	    if(strcmp(Stoken,sNames[i]) == 0)
	    {
	       expressionCode[tokenIndex] = VAR;  tokenIndex++;
	       expressionCode[tokenIndex] =   i;  tokenIndex++;
	       checkFlag = 1;
	    }}
	//
	//  Symbolic Constants  : Coded (VAR, Index of evaluationData value)
	//
	    if(checkFlag == 0)
	    {
	    for(i = vCount; i < vCount + cCount; i++)
	    {
	    if(strcmp(Stoken,sNames[i]) == 0)
	    {
	       expressionCode[tokenIndex] = VAR;  tokenIndex++;
	       expressionCode[tokenIndex] =  i;   tokenIndex++;
	       checkFlag = 1;
	    }
	    }}
	//
	//  Symbolic Operators : Coded (priority value, Index of Function)
	//
	    if(checkFlag == 0)
	    {
	     opIndex = opLib->getOperatorIndex(Stoken);
	     if(opIndex >= 0)
	     {
	      expressionCode[tokenIndex] =
	      opLib->getOperatorPriority(opIndex);
	      tokenIndex++;
	      expressionCode[tokenIndex] =  opIndex;
	      tokenIndex++;
	      checkFlag = 1;
	    }}
	    //
	    // Have a symbolic constant which doesn't match operator, variable
	    // or a constant
	    //
	    if(checkFlag != 1)
	    {
	    SymFunException symFunException("Illegal Symbol",Stoken,"");
	    errorHandler();
	    throw symFunException;
	    return 1;
	    }
	    }
	//
	//  Algebraic Operators : Coded (priority value, Index of Function)
	//
	  	else if(Stoken[0] == '+')
	    {

	    if(tokenIndex == 0)                          // first operand
	    { unaryFlag = 1;}
	    else if
	    (
	    (expressionCode[tokenIndex  -2] == DELIM)&&     // delimiter
	    ((expressionCode[tokenIndex -1] == LEFTP)||     //   (
	    (expressionCode[tokenIndex  -1] == COMMA))      //   ,
	    )
	    { unaryFlag = 1;}
	    else if
	    (
	    (expressionCode[tokenIndex  -2] > 0)&&                 // operand
	    ((expressionCode[tokenIndex -1] == binaryPlusIndex)||  //   +
	     (expressionCode[tokenIndex -1] == binaryMinusIndex)|| //   -
	     (expressionCode[tokenIndex -1] == binaryDivideIndex)||//   /
	     (expressionCode[tokenIndex -1] == binaryExpoIndex)||  //   +
	     (expressionCode[tokenIndex -1] == binaryTimesIndex))  //   *
	    )
	    { unaryFlag = 1;}
	    else
	    {
	    unaryFlag = 0;
	    }

	    if(unaryFlag == 1)
	    {
	      expressionCode[tokenIndex] =
	      opLib->getOperatorPriority(unaryPlusIndex);
	      tokenIndex++;
	      expressionCode[tokenIndex] =  unaryPlusIndex;
	      tokenIndex++;
	    }
	    else
	    {
	 	  expressionCode[tokenIndex] =
	      opLib->getOperatorPriority(binaryPlusIndex);
	      tokenIndex++;
	      expressionCode[tokenIndex] =  binaryPlusIndex;
	      tokenIndex++;
	    }
	    }
	//
	//
	//
	    else if(Stoken[0] == '-')
	    {
	    if(tokenIndex == 0)                          // first operand
	    { unaryFlag = 1;}
	    else if
	    (
	    (expressionCode[tokenIndex  -2] == DELIM)&&     // delimiter
	    ((expressionCode[tokenIndex -1] == LEFTP)||     //   (
	    (expressionCode[tokenIndex  -1] == COMMA))      //   ,
	    )
	    { unaryFlag = 1;}
	    else if
	    (
	    (expressionCode[tokenIndex  -2] > 0)&&                 // operand
	    ((expressionCode[tokenIndex -1] == binaryPlusIndex)||  //   +
	     (expressionCode[tokenIndex -1] == binaryMinusIndex)|| //   -
	     (expressionCode[tokenIndex -1] == binaryDivideIndex)||//   /
	     (expressionCode[tokenIndex -1] == binaryExpoIndex)||  //   +
	     (expressionCode[tokenIndex -1] == binaryTimesIndex))  //   *
	    )
	    { unaryFlag = 1;}
	    else
	    {
	    unaryFlag = 0;
	    }

	    if(unaryFlag == 1)
	    {
	      expressionCode[tokenIndex] =
	      opLib->getOperatorPriority(unaryMinusIndex);
	      tokenIndex++;
	      expressionCode[tokenIndex] =  unaryMinusIndex;
	      tokenIndex++;
	    }
	    else
	    {
	 	  expressionCode[tokenIndex] =
	      opLib->getOperatorPriority(binaryMinusIndex);
	      tokenIndex++;
	      expressionCode[tokenIndex] =  binaryMinusIndex;
	      tokenIndex++;
	    }
	//
	//                     *, /, ^
	//
	    }
	    else if((Stoken[0] == '*')||(Stoken[0] == '/')||(Stoken[0] == '^'))
	    {
	     opIndex = opLib->getOperatorIndex(Stoken);
	     expressionCode[tokenIndex] =
	     opLib->getOperatorPriority(opIndex);
	     tokenIndex++;
	     expressionCode[tokenIndex] =  opIndex;
	     tokenIndex++;
	    }
	    else
	    {
	    SymFunException symFunException("Illegal Symbol",Stoken,"");
	    errorHandler();
	    throw symFunException;
	    return 1;
	    }
	 //
	 // update token pointer
	 //
	    Sptr = Sptr + strlen(Sptr) + 1;
	    }

	    expressionCodeSize = tokenIndex;
	    return 0;
	 }
	//
	//##################################################################
	//  		        SEPARATE_INTO_TOKENS
	//##################################################################
	//
	int separateIntoTokens(char* sIn, char* S)
	{
	//
	//  This routine takes the string sIn and decomposes it into
	//  distinct tokens separated by single spaces " ". The storage for
	//  S must be declared externally (maximal size will be
	//  (2*strlen(sIn) + 1).  No error checking is done to see if S
	//  is of sufficient size.
	//
	    char* sInput  = new char[strlen(sIn) + 1];  //  capture input string
	    COPYSTR(sInput, strlen(sIn) + 1, sIn);                        //
	//
	    int i;
	//
	//  Remove Spaces
	//
	    int  iMark = 0;
	    for(i = 0; i < (int)strlen(sInput); i++)
	    {
	    if(sInput[i] != ' '){ sInput[iMark] = sInput[i]; iMark++; }
	    }
	    sInput[iMark] = '\0';
	//
	//  Insert Spaces
	//
	    iMark = 0;
	    for(i = 0; i < (int)strlen(sInput); i++)
	    {
	//
	//  Operands and delimiters
	//
	    if( ((int(sInput[i]) >= 40)&&(int(sInput[i]) <= 45))
	        ||(sInput[i] == '/')
	        ||(sInput[i] == '^')
	      ) {S[iMark] = sInput[i]; iMark++; S[iMark] = ' '; iMark++;}

	    else
	//
	//  Variables and symbolic constants
	//
	    if(    ((int(sInput[i]) >= 65)&&(int(sInput[i]) <= 90))
	        || ((int(sInput[i]) >= 97)&&(int(sInput[i]) <= 122))
	      )
	    {
	    while
	      (    ((int(sInput[i]) >= 65)&&(int(sInput[i]) <= 90))
	        || ((int(sInput[i]) >= 97)&&(int(sInput[i]) <= 122))
	        || ((int(sInput[i]) >= 48)&&(int(sInput[i]) <= 57))
	      ){ S[iMark] = sInput[i]; iMark++; i++; }
	      i--;
	      S[iMark] = ' '; iMark++;
	    }
	//
	//  Digits
	//
	    else
	    if ((((int(sInput[i]) >= 48)&&(int(sInput[i]) <= 57)))
	      ||(sInput[i] == '.'))
	    {
	     while((int(sInput[i]) >= 48)&&(int(sInput[i]) <= 57))
	     {S[iMark] = sInput[i]; iMark++; i++; }
	 //
	 //  Decimal Numbers
	 //
	     if(sInput[i] == '.')
	     {
	     S[iMark] = '.'; iMark++; i++;
	     while((int(sInput[i]) >= 48)&&(int(sInput[i]) <= 57))
	     {S[iMark] = sInput[i]; iMark++; i++; }
	     }
	 //
	 //  Exponential Notation
	 //
	     if((sInput[i] == 'e')||(sInput[i] == 'E'))
	     {
	     S[iMark] = 'e'; iMark++; i++;
	     if(sInput[i] == '+'){S[iMark] = '+'; iMark++; i++;}
	     if(sInput[i] == '-'){S[iMark] = '-'; iMark++; i++;}
	     while((int(sInput[i]) >= 48)&&(int(sInput[i]) <= 57))
	     {S[iMark] = sInput[i]; iMark++; i++; }
	     }
	     else
	//
	//   Check for invalid numeric input
	//
	     if(   ((int(sInput[i]) >= 65)&&(int(sInput[i]) <= 90))
	        || ((int(sInput[i]) >= 97)&&(int(sInput[i]) <= 122))
	     )
	     {
	     std::string symbol(1,sInput[i]);
	     delete [] sInput;
	     SymFunException symFunException("Illegal Symbol",symbol,sIn);
	     errorHandler();
	     throw symFunException;
	     return 1;
	     }
	//
	//  Insert Space
	//
	     i--;
	     S[iMark] = ' '; iMark++;

	    }
	    else
	    {
	    std::string symbol(1,sInput[i]);
	    delete [] sInput;
	    SymFunException symFunException("Illegal Symbol",symbol,sIn);
	    errorHandler();
	    throw symFunException;
	    return 1;
	    }

	    }
	//
	//  Close String  and clean up
	//
	    S[iMark] = '\0';
	    delete [] sInput;
	    return 0;
	}

	//
	//##################################################################
	//  		        Error Handler
	//##################################################################
	//
	void errorHandler()
	{
	    destroy();
	}


    char** getSymbolNamesPtr()     const {return sNames;};
    long   getSymbolCount()        const {return symbolCount;};
    long   getEvaluationDataSize() const {return evaluationDataSize;};
    long*  getExecutionArrayPtr()  const {return executionArray;};
    long   getExecutionArraySize() const {return executionArraySize;};

//
//  Internal Data Storage
//
    private :

    int         variableCount;
    int         constantCount;

    char**      sNames;
    long        symbolCount;

    long*       executionArray;
    long        executionArraySize;

    long        evaluationDataSize;

    SCC::OperatorLib* opLib;

    enum{DELIM = -2, VAR   =-1, NOOP   = 0};
    enum{LEFTP = -1, COMMA = 0, RIGHTP = 1};



};
} // namespace
#endif

