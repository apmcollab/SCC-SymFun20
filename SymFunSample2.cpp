#include <iostream>
#include <cmath>
#include <cstdio>
#include <string>
 

#include "SCC_SymFun.h"

//
//######################################################################
//
// SymFun Test Program #2
//
// INPUT :
// ------
// User is prompted to specify a function f(x).
//
// Example :  Typing x*x +2 followed by a carriage return would input
// the function x squared plus 2. The syntax for function specification
// is that of the syntax for a C++ expression.
//
// User is prompted to specify the evaluation point
//
// OUTPUT :
// ------
// The value of the function at the evaluation point
//
//######################################################################
//
int main()
{
    std::string functionString;      // std::string to hold function
                                     // specification
//
//  Obtain Function Input
//
    std::cout << "Enter f(x) :" ;
    getline (std::cin,functionString);
    std::cout << std::endl;

    std::vector<std::string>  V = {"x"};  // x = independent variable

    SCC::SymFun f;

    // The SymFun instance is initialized within a try/catch block so
    // that errors in function specification can be captured.

    try
    {
    	f.initialize(V,functionString);
    }
    catch (const SCC::SymFunException& e)
    {
      std::cerr << e.what() << std::endl;
      std::cerr << "XXXX Execution Terminated XXXXX" << std::endl;
      return 1;
    }

    std::cout << "The function specified : ";
    std::cout << f.getConstructorString()  << std::endl << std::endl;

    double x;
    std::cout << "Enter evaluation point : " ;
    std::cin  >> x;
    std::cout << std::endl;

    std::cout << "The value of the function at x = " << x << "  is: ";
    std::cout << f(x) << std::endl << std::endl;

    printf("XXXX Execution Complete XXXXX\n");
    return 0;
}

  
