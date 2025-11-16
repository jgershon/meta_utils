#pragma once

#include "meta_utils/sequence.hpp"

#include <concepts>
#include <cstddef>
#include <utility>

namespace meta_utils {

template<std::integral Int, typename... I>
void
for_constexpr(std::integer_sequence<Int, I...> const &, auto && body)
{
    ((body.template operator()<I>()), ...);
}

template<std::integral Int, Int Begin, Int End, Int Step = 1, typename Body>
void
for_constexpr(Body && body)
{
    for_constexpr(make_integer_sequence<Int, Begin, End, Step>{}, std::forward<Body>(body));
}

} // namespace meta_utils
