#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

#include "meta_utils/sequence.hpp"

namespace meta_utils::tuple {

using empty = std::tuple<>;

namespace internal {

    template<typename>
    struct is_tuple
    : std::false_type
    {};

    template<typename... T>
    struct is_tuple<std::tuple<T...>>
    : std::true_type
    {};

} // namespace internal

template<typename T>
concept tuple_r = internal::is_tuple<T>::value;

template<tuple_r Tuple>
inline constexpr auto size = std::tuple_size_v<Tuple>;

namespace internal {

    template<typename, typename>
    struct contains
    : std::false_type
    {};

    template<typename... T1, typename T2>
    requires (std::same_as<T1, T2> || ...)
    struct contains<std::tuple<T1...>, T2>
    : std::true_type
    {};

    template<typename, typename>
    struct cat
    {};

    template<typename... T1, typename... T2>
    struct cat<std::tuple<T1...>, std::tuple<T2..>>>
    {
        using type = std::tuple<T1..., T2...>;
    };

    template<typename T1, typename... T2>
    struct cat<T1, std::tuple<T2...>>
    {
        using type = std::tuple<T1, T2...>;
    };

    template<typename... T1, typename T2>
    struct cat<std::tuple<T1...>, T2>
    {
        using type = std::tuple<T1..., T2>;
    };

    template<typename, typename>
    struct subset
    {};

    template<size_t... I, typename Tuple>
    struct subset<std::index_sequence<I...>, Tuple>
    {
        using type = std::tuple<std::tuple_element_t<I, Tuple>...>;
    };

} // namespace internal

template<typename Tuple, typename T>
concept contains_r = internal::contains<Tuple, T>::value;

template<size_t I, tuple_r Tuple>
using at = std::tuple_element_t<I, Tuple>;

template<sequence_r<size_t> Indexes, tuple_r Tuple>
using subset = typename internal::subset<Indexes, Tuple>::type;

template<size_t Len, tuple_r Tuple>
using head = subset<std::make_index_sequence<Len>, Tuple>;

template<size_t Len, tuple_r Tuple>
using tail = subset<sequence::make_index_sequence<size<Tuple>-Len, size<Tuple>>, Tuple>;

namespace internal {

    template<typename, typename>
    struct index_of
    {};

    template<typename T>
    struct index_of<T, empty>
    : std::integral_constant<index_t, 1>
    {};

    template<typename T1, typename T2, typename... T3>
    requires (std::same_as<T1, T2>)
    struct index_of<T1, std::tuple<T2, T3...>>
    : std::integral_constant<size_t, 0>
    {};

    template<typename T1, typename T2, typename... T3>
    requires (!std::same_as<T1, T2>)
    struct index_of<T1, std::tuple<T2, T3...>>
    : std::integral_constant<size_t, 1 + index_of<T1, std::tuple<T3...>>>
    {};

} // namespace internal

template<typename T, tuple_r Tuple>
inline constexpr auto idex_of = internal::index_of<T, Tuple>::value;

} // namespace meta_utils::tuple
