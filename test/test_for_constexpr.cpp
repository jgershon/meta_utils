#include "gtest/gtest.h"

import meta_utils;

#ifdef STL_MODULE
import std;
#else
#include <type_traits>
#endif

using namespace meta_utils;

TEST(test_for_constexpr, test_sequence)
{
    using doubles = sequence::make_double_sequence<0.0, 4.0>;

    double sum = 0;
    for_constexpr(doubles{}, [&]<double I> { sum += I; });
    EXPECT_EQ(sum, 6);

    sum = 0;
    for_constexpr(doubles{}, [&](double i) { sum += i; });
    EXPECT_EQ(sum, 6);

    using ints = sequence::make_sequence<int, 0, 4>;

    sum = 0;
    for_constexpr(ints{}, [&]<int I> { sum += I; });
    EXPECT_EQ(sum, 6);

    sum = 0;
    for_constexpr(ints{}, [&](int i) { sum += i; });
    EXPECT_EQ(sum, 6);
}

TEST(test_for_constexpr, test_step)
{
    int sum = 0;
    for_constexpr<int, 0, 4>([&]<int I> { sum += I; });
    EXPECT_EQ(sum, 6);

    sum = 0;
    for_constexpr<int, 0, 10, 2>([&]<int I> { sum += I; });
    EXPECT_EQ(sum, 20);
}

TEST(test_for_constexpr, test_types)
{
    using seq = tuple::tuple<
        std::integral_constant<int, 0>, std::integral_constant<int, 1>,
        std::integral_constant<int, 2>, std::integral_constant<int, 3>>;

    int sum = 0;
    for_constexpr<seq>([&]<typename I> { sum += I::value; });
    EXPECT_EQ(sum, 6);
}

TEST(test_for_constexpr, test_tuple)
{
    int sum = 0;
    for_each(tuple::make_tuple(0, 1.0, 2z, 3.f),
             [&](auto i) { sum += static_cast<int>(i); });
    EXPECT_EQ(sum, 6);
}