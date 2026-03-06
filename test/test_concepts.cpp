#include "gtest/gtest.h"

import meta_utils;

using namespace meta_utils;

TEST(concepts, arithmetic_r)
{
    EXPECT_TRUE((arithmetic_r<float>));
    EXPECT_FALSE((arithmetic_r<float *>));
}

TEST(concepts, same_cvref_r)
{
    EXPECT_TRUE((same_cvref_r<int const &, int>));
    EXPECT_FALSE((same_cvref_r<int const &, float const &>));
}

TEST(concepts, strictly_derived_from_r)
{
    struct foo
    {};
    struct bar : foo
    {};
    struct baz
    {};
    EXPECT_TRUE((strictly_derived_from_r<bar, foo>));
    EXPECT_FALSE((strictly_derived_from_r<foo, foo>));
    EXPECT_FALSE((strictly_derived_from_r<baz, foo>));
}
