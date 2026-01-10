#include "gtest/gtest.h"

#include "meta_utils/const_math.hpp"
#include "touch.hpp"

#include <array>
#include <cmath>
#include <utility>

using namespace meta_utils;

TEST(test_const_math, test_floor_log2)
{
    for (auto x = size_t{0}; x < 1024; ++x)
    {
        EXPECT_EQ(floor_log2(x), std::floor(std::log2(x)));
    };
    EXPECT_THROW({touch(floor_log2(size_t{0}));}, std::invalid_argument);
    EXPECT_THROW({touch(floor_log2(-1));}, std::invalid_argument);
}

TEST(test_const_math, test_floor_log)
{
    auto const x = int{12345};
    auto const y = double{54321};
    for (auto b = 0.5; b < 10; ++b)
    {
        EXPECT_EQ(floor_log(x, b), std::floor(std::log(x) / std::log(b)));
        EXPECT_EQ(floor_log(y, b), std::floor(std::log(y) / std::log(b)));
    }
    EXPECT_THROW({touch(floor_log(0, 3));}, std::invalid_argument);
    EXPECT_THROW({touch(floor_log(-1, 3));}, std::invalid_argument);
}

TEST(test_const_math, test_pow)
{
    auto const x = int{17};
    auto const y = double{13};
    for (auto p = size_t{0}; p < 5; ++p)
    {
        EXPECT_EQ(pow(x, p), std::pow(x, p));
        EXPECT_EQ(pow(y, p), std::pow(y, p));
    }
}

TEST(test_const_math, test_n_choose_k)
{
    auto test_pair = [] <size_t N, size_t K> ()
    {
        auto expected = tgamma(N+1) / tgamma(K+1) / tgamma(N-K+1);
        EXPECT_EQ((n_choose_k<N, K>()), expected);
        EXPECT_EQ(n_choose_k(N, K), expected);
    };
    auto test_row = [&] <size_t... K> (std::index_sequence<K...>)
    {
        static constexpr auto N = sizeof...(K) - 1;
        ((test_pair.operator()<N, K>()), ...);
    };
    [&] <size_t... N> (std::index_sequence<N...>)
    {
        ((test_row(std::make_index_sequence<N+1>{})), ...);
    } (std::make_index_sequence<6>{});
    EXPECT_THROW({touch(n_choose_k(-1, -2));}, std::invalid_argument);
    EXPECT_THROW({touch(n_choose_k(2, 3));}, std::invalid_argument);
}
