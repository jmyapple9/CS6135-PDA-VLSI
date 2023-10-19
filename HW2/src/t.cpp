#include <iostream>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

using namespace boost::multiprecision;
using namespace std;
int main() {
    
    cpp_dec_float_50 a = 8740 * 37145;  // Example values for a, b, c
    cpp_dec_float_50 b = 0;
    cpp_dec_float_50 c = (cpp_dec_float_50) 75815 * (cpp_dec_float_50)129160;

    cpp_dec_float_50 result = (a + b) / (c);

    cout << "Result: " << result << endl;

    return 0;
}