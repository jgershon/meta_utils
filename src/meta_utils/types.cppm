module;

#include <concepts>
#include <type_traits>

export module meta_utils:types;

export namespace meta_utils {

template <typename T>
concept arithmetic_r = std::is_arithmetic_v<T>;

template <typename T1, typename T2>
concept same_cvref_r =
    std::same_as<std::remove_cvref_t<T1>, std::remove_cvref_t<T2>>;

template <typename T, typename Base>
concept strictly_derived_from_r =
    std::derived_from<std::remove_cvref_t<T>, std::remove_cvref_t<Base>> &&
    !same_cvref_r<T, Base>;

} // namespace meta_utils
