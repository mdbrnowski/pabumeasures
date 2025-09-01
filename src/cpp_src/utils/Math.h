#pragma once

namespace pbmath {

constexpr long double EPS = 1e-10;

long long ceil_div(long long a, long long b);

bool is_less_than(long double a, long double b);

bool is_greater_than(long double a, long double b);

bool is_equal(long double a, long double b);

long double floor(long double x);

} // namespace pbmath
