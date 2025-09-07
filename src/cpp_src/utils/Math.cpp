#include "Math.h"

#include <cmath>
#include <cstdlib>

namespace pbmath {

long long ceil_div(long long a, long long b) { return (a + b - 1) / b; }

bool is_less_than(long double a, long double b) { return (b - a) > pbmath::EPS; }

bool is_greater_than(long double a, long double b) { return (a - b) > pbmath::EPS; }

bool is_equal(long double a, long double b) { return std::abs(a - b) <= pbmath::EPS; }

long double floor(long double x) { return std::floor(x + pbmath::EPS); }

long double ceil(long double x) { return std::ceil(x - pbmath::EPS); }

} // namespace pbmath
