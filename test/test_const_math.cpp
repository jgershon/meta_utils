#include "gtest/gtest.h"

#include "meta_utils/const_math.hpp"
#include "touch.hpp"

#include <array>
#include <cmath>
#include <utility>

using namespace meta_utils;

TEST(test_const_math, test_floor_log2)
{
    auto test_val = [] <size_t N> ()
    {
        auto expected = floor(log2(N));
        EXPECT_EQ((floor_log2<size_t, N>()), expected);
        EXPECT_EQ((floor_log2(N)), expected);
    };
    [&] <size_t... N> (std::index_sequence<N...>)
    {
        ((test_val.operator()<N + 1>()), ...);
    } (std::make_index_sequence<1024>{});
    EXPECT_THROW({touch(floor_log2(0));}, std::invalid_argument);
    EXPECT_THROW({touch(floor_log2(-1));}, std::invalid_argument);
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
