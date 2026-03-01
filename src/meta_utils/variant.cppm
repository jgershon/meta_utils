module;

#ifdef STL_MODULE
import stl;
#else
#include <tuple>
#include <type_traits>
#include <variant>
#endif

export module meta_utils:variant;

import :tuple;
import :overload;

namespace meta_utils::variant {

template <typename>
struct is_variant_ : std::false_type
{};

template <typename... T>
struct is_variant_<std::variant<T...>> : std::true_type
{};

template <typename, typename...>
struct make_variant_
{};

template <typename... V>
struct make_variant_<std::tuple<V...>>
{
    using type = std::variant<V...>;
};

template <typename... V, typename... T>
struct make_variant_<std::tuple<V...>, void, T...>
    : make_variant_<std::tuple<V...>, std::monostate, T...>
{};

template <typename... V, typename... T>
    requires(!tuple::contains_r<std::tuple<V...>, std::monostate>)
struct make_variant_<std::tuple<V...>, std::monostate, T...>
    : make_variant_<std::tuple<std::monostate, V...>, T...>
{};

template <typename... V, typename T1, typename... T2>
    requires(tuple::contains_r<std::tuple<V...>, T1>)
struct make_variant_<std::tuple<V...>, T1, T2...>
    : make_variant_<std::tuple<V...>, T2...>
{};

template <typename... V, typename T1, typename... T2>
    requires(!tuple::contains_r<std::tuple<V...>, T1>)
struct make_variant_<std::tuple<V...>, T1, T2...>
    : make_variant_<std::tuple<V..., T1>, T2...>
{};

template <typename>
struct from_tuple_
{};

template <typename... T>
struct from_tuple_<std::tuple<T...>> : make_variant_<tuple::empty, T...>
{};

template <typename>
struct to_tuple_
{};

template <typename... T>
struct to_tuple_<std::variant<T...>>
{
    using type = std::tuple<T...>;
};

template <typename, typename>
struct visit_result_
{};

template <typename Func, typename... Args>
struct visit_result_<Func, std::variant<Args...>>
    : make_variant_<std::invoke_result_t<Func, Args>...>
{};

export {

    using empty = std::variant<>;

    template <typename T>
    concept variant_r = is_variant_<T>::value;

    template <typename... T>
    using make_variant = typename make_variant_<tuple::empty, T...>::type;

    template <tuple::tuple_r Tuple>
    using from_tuple = typename from_tuple_<Tuple>::type;

    template <variant_r Var>
    using to_tuple = typename to_tuple_<Var>::type;

    template <typename Var, typename T>
    concept contains_r = requires {
        requires variant_r<Var>;
        requires tuple::contains_r<to_tuple<Var>, T>;
    };

    template <overload_r Func, variant_r Var>
    using visit_result = typename visit_result_<Func, Var>::type;

    template <overload_r Func, variant_r Var>
    constexpr auto
    apply(Func && func, Var && var) -> visit_result<Func, Var>
    {
        return std::visit(std::forward<Func>(func), std::forward<Var>(var));
    }
} // export

template <typename Var1, typename Var2>
struct set_func_
{
    using tuple_1 = to_tuple<Var1>;
    using tuple_2 = to_tuple<Var2>;

    struct in_1
    {
        template <typename T>
        static constexpr bool value = tuple::contains_r<tuple_1, T>;
    };

    struct in_2
    {
        template <typename T>
        static constexpr bool value = tuple::contains_r<tuple_2, T>;
    };
}; // struct set_func_

template <typename Var1, typename Var2>
struct unite_
{
    using type = from_tuple<tuple::cat<to_tuple<Var1>, to_tuple<Var2>>>;
};

template <typename Var1, typename Var2>
struct intersect_
{
    using set_func = set_func_<Var1, Var2>;
    using type = from_tuple<
        tuple::filter<typename set_func::tuple_1, typename set_func::in_2>>;
};

template <typename Var1, typename Var2>
struct diff_
{
    using set_func = set_func_<Var1, Var2>;
    using type = from_tuple<
        tuple::erase_if<typename set_func::tuple_1, typename set_func::in_2>>;
};

template <typename Var1, typename Var2>
struct symmetric_diff_
{
    using set_func = set_func_<Var1, Var2>;
    using type = from_tuple<tuple::cat<
        tuple::erase_if<typename set_func::tuple_1, typename set_func::in_2>,
        tuple::erase_if<typename set_func::tuple_2, typename set_func::in_1>>>;
};

export {

    template <variant_r Var1, variant_r Var2>
    using unite = typename unite_<Var1, Var2>::type;

    template <variant_r Var1, variant_r Var2>
    using intersect = typename intersect_<Var1, Var2>::type;

    template <variant_r Var1, variant_r Var2>
    using diff = typename diff_<Var1, Var2>::type;

    template <variant_r Var1, variant_r Var2>
    using symmetric_diff = typename symmetric_diff_<Var1, Var2>::type;

    template <typename T, variant_r Var>
    using insert = unite<Var, std::variant<T>>;

    template <typename T, variant_r Var>
    using erase = diff<Var, std::variant<T>>;

} // export

} // namespace meta_utils::variant
