
//
// Author: Chris Anderson
// Version 4/22/2022
//
//

/*
#############################################################################
#
# Copyright 1996-2022 Chris Anderson
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
#include <cstring>
#include <cmath>

#include "SCC_OperatorLib.h"

#ifndef REAL_OPERATOR_LIB_
#define REAL_OPERATOR_LIB_

namespace SCC
{
class  RealOperatorLib : public SCC::OperatorLib
{

public :

	RealOperatorLib()
	{
	    const char* LibSymbols [] =
	    {"+", "-", "+", "-", "*", "/","^","sin", "cos",
	    "tan","asin","acos","atan","atan2",
	    "sinh","cosh","tanh",
	    "ceil","exp","abs","floor","fmod","log","log10","sqrt","pow"};

	     Symbols = new const char*[operatorCount];
	     for(long i = 0; i <  operatorCount; ++i)
	     {
	    	 Symbols[i] = LibSymbols[i];
	     }

	     long PriorityValues [] =
	     {3,3,5,5,4,4,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

         Priority = new long[operatorCount];
	     for(long i = 0; i <  operatorCount; ++i)
	     {
	    	 Priority[i] = PriorityValues [i];
	     }

	     long ArgCountValues[] =
	     {1,1,2,2,2,2,2,1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,1,1,2};
	     ArgCount = new long[operatorCount];

	     for(long i = 0; i <  operatorCount; ++i)
	     {
	    	 ArgCount[i] = ArgCountValues[i];
	     }


	     void* FunctionArrayValues [] =
	     {
	     (void*)SCC::RealOperatorLib::Plus,
	     (void*)SCC::RealOperatorLib::Minus,
	     (void*)SCC::RealOperatorLib::Add,
	     (void*)SCC::RealOperatorLib::Subtract,
	     (void*)SCC::RealOperatorLib::Times,          // 5 //
	     (void*)SCC::RealOperatorLib::Divide,
	     (void*)SCC::RealOperatorLib::Exponentiate,
	     (void*)SCC::RealOperatorLib::Sin,
	     (void*)SCC::RealOperatorLib::Cos,
	     (void*)SCC::RealOperatorLib::Tan,            // 10 //
	     (void*)SCC::RealOperatorLib::Asin,
	     (void*)SCC::RealOperatorLib::Acos,
	     (void*)SCC::RealOperatorLib::Atan,
	     (void*)SCC::RealOperatorLib::Atan2,           //14//
	     (void*)SCC::RealOperatorLib::Sinh,
	     (void*)SCC::RealOperatorLib::Cosh,
	     (void*)SCC::RealOperatorLib::Tanh,
	     (void*)SCC::RealOperatorLib::Ceil,
	     (void*)SCC::RealOperatorLib::Exp,
	     (void*)SCC::RealOperatorLib::Abs,           // 20 //
	     (void*)SCC::RealOperatorLib::Floor,
	     (void*)SCC::RealOperatorLib::Fmod,
	     (void*)SCC::RealOperatorLib::Log,
	     (void*)SCC::RealOperatorLib::Log10,           // 24 //
	     (void*)SCC::RealOperatorLib::Sqrt,            // 25 //
	     (void*)SCC::RealOperatorLib::Pow
	     };

	     FunctionArray = new void*[operatorCount];
	     for(long i = 0; i <  operatorCount; ++i)
	     {
	    	 FunctionArray[i] = FunctionArrayValues[i];
	     }
	}

	~RealOperatorLib()
	{
		delete [] Symbols;
		delete [] Priority;
		delete [] ArgCount;
		delete [] FunctionArray;
	}


    long getOperatorIndex(const char* Sym) const
    {
        long ireturn = -1;
        long i;
    	for(i=0; i< operatorCount; i++)
        {
    	if(strcmp(Sym,Symbols[i]) == 0) ireturn = i;
        }
        return ireturn;
    }

    long getUnaryOperatorIndex(const char* Sym) const
    {
        long ireturn = -1;
    	if(strcmp(Sym,"+") == 0) ireturn = 0;
        if(strcmp(Sym,"-") == 0) ireturn = 1;
        return ireturn;
    }

    long getBinaryOperatorIndex(const char* Sym) const
    {
        long ireturn = -1;
    	if(strcmp(Sym,"+") == 0) ireturn = 2;
        if(strcmp(Sym,"-") == 0) ireturn = 3;
        return ireturn;
    }

    long getOperatorPriority(long index) const
    {
     	return Priority[index];
    }

    long getOperatorArgCount(long index) const
    {
     	return ArgCount[index];
    }

    long getOperatorCount() const
    {
     	return operatorCount;
    }

    const char* getOperatorSymbol(long index) const
    {
     	return Symbols[index];
    }
//
//  Unary Operators
//
    static void Plus(double** const argPtr)
    {	*argPtr[1] =  +(*argPtr[0]);  }

    static void Minus(double** const argPtr)
    {	*argPtr[1] =  -(*argPtr[0]);  }

    static void Add(double**  const argPtr)
    { 	*argPtr[2] =  (*argPtr[0])+(*argPtr[1]); }

    static void Subtract(double** const argPtr)
    { 	*argPtr[2] =  (*argPtr[0])-(*argPtr[1]); }

    static void Times(double** const argPtr)
    {	*argPtr[2] = (*argPtr[0])*(*argPtr[1]); }

    static void Divide(double** const argPtr)
    {	*argPtr[2] =  (*argPtr[0])/(*argPtr[1]); }

    static void Exponentiate(double** argPtr)
    {	*argPtr[2] =  std::pow(*argPtr[0],*argPtr[1]); }

    static void Sin(double** argPtr)
    { 	*argPtr[1] = std::sin(*argPtr[0]); }

    static void Cos(double** argPtr)
    {	*argPtr[1] = std::cos(*argPtr[0]); }

    static void Tan(double** argPtr)
    {	*argPtr[1] = std::tan(*argPtr[0]); }

    static void Asin(double** argPtr)
    {	*argPtr[1] = std::asin(*argPtr[0]); }

    static void Acos(double** argPtr)
    {	*argPtr[1] = std::acos(*argPtr[0]); }

    static void Atan(double** argPtr)
    {  *argPtr[1] = std::atan(*argPtr[0]); }

    static void Atan2(double** argPtr)
    { *argPtr[2] = std::atan2(*argPtr[0],*argPtr[1]);}

    static void Sinh(double** argPtr)
    {	*argPtr[1] = std::sinh(*argPtr[0]); }

    static void Cosh(double** argPtr)
    {	*argPtr[1] = std::cosh(*argPtr[0]); }

    static void Tanh(double** argPtr)
    {	*argPtr[1] = std::tanh(*argPtr[0]); }

    static void Ceil(double** argPtr)
    {	*argPtr[1] = std::ceil(*argPtr[0]); }

    static void Exp(double** argPtr)
    {	*argPtr[1] = std::exp(*argPtr[0]); }

    static void Abs(double** argPtr)
    {	*argPtr[1] = std::abs(*argPtr[0]); }

    static void Floor(double** argPtr)
    {	*argPtr[1] = floor(*argPtr[0]); }

    static void Fmod(double** argPtr)
    { *argPtr[2] = std::fmod(*argPtr[0],*argPtr[1]);}

    static void Log(double** argPtr)
    {	*argPtr[1] = std::log(*argPtr[0]); }

    static void Log10(double** argPtr)
    {	*argPtr[1] = std::log10(*argPtr[0]); }

    static void Sqrt(double** argPtr)
    {	*argPtr[1] = std::sqrt(*argPtr[0]); }

    static void Pow(double** argPtr)
    { *argPtr[2] = std::pow(*argPtr[0],*argPtr[1]);}



    constexpr static long  operatorCount = 26;

    void**   FunctionArray;
    const char**   Symbols;
    long*         Priority;
    long*         ArgCount;
};
}
#endif

