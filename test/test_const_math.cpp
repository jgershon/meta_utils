#include "gtest/gtest.h"

import meta_utils;

#include <cmath>
#include <utility>

using namespace meta_utils;

TEST(test_const_math, test_floor_log2)
{
    for (auto x = size_t{1}; x < 1024; ++x)
    {
        EXPECT_EQ(floor_log2(x), std::floor(std::log2(x)));
    };
    EXPECT_THROW(
        { [[maybe_unused]] auto x = floor_log2(size_t{0}); },
        std::invalid_argument);
}

TEST(test_const_math, test_pow)
{
    auto const x = int{17};
    auto const y = double{13};
    for (auto p = size_t{0}; p < 5; ++p)
    {
        EXPECT_EQ(meta_utils::pow(x, p), std::pow(x, p));
        EXPECT_EQ(meta_utils::pow(y, p), std::pow(y, p));
    }
}

TEST(test_const_math, test_n_choose_k)
{
    auto test_pair = []<size_t N, size_t K>() {
        auto expected = tgamma(N + 1) / tgamma(K + 1) / tgamma(N - K + 1);
        EXPECT_EQ((n_choose_k<N, K>()), expected);
        EXPECT_EQ(n_choose_k(N, K), expected);
    };
    auto test_row = [&]<size_t... K>(std::index_sequence<K...>) {
        static constexpr auto N = sizeof...(K) - 1;
        ((test_pair.operator()<N, K>()), ...);
    };
    [&]<size_t... N>(std::index_sequence<N...>) {
        ((test_row(std::make_index_sequence<N + 1>{})), ...);
    }(std::make_index_sequence<6>{});
    EXPECT_THROW(
        { [[maybe_unused]] auto x = n_choose_k(-1, -2); },
        std::invalid_argument);
    EXPECT_THROW(
        { [[maybe_unused]] auto x = n_choose_k(2, 3); }, std::invalid_argument);
}
