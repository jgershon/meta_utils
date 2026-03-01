module;

#ifdef STL_MODULE
import std;
#else
#include <concepts>
#include <utility>
#endif

export module meta_utils:overload;

namespace meta_utils {

export {

    template <typename... T>
    struct overload : T...
    {
        using T::operator()...;
    };

    template <typename... Funcs>
    [[nodiscard]] constexpr auto
    make_overload(Funcs &&... funcs)
    {
        return overload<Funcs...>{std::forward<Funcs>(funcs)...};
    }

    template <typename T, typename... Args>
    concept overload_r =
        requires { requires(std::invocable<T, Args> && ... && true); };

} // export

} // namespace meta_utils
