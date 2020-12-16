#include <iostream>
#include <string>

#include "calculator.hpp"


int main()
{
    float result;
    auto code = mobilfone::Calculator::do_math("14.5 * 23.3", result); // 337.85
    if (code == mobilfone::CALC_PARSE_STATUS::PARSE_OK) std::cout << "result == " << result;

    return 0;
}
