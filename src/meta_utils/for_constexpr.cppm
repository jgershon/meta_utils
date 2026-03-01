module;

#ifdef STL_MODULE
import std;
#else
#include <cstddef>
#include <utility>
#endif

export module meta_utils:for_constexpr;

import :sequence;
import :tuple;

export namespace meta_utils {

template <typename T, T... Vals>
constexpr void
for_constexpr(sequence::sequence<T, Vals...> const &, auto && body)
{
    ((body.template operator()<Vals>()), ...);
}

template <arithmetic_r T, T Begin, T End, T Step = 1, typename Body>
constexpr void
for_constexpr(Body && body)
{
    for_constexpr(sequence::make_sequence<T, Begin, End, Step>{},
                  std::forward<Body>(body));
}

template <tuple::tuple_r Tuple, typename Body>
constexpr void
for_each(Tuple && values, Body && body)
{
    for_constexpr<std::size_t, 0, size(values)>(
        [values = std::forward<Tuple>(values),
         body = std::forward<Body>(body)]<std::size_t I> {
            body(std::get<I>(values));
        });
}

} // namespace meta_utils
