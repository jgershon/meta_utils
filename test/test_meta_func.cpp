#include "gtest/gtest.h"
#include <cstddef>

import meta_utils;

#ifdef STL_MODULE
import std;
#else
#include <concepts>
#include <type_traits>
#endif

using namespace meta_utils;

struct to_unsigned
{
    template <std::integral T>
    using type = std::make_unsigned<T>;
};

using int_size =
    decltype([]<std::integral T> static -> std::size_t { return sizeof(T); });

using is_int =
    decltype([]<typename T> static -> bool { return std::integral<T>; });

using is_signed = decltype([]<typename T> static -> bool {
    if constexpr (std::integral<T>)
    {
        return std::same_as<T, std::make_signed_t<T>>;
    }
    else
    {
        return false;
    }
});

using is_const = decltype([]<typename T> static -> bool {
    return std::same_as<T, T const>;
});

TEST(meta_func, alias_r)
{
    EXPECT_TRUE(alias_r<std::remove_cvref<int const>>);
    EXPECT_FALSE(alias_r<int>);
}

TEST(meta_func, constant_r)
{
    EXPECT_TRUE((constant_r<std::true_type, bool>));
    EXPECT_FALSE((constant_r<std::remove_cvref<int const>, bool>));
}

TEST(meta_func, map_r)
{
    EXPECT_TRUE((map_r<to_unsigned, int>));
    EXPECT_FALSE((map_r<to_unsigned, double>));
}

TEST(meta_func, function_r)
{
    EXPECT_TRUE((function_r<int_size, std::size_t, int>));
    EXPECT_FALSE((function_r<int_size, std::size_t, double>));
}

TEST(meta_func, predicate_r)
{
    EXPECT_TRUE((predicate_r<is_int, int>));
    EXPECT_FALSE((predicate_r<to_unsigned, int>));
}

TEST(meta_func, invoke) { EXPECT_TRUE((invoke<is_int, int>())); }

TEST(meta_func, make_map)
{
    EXPECT_TRUE((map_r<make_map<std::make_signed>, int>));
}

TEST(meta_func, make_function)
{
    EXPECT_TRUE((predicate_r<make_function<std::is_arithmetic>, int>));
}

TEST(meta_func, is_a)
{
    EXPECT_TRUE((is_a<int>::template operator()<int>()));
    EXPECT_FALSE((is_a<int>::template operator()<double>()));
}

TEST(meta_func, negate)
{
    EXPECT_FALSE((negate<is_int>::template operator()<int>()));
    EXPECT_TRUE((negate<is_int>::template operator()<double>()));
}

TEST(meta_func, all)
{
    EXPECT_TRUE(
        (all<is_int, is_signed, is_const>::template operator()<int const>()));
    EXPECT_FALSE(
        (all<is_int, is_signed, is_const>::template operator()<int>()));
}

TEST(meta_func, any)
{
    EXPECT_TRUE(
        (any<is_int, is_signed, is_const>::template operator()<std::size_t>()));
    EXPECT_FALSE(
        (any<is_int, is_signed, is_const>::template operator()<double>()));
}