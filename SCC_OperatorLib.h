//
//##################################################################
//  		        OPERATOR LIB (VIRTUAL BASE CLASS)
//            FOR SYMFUN SYMBOLIC FUNCTION MATHEMATICAL OPERATORS
//##################################################################
//
//	                 Chris Anderson 9/10/96 (C) UCLA
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
#ifndef  OPERATOR_LIB_
#define  OPERATOR_LIB_
namespace SCC
{
class  OperatorLib
{

public :

    virtual int  getOperatorIndex(const char*) const {return 0;};
    virtual int  getUnaryOperatorIndex(const char*) const {return 0;};
    virtual int  getBinaryOperatorIndex(const char*) const {return 0;};

    virtual int   getOperatorPriority(int) const {return 0;};
    virtual const char* getOperatorSymbol(int) const {return 0;};
    virtual int   getOperatorCount() const {return 0;};
    virtual int   getOperatorArgCount(int) const {return 0;};
};
}
#endif

