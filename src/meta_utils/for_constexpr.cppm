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
for_constexpr(sequence::value_sequence<T, Vals...>, auto && body)
{
    if constexpr (requires {
                      { (body(Vals), ...) };
                  })
    {
        (body(Vals), ...);
    }
    else
    {
        ((body.template operator()<Vals>()), ...);
    }
}

template <typename T, T... Vals>
constexpr void
for_constexpr(sequence::integer_sequence<T, Vals...>, auto && body)
{
    if constexpr (requires {
                      { (body(Vals), ...) };
                  })
    {
        (body(Vals), ...);
    }
    else
    {
        ((body.template operator()<Vals>()), ...);
    }
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
for_constexpr(Body && body)
{
    for_constexpr<std::size_t, 0, tuple::size<Tuple>>(
        [body = std::forward<Body>(body)]<std::size_t I> {
            body.template operator()<tuple::at<I, Tuple>>();
        });
}

template <tuple::tuple_r Tuple, typename Body>
constexpr void
for_each(Tuple && values, Body && body)
{
    for_constexpr<std::size_t, 0, tuple::size<Tuple>>(
        [values = std::forward<Tuple>(values),
         body = std::forward<Body>(body)]<std::size_t I> {
            body(tuple::get<I>(values));
        });
}

} // namespace meta_utils
