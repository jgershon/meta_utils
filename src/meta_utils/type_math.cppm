module;

#ifdef STL_MODULE
import std;
#else
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <ratio>
#include <tuple>
#include <type_traits>
#endif

export module meta_utils:type_math;

import :tuple;

namespace meta_utils::type_math {

export {
    template <typename T>
    struct traits
    {
        using base_type = std::remove_cvref_t<T>;
        using exponent = std::ratio<1>;
    };

    template <typename T>
    using base_of = typename traits<T>::base_type;

    template <typename T>
    using exponent_of = typename traits<T>::exponent;

    struct unity
    {};

    template <>
    struct traits<unity> : traits<void>
    {};

} // export

template <typename Base, typename Exp>
struct raised_
{};

template <typename T, std::intmax_t Den>
    requires(Den != 0)
struct raised_<T, std::ratio<0, Den>>
{
    using type = unity;
};

template <typename T, std::intmax_t I>
    requires(I != 0)
struct raised_<T, std::ratio<I, I>>
{
    using type = std::remove_cvref_t<T>;
};

template <typename Base, typename Exp, typename Pow>
struct raised_<raised_<Base, Exp>, Pow>
    : raised_<Base, typename std::ratio_multiply<Exp, Pow>::type>
{};

export {

    template <typename Base, intmax_t Num, intmax_t Den>
    struct traits<raised_<Base, std::ratio<Num, Den>>>
    {
        using base_type = std::remove_cvref_t<Base>;
        using exponent = typename std::ratio<Num, Den>::type;
    };

    template <typename T, typename Base = void>
    concept raised_r = requires {
        requires(std::same_as<Base, void> &&
                 std::derived_from<T, raised_<base_of<T>, exponent_of<T>>>) ||
                    std::derived_from<T, raised_<Base, exponent_of<T>>>;
    };
}

template <typename>
struct product_
{};

template <typename... T>
struct product_<std::tuple<T...>>
{
    using factors = std::tuple<std::remove_cvref_t<T>...>;
    using base_types = std::tuple<base_of<T>...>;
    static constexpr auto size = sizeof...(T);
};

template <std::size_t Len, typename Prod>
struct head_
{
    using type = product_<tuple::head<Len, typename Prod::factors>>;
};

template <typename Prod>
struct head_<0, Prod>
{
    using type = unity;
};

template <std::size_t Len, typename Prod>
struct tail_
{
    using type = product_<tuple::tail<Len, typename Prod::factors>>;
};

template <typename Prod>
struct tail_<0, Prod>
{
    using type = unity;
};

export {

    template <typename T>
    concept product_r = requires {
        requires std::derived_from<T, product_<typename T::factors>>;
    };

    template <product_r Prod>
    struct traits<Prod>
    {};

    template <typename Prod, typename T>
    concept contains_r = requires {
        requires product_r<Prod>;
        requires tuple::contains_r<typename Prod::base_types, base_of<T>>;
    };

    template <typename T, product_r Prod>
    inline constexpr std::size_t index_of =
        tuple::index_of<base_of<T>, typename Prod::base_types>;

    template <std::size_t Len, product_r Prod>
    using head = typename head_<Len, Prod>::type;

    template <std::size_t Len, product_r Prod>
    using tail = typename tail_<Len, Prod>::type;

    template <std::size_t I, product_r Prod>
    using at = tuple::at<I, Prod>;

} // export

template <typename T, typename Exp>
struct pow_
{
    using type = raised_<T, Exp>;
};

template <typename... T, typename Exp>
struct pow_<product_<std::tuple<T...>>, Exp>
{
    using type = product_<std::tuple<pow_<T, Exp>...>>;
};

template <typename...>
struct multiply_
{};

template <>
struct multiply_<>
{
    using type = unity;
};

template <typename T>
struct multiply_<T>
{
    using type = std::remove_cvref_t<T>;
};

template <typename T>
struct multiply_<T, unity> : multiply_<T>
{};

template <typename T>
    requires(!std::same_as<T, unity>)
struct multiply_<unity, T> : multiply_<T>
{};

template <product_r Prod, typename... T1, typename... T2>
struct multiply_<Prod, product_<std::tuple<T1...>>, T2...>
    : multiply_<Prod, T1..., T2...>
{};

template <typename T1, typename T2, typename... T3>
    requires(!std::same_as<T1, unity> && std::same_as<base_of<T1>, base_of<T2>>)
struct multiply_<T1, T2, T3...>
{
    using exponent =
        typename std::ratio_add<exponent_of<T1>, exponent_of<T2>>::type;
    using type =
        typename multiply_<raised_<base_of<T1>, exponent>, T3...>::type;
};

template <typename T1, typename T2, typename... T3>
    requires(!std::same_as<T1, unity> && !std::same_as<T2, unity> &&
             !std::same_as<base_of<T1>, base_of<T2>>)
struct multiply_<T1, T2, T3...> : multiply_<product_<std::tuple<T1, T2>>, T3...>
{};

template <typename... T1, typename T2, typename... T3>
    requires(!contains_r<product_<std::tuple<T1...>>, T2>)
struct multiply_<product_<std::tuple<T1...>>, T2, T3...>
    : multiply_<product_<std::tuple<T1..., T2>>, T3...>
{};

template <typename T1, typename... T2, typename... T3>
    requires(!contains_r<product_<std::tuple<T2...>>, T1>)
struct multiply_<T1, product_<std::tuple<T2...>>, T3...>
    : multiply_<product_<std::tuple<T1, T2...>>, T3...>
{};

template <typename T1, product_r Prod, typename... T2>
    requires(contains_r<Prod, T1>)
struct multiply_<T1, Prod, T2...> : multiply_<Prod, T1, T2...>
{};

template <product_r Prod, typename T1, typename... T2>
    requires(contains_r<Prod, T1>)
struct multiply_<Prod, T1, T2...>
{
    static constexpr std::size_t index = index_of<T1, Prod>;
    using type =
        typename multiply_<head<index, Prod>,
                           typename multiply_<at<index, Prod>, T1>::type,
                           tail<Prod::size - index - 1, Prod>, T2...>::type;
};

export {

    template <typename T, intmax_t Num, intmax_t Den = 1>
    using pow = typename pow_<T, std::ratio<Num, Den>>::type;

    template <typename... T>
    using multiply = typename multiply_<T...>::type;

    template <typename Num, typename Den>
    using divide = multiply<Num, pow<Den, -1>>;

} // export

} // namespace meta_utils::type_math
