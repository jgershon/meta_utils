module;

#ifdef STL_MODULE
import std;
#else
#include <concepts>
#endif

export module meta_utils:meta_func;

import :concepts;

export namespace meta_utils {

template <typename T>
concept alias_r = requires { typename T::type; };

template <typename T, typename Value>
concept constant_r = requires {
    { T::value } -> std::convertible_to<Value>;
};

template <typename T, typename... Args>
concept map_r = requires { typename T::template type<Args...>; };

template <typename T, typename Result, typename... Args>
concept function_r = requires {
    { T::template operator()<Args...>() } -> std::convertible_to<Result>;
};

template <typename T, typename... Args>
concept predicate_r = requires {
    { T::template operator()<Args...>() } -> same_cvref_r<bool>;
};

template <typename T, typename... Args>
[[nodiscard]] consteval auto
invoke()
{
    return T::template operator()<Args...>();
}

template <template <typename...> typename Map>
struct make_map
{
    template <typename... T>
    using type = typename Map<T...>::type;
};

template <template <typename...> typename Func>
struct make_function
{
    template <typename... T>
    [[nodiscard]] static consteval auto
    operator()()
    {
        return Func<T...>::value;
    }
};

template <typename Target>
struct is_a
{
    template <typename T>
    [[nodiscard]] static consteval bool
    operator()()
    {
        return std::same_as<T, Target>;
    }
};

template <typename Pred>
struct negate
{
    template <typename... T>
    [[nodiscard]] static consteval bool
    operator()()
    {
        return !Pred::template operator()<T...>();
    }
};

template <typename... Preds>
struct all
{
    template <typename... T>
    [[nodiscard]] static consteval bool
    operator()()
    {
        auto eval_one = []<typename Pred> -> bool {
            return Pred::template operator()<T...>();
        };
        return ((eval_one.template operator()<Preds>()) && ... && true);
    }
};

template <typename... Preds>
struct any
{
    template <typename... T>
    [[nodiscard]] static consteval bool
    operator()()
    {
        auto eval_one = []<typename Pred> -> bool {
            return Pred::template operator()<T...>();
        };
        return ((eval_one.template operator()<Preds>()) || ... || false);
    }
};

} // namespace meta_utils
