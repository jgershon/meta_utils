#pragma once

#include "meta_utils/const_math.hpp"
#include "meta_utils/sequence.hpp"
#include "meta_utils/types.hpp"

#include <concepts>
#include <cstddef>
#include <utility>

namespace meta_utils {

template<typename T, typename... Vals>
void
for_constexpr(sequence::sequence<T, Vals...> const &, auto && body)
{
    ((body.template operator()<Vals>()), ...);
}

template<arithmetic_r T, T Begin, T End, T Step = 1, typename Body>
void
for_constexpr(Body && body)
{
    for_constexpr(sequence::make_sequence<T, Begin, End, Step>{}, std::forward<Body>(body));
}

} // namespace meta_utils
