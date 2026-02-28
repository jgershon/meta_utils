module;

#ifdef STL_MODULE
import std;
#else
#include <utility>
#endif

export module meta_utils:for_constexpr;

import :sequence;

export namespace meta_utils {

template <typename T, T... Vals>
void
for_constexpr(sequence::sequence<T, Vals...> const &, auto && body)
{
    ((body.template operator()<Vals>()), ...);
}

template <arithmetic_r T, T Begin, T End, T Step = 1, typename Body>
void
for_constexpr(Body && body)
{
    for_constexpr(sequence::make_sequence<T, Begin, End, Step>{},
                  std::forward<Body>(body));
}

} // namespace meta_utils
