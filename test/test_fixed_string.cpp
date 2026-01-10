#include "gtest/gtest.h"

#include <cstring>

import meta_utils;

using namespace meta_utils;

constexpr char mic_test[] = "Is this thing on?";

TEST(test_fixed_string, test_constructor)
{
    constexpr auto fs = fixed_string{mic_test};
}

TEST(test_fixed_string, test_equals)
{
    constexpr auto fs = fixed_string{mic_test};
    EXPECT_EQ(fs, fs);
    EXPECT_NE(fixed_string{"Nope."}, fs);
}

TEST(test_fixed_string, test_char_ptr)
{
    EXPECT_EQ(
        strcmp(static_cast<char const *>(fixed_string{mic_test}), mic_test), 0);
}

TEST(test_fixed_string, test_string_view)
{
    EXPECT_EQ(static_cast<std::string_view>(fixed_string{mic_test}),
              std::string_view{mic_test});
}

TEST(test_fixed_string, test_size)
{
    EXPECT_EQ(fixed_string{mic_test}.size(), strlen(mic_test));
}
