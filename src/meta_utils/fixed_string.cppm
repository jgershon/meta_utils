module;

#ifdef STL_MODULE
import std;
#else
#include <algorithm>
#include <cstddef>
#include <string_view>
#endif

export module meta_utils:fixed_string;

export namespace meta_utils {

template <std::size_t N>
struct fixed_string
{
    char data[N + 1];

    constexpr fixed_string(char const (&s)[N + 1]) noexcept
    {
        std::copy_n(s, N + 1, data);
    }

    [[nodiscard]] constexpr bool
    operator==(fixed_string const & x) const = default;

    [[nodiscard]] constexpr
    operator char const *() const
    {
        return data;
    }

    [[nodiscard]] constexpr
    operator std::string_view() const
    {
        return {data, N};
    }

    [[nodiscard]] constexpr std::size_t
    size() const
    {
        return N;
    }
}; // struct fixed_string

template <std::size_t N>
fixed_string(char const (&)[N]) -> fixed_string<N - 1>;

} // namespace meta_utils
