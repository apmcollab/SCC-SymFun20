#include <iostream>
#include <cmath>
#include <cstdio>
#include <functional>
using namespace std;

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
    vector<string>      V = {"x"};               // x = independent variable
    string functionString = "2.0*x+ sin(x)";    // string specifying fun ction

    SCC::SymFun f;

    // The SymFun instance is initialized within a try/catch block so
    // that errors in function specification can be captured.

    try
    {
        f.initialize(V,functionString);
    }
    catch (const SCC::SymFunException& e)
    {
          cerr << e.what() << endl;;
          cerr << "XXXX Execution Terminated XXXXX" << endl;
          //return 1;
    }

    cout << "The function specified " << endl;
    cout << f.getConstructorString()  << endl << endl;

    cout << "The value of the function at x = 1.0 is: ";
    cout << f(1.0) << endl << endl;

    //
    // Demonstrate how to capture an anonymous function reference
    // to the SymFun instance
    //

    std::function<double(double)> f_Ref = f.getEvaluationPtr1d();

    cout << "The value of the function at x = 1.0 when f is  " << endl;
    cout << "accessed via an anonymous function  : ";
    cout << f_Ref(1.0) << endl << endl;

    printf("XXXX Execution Complete XXXXX\n");
    return 0;
}



  
