## SymFun20
A repository for the distribution of the classes SCC::SymFun and SCC::SymFunUtility and required supporting classes. 
 
Instances of SCC::SymFun provide means of evaluating functions of double variables that are specified symbolically, i.e. as strings. The names of the variables and symbolic constants used to represent the function are specified by the programmer. The operations allowed in the function specification include the standard algebraic operations as well as many of the standard functions available for double values in the C++ standard library.

The operators allowed are

 {"+", "-", "+", "-", "*", "/", "^", "sin", "cos", "tan","asin","acos","atan","atan2", "sinh","cosh","tanh",
	    "ceil","exp","abs","floor","fmod","log","log10","sqrt","pow"};

The "^" is interpreted as the exponentiation operator, i.e. x^2 is x squared.

The SCC::SymFunUtility class provides member functions that differentiate SCC::SymFun instances symbolically and return the result as a SCC:SymFun instance. 

In addition to the samples contained within this repository, the documentation contains code snippets that demonstrate usage of the SCC::SymFun and SCC::SymFunUtility classes. 

**Note:** The SCC::SymFun and SCC::SymFunUtility in this repository are not backwards compatable with prior versions of the SymFun classes. The header files for the new versions are distinguished by the prefix SCC_ and all source code is contained within these new header files to facilitate usage. 

### Prerequisites
C++11
### Versioning
Release : 2.0.2
### Date
April 30, 2020
### Authors
Chris Anderson
### License
GPLv3  For a copy of the GNU General Public License see <http://www.gnu.org/licenses/>.
### Acknowledgements







