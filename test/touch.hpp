#pragma once

inline constexpr void
touch(auto const & val)
{
    static_cast<void>(val);
}
