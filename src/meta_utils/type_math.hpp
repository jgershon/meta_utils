#pragma once

#include <concepts>
#include <cstdint>
#include <ratio>
#include <type_traits>

#include "meta_utils/tuple.hpp"

namespace meta_utils::type_math {

template<typename T>
struct traits
{
    using base_type = std::remove_cvref_t<T>;
    using exponent = std::ratio<1>;
};

template<typename T>
using base_of = typename traits<T>::base_type;

template<typename T>
using exponent_of = typename traits<T>::exponent;

struct unity
{};

template<>
struct traits<unity>
: traits<void>
{};

namespace internal {

    template<typename Base, typename Exp>
    struct raised
    {};

    template<typename T, std::intmax_t Den>
    requires (Den != 0)
    struct raised<T, std::ratio<0, Den>>
    {
        using type = unity;
    };

    template<typename T, std::intmax_t I>
    requires (I != 0)
    struct raised<T, std::ratio<I, I>>
    {
        using type = std::remove_cvref_t<T>;
    };

    template<typename Base, typename Exp, typename Pow>
    struct raised<raised<Base, Exp>, Pow>
    : raised<Base, typename std::ratio_multiply<Exp, Pow>::type>
    {};

} // namespace internal

template<typename Base, intmax_t Num, intmax_t Den>
struct traits<internal::raised<Base, std::ratio<Num, Den>>>
{
    using base_type = std::remove_cvref_t<Base>;
    using exponent = typename std::ratio<Num, Den>::type;
};

template<typename T, typename Base = void>
concept raised_r = requires {
    requires (std::same_as<Base, void> && std::derived_from<T, internal::raised<base_of<T>, exponent_of<T>>>)
        || std::derived_from<T, internal::raised<Base, exponent_of<T>>>;
};

namespace internal {

    template<typename>
    struct product
    {};

    template<typename... T>
    struct product<std::tuple<T...>>
    {
        using factors = std::tuple<std::remove_cvref_t<T>...>;
        using base_types = std::tuple<base_of<T>...>;
        static constexpr auto size = sizeof...(T);
    };

    template<size_t Len, typename Prod>
    struct head
    {
        using type = product<tuple::head<Len, typename Prod::factors>>;
    };

    template<typename Prod>
    struct head<0, Prod>
    {
        using type = unity;
    };

    template<size_t Len, typename Prod>
    struct tail
    {
        using type = product<tuple::tail<Len, typename Prod::factors>>;
    };

    template<typename Prod>
    struct tail<0, Prod>
    {
        using type = unity;
    };

} // namespace internal

template<typename T>
concept product_r = requires {
    requires std::derived_from<T, internal::product<typename T::factors>>;
};

template<product_r Prod>
struct traits<Prod>
{};

template<typename Prod, typename T>
concept contains_r = requires {
    requires product_r<Prod>;
    requires tuple::contains<typename Prod::base_types, base_of<T>>;
};

template<typename T, product_r Prod>
inline constexpr size_t index_of = tuple::index_of<base_of<T>, typename Prod::base_types>;

template<size_t Len, product_r Prod>
using head = typename internal::head<Len, Prod>::type;

template<size_t Len, product_r Prod>
using tail = typename internal::tail<Len, Prod>::type;

template<size_t I, product_r Prod>
using at = tuple::at<I, Prod>;

namespace internal {

    template<typename T, typename Exp>
    struct pow
    {
        using type = raised<T, Exp>;
    };

    template<typename... T, typename Exp>
    struct pow<product<T...>, Exp>
    {
        using type = product<pow<T, Exp>...>;
    };

    template<typename...>
    struct mulitply
    {};

    template<>
    struct multiply<>
    {
        using type = unity;
    };

    template<typename T>
    struct multiply<T>
    {
        using type = std::remove_cvref_t<T>;
    };

    template<typename T>
    struct multiply<T, unity>
    : multiply<T>
    {};

    template<typename T>
    requires (!std::same_as<T, unity>)
    struct multiply<unity, T>
    : multiply<T>
    {};

    template<product_r Prod, typename... T1, typename... T2>
    struct multiply<Prod, product<T1...>, T2...>
    : multiply<Prod, T1..., T2...>
    {};

    template<typename T1, typename T2, typename... T3>
    requires (!std::same_as<T1, unity> && std::same_as<base_of<T1>, base_of<T2>>)
    struct multiply<T1, T2, T3...>
    {
        using exponent = typename std::ratio_add<exponent_of<T1>, exponent_of<T2>>::type;
        using type = typename multiply<raised<base_of<T1>, exponent>, T3...>::type;
    };

    template<typename T1, typename T2, typename... T3>
    requires (!std::same_as<T1, unity> && !std::same_as<T2, unity> && !std::same_as<base_of<T1>, base_of<T2>>)
    struct multiply<T1, T2, T3...>
    : multiply<product<T1, T2>, T3...>
    {};

    template<typename... T1, typename T2, typename... T3>
    requires (!contains<product<T1...>, T2>)
    struct multiply<product<T1...>, T2, T3...>
    : multiply<product<T1..., T2>, T3...>
    {};

    template<typename T1, typename... T2, typename... T3>
    requires (!contains<product<T2...>, T1>)
    struct multiply<T1, product<T2...>, T3...>
    : multiply<product<T1, T2...>, T3...>
    {};

    template<typename T1, product_r Prod, typename... T2>
    requires (contains<Prod, T1>)
    struct multiply<T1, Prod, T2...>
    : multiply<Prod, T1, T2...>
    {};

    template<product_r Prod, typename T1, typename... T2>
    requires (contains<Prod, T1>)
    struct multiply<Prod, T1, T2...>
    {
        static constexpr size_t index = index_of<T1, Prod>;
        using type = typename multiply<
            head<index, Prod>,
            typename multiply<at<index, Prod>, T1>::type,
            tail<Prod::size - index - 1, Prod>,
            T2...
        >::type;
    };

} // namespace internal

template<typename T, intmax_t Num, intmax_t Den = 1>
using pow = typename internal::pow<T, std::ratio<Num, Den>>::type;

template<typename... T>
using multiply = typename internal::multiply<T...>::type;

template<typename Num, typename Den>
using divide = multiply<Num, pow<Den, -1>>;

} // namespace meta_utils::type_math
