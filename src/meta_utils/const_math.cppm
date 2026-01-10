module;

#include <concepts>
#include <stdexcept>
#include <type_traits>

export module meta_utils:const_math;

import :types;

export namespace meta_utils {

[[nodiscard]] inline constexpr auto
floor_log2(std::integral auto n)
{
    if (n <= 0) [[unlikely]]
    {
        throw std::invalid_argument{"floor_log2() argument == 0."};
    }
    auto result = decltype(n){0};
    while (n > 1)
    {
        ++result;
        n = n >> 1;
    }
    return result;
}

[[nodiscard]] inline constexpr auto
pow(arithmetic_r auto const & base, std::unsigned_integral auto exponent)
{
    auto result = decltype(base){1};
    auto squares = base;
    while (exponent)
    {
        if (exponent & 1)
        {
            result *= squares;
        }
        exponent >>= 1;
        squares *= squares;
    }
    return result;
}

template <std::integral Int1, std::integral Int2>
[[nodiscard]] inline constexpr auto
n_choose_k(Int1 const & n, Int2 const & k)
{
    using result_type = std::common_type_t<Int1, Int2>;

    if (k < 0) [[unlikely]]
    {
        throw std::invalid_argument{"n_choose_k() k < 0."};
    }
    if (k > n) [[unlikely]]
    {
        throw std::invalid_argument{"n_choose_k() k > n."};
    }
    auto inner = [&](this auto && self, auto const & m,
                     auto const & j) -> result_type {
        if ((j == 0) || (j == m)) [[unlikely]]
        {
            return result_type{1};
        }
        return self(m - 1, j - 1) + self(m - 1, j);
    };
    return inner(n, k);
}

template <size_t N, size_t K>
    requires((K == 0) || (K == N))
[[nodiscard]] inline consteval size_t
n_choose_k()
{
    return size_t{1};
}

template <size_t N, size_t K>
    requires((K > 0) && (K < N))
[[nodiscard]] inline consteval size_t
n_choose_k()
{
    return n_choose_k<N - 1, K - 1>() + n_choose_k<N - 1, K>();
}

} // namespace meta_utils
