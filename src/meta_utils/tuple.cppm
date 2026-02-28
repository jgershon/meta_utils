module;

#ifdef STL_MODULE
import std;
#else
#include <cstddef>
#include <tuple>
#include <type_traits>
#endif

export module meta_utils:tuple;

import :sequence;

namespace meta_utils::tuple {

template <typename>
struct is_tuple_ : std::false_type
{};

template <typename... T>
struct is_tuple_<std::tuple<T...>> : std::true_type
{};

export {

    using empty = std::tuple<>;

    template <typename T>
    concept tuple_r = is_tuple_<T>::value;

    template <tuple_r Tuple>
    inline constexpr auto size = std::tuple_size_v<Tuple>;

} // export

template <typename, typename>
struct contains_ : std::false_type
{};

template <typename... T1, typename T2>
    requires(std::same_as<T1, T2> || ...)
struct contains_<std::tuple<T1...>, T2> : std::true_type
{};

template <typename, typename>
struct cat_
{};

template <typename... T1, typename... T2>
struct cat_<std::tuple<T1...>, std::tuple<T2...>>
{
    using type = std::tuple<T1..., T2...>;
};

template <typename T1, typename... T2>
struct cat_<T1, std::tuple<T2...>>
{
    using type = std::tuple<T1, T2...>;
};

template <typename... T1, typename T2>
struct cat_<std::tuple<T1...>, T2>
{
    using type = std::tuple<T1..., T2>;
};

template <typename, typename>
struct subset_
{};

template <size_t... I, typename Tuple>
struct subset_<std::index_sequence<I...>, Tuple>
{
    using type = std::tuple<std::tuple_element_t<I, Tuple>...>;
};

export {

    template <typename Tuple, typename T>
    concept contains_r = contains_<Tuple, T>::value;

    template <size_t I, tuple_r Tuple>
    using at = std::tuple_element_t<I, Tuple>;

    template <sequence::sequence_r<size_t> Indexes, tuple_r Tuple>
    using subset = typename subset_<Indexes, Tuple>::type;

    template <size_t Len, tuple_r Tuple>
    using head = subset<std::make_index_sequence<Len>, Tuple>;

    template <size_t Len, tuple_r Tuple>
    using tail =
        subset<sequence::make_index_sequence<size<Tuple> - Len, size<Tuple>>,
               Tuple>;

} // export

template <typename, typename>
struct index_of_
{};

template <typename T>
struct index_of_<T, empty> : std::integral_constant<size_t, 1>
{};

template <typename T1, typename T2, typename... T3>
    requires(std::same_as<T1, T2>)
struct index_of_<T1, std::tuple<T2, T3...>> : std::integral_constant<size_t, 0>
{};

template <typename T1, typename T2, typename... T3>
    requires(!std::same_as<T1, T2>)
struct index_of_<T1, std::tuple<T2, T3...>>
    : std::integral_constant<size_t,
                             1 + index_of_<T1, std::tuple<T3...>>::value>
{};

export {

    template <typename T, tuple_r Tuple>
    inline constexpr auto index_of = index_of_<T, Tuple>::value;
}

} // namespace meta_utils::tuple
