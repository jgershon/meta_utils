#pragma once

#include <concepts>
#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace meta_utils {

template<typename T>
concept arithmetic = std::is_arithmetic_v<T>;

[[nodiscard]] inline constexpr auto
floor_log2(std::unsigned_integral auto n)
{
    auto result = decltype(n){0};
    while (n > 1)
    {
        ++result;
        n = n >> 1;
    }
    return result; 
}

template<arithmetic Value, arithmetic Base>
[[nodiscard]] inline constexpr auto
floor_log(Value const & x, Base const & base)
{
    using result_type = std::common_type_t<Value, Base>;

    if (x <= 0) [[unlikely]]
    {
        throw std::invalid_argument{"floor_log() argument <= 0."};
    }
    auto result = result_type{0};
    auto remainder = static_cast<result_type>(x);
    while (remainder > 1)
    {
        ++result;
        remainder /= base;
    }
}

[[nodiscard]] inline constexpr auto
pow(arithmetic auto const & base, std::unsigned_integral auto exponent)
{
    auto result = decltype(base){1};
    auto squares = base;
    while (exponent)
    {
        if (exponent & 1)
        {
            result *= squares;
        }
        exponent = exponent >> 1;
        squares *= squares;
    }
    return result;
}

template<std::unsigned_integral Int>
[[nodiscard]] inline constexpr auto
n_choose_k(Int const & n, Int const & k)
{
    if (k > n) [[unlikely]]
    {
        throw std::invalid_argument{"n_choose_k() k > n."};
    }
    auto inner = [&] (this auto && self, Int const & m, Int const & j) -> Int
    {
        if ((j == 0) || (j == m)) [[unlikely]]
        {
            return Int{1};
        }
        return self(m - 1, j - 1) + self(m - 1, j);
    };
    return inner(n, k);
}

template<size_t N, size_t K>
requires ((K == 0) || (K == N))
[[nodiscard]] inline consteval size_t
n_choose_k()
{
    return size_t{1};
}

template<size_t N, size_t K>
requires ((K > 0) && (K < N))
[[nodiscard]] inline consteval size_t
n_choose_k()
{
    return n_choose_k<N - 1, K - 1>() + n_choose_k<N - 1, K>();
}

} // namespace meta_utils
