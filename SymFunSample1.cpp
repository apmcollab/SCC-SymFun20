#include <iostream>
#include <cmath>
#include <cstdio>
#include <functional>
 

#include "SCC_SymFun.h"
    
//
//######################################################################
//
// SCC::SymFun Test Program #1
//
// This program demonstrates the creation of an SCC::SymFun instance that
// implements and then evaluates f(x) = 2*x + sin(x)
//
//######################################################################
//
int main()
{
//
//  Create a SCC::SymFun that implements 2*x + sin(x)
//
    std::vector<std::string>      V = {"x"};               // x = independent variable
    std::string functionString = "2.0*x+ sin(x)";    // std::string specifying fun ction

    SCC::SymFun f;

    // The SymFun instance is initialized within a try/catch block so
    // that errors in function specification can be captured.

    try
    {
        f.initialize(V,functionString);
    }
    catch (const SCC::SymFunException& e)
    {
          std::cerr << e.what() << std::endl;;
          std::cerr << "XXXX Execution Terminated XXXXX" << std::endl;
          //return 1;
    }

    std::cout << "The function specified " << std::endl;
    std::cout << f.getConstructorString()  << std::endl << std::endl;

    std::cout << "The value of the function at x = 1.0 is: ";
    std::cout << f(1.0) << std::endl << std::endl;

    //
    // Demonstrate how to capture an anonymous function reference
    // to the SymFun instance
    //

    std::function<double(double)> f_Ref = f.getEvaluationPtr1d();

    std::cout << "The value of the function at x = 1.0 when f is  " << std::endl;
    std::cout << "accessed via an anonymous function  : ";
    std::cout << f_Ref(1.0) << std::endl << std::endl;

    printf("XXXX Execution Complete XXXXX\n");
    return 0;
}



  
