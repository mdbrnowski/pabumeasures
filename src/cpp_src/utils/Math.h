#pragma once

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <optional>

namespace pbmath {

constexpr long double EPS = 1e-10;

long long ceil_div(long long a, long long b);

bool is_less_than(long double a, long double b);

bool is_greater_than(long double a, long double b);

bool is_equal(long double a, long double b);

long double floor(long double x);

long double ceil(long double x);

template <typename T> std::optional<T> optional_min(const std::optional<T> &current, const T &new_value) {
    if (current) {
        return std::min(*current, new_value);
    }
    return new_value;
}

template <typename T> std::optional<T> optional_max(const std::optional<T> &current, const T &new_value) {
    if (current) {
        return std::max(*current, new_value);
    }
    return new_value;
}

} // namespace pbmath
