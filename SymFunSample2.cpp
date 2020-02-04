#include <iostream>
#include <cmath>
#include <cstdio>
#include <string>
using namespace std;

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
    string functionString;      // string to hold function
                                // specification
//
//  Obtain Function Input
//
    cout << "Enter f(x) :" ;
    getline (cin,functionString);
    cout << endl;

    vector<string>  V = {"x"};  // x = independent variable

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
      return 1;
    }

    cout << "The function specified : ";
    cout << f.getConstructorString()  << endl << endl;

    double x;
    cout << "Enter evaluation point : " ;
    cin  >> x;
    cout << endl;

    cout << "The value of the function at x = " << x << "  is: ";
    cout << f(x) << endl << endl;

    printf("XXXX Execution Complete XXXXX\n");
    return 0;
}

  
