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
import :trait;

namespace meta_utils::tuple {

template <typename>
struct is_tuple_ : std::false_type
{};

template <typename... T>
struct is_tuple_<std::tuple<T...>> : std::true_type
{};

template <typename, typename>
struct is_predicate_ : std::false_type
{};

template <typename T, typename... Elements>
    requires(trait::predicate_r<T, Elements> && ...)
struct is_predicate_<T, std::tuple<Elements...>> : std::true_type
{};

export {

    using empty = std::tuple<>;

    template <typename T>
    concept tuple_r = is_tuple_<T>::value;

    template <tuple_r Tuple>
    constexpr auto size = std::tuple_size_v<Tuple>;

    template <typename T, typename Tuple>
    concept predicate_r = is_predicate_<T, Tuple>::value;

} // export

template <typename, typename>
struct contains_ : std::false_type
{};

template <typename... T1, typename T2>
    requires(std::same_as<T1, T2> || ...)
struct contains_<std::tuple<T1...>, T2> : std::true_type
{};

template <typename, typename>
struct count_
{};

template <typename... T1, typename T2>
struct count_<std::tuple<T1...>, T2>
{
    static constexpr auto value =
        (static_cast<size_t>(std::same_as<T1, T2>) + ...);
};

template <typename...>
struct cat_
{};

template <>
struct cat_<>
{
    using type = empty;
};

template <typename... T>
    requires(!tuple_r<T> && ... && true)
struct cat_<T...>
{
    using type = std::tuple<T...>;
};

template <typename... T>
struct cat_<std::tuple<T...>>
{
    using type = std::tuple<T...>;
};

template <typename... T1, typename... T2, typename... T3>
struct cat_<std::tuple<T1...>, std::tuple<T2...>, T3...>
    : cat_<std::tuple<T1..., T2...>, typename cat_<T3...>::type>
{};

template <typename T1, typename... T2>
    requires(!tuple_r<T1> && (tuple_r<T2> || ... || false))
struct cat_<T1, T2...> : cat_<std::tuple<T1>, typename cat_<T2...>::type>
{};

template <typename, typename>
struct select_
{};

template <std::size_t... I, typename Tuple>
struct select_<std::index_sequence<I...>, Tuple>
{
    using type = std::tuple<std::tuple_element_t<I, Tuple>...>;
};

export {

    template <typename Tuple, typename T>
    concept contains_r = contains_<Tuple, T>::value;

    template <typename... T>
    using cat = typename cat_<T...>::type;

    template <std::size_t I, tuple_r Tuple>
    using at = std::tuple_element_t<I, Tuple>;

    template <tuple_r Tuple>
    using front = at<0, Tuple>;

    template <tuple_r Tuple>
    using back = at<size<Tuple> - 1, Tuple>;

    template <sequence::sequence_r<std::size_t> Indexes, tuple_r Tuple>
    using select = typename select_<Indexes, Tuple>::type;

    template <std::size_t Len, tuple_r Tuple>
    using head = select<std::make_index_sequence<Len>, Tuple>;

    template <std::size_t Len, tuple_r Tuple>
    using tail =
        select<sequence::make_index_sequence<size<Tuple> - Len, size<Tuple>>,
               Tuple>;

    template <tuple_r Tuple>
    using pop_front =
        select<sequence::make_index_sequence<1, size<Tuple>>, Tuple>;

    template <tuple_r Tuple>
    using pop_back =
        select<sequence::make_index_sequence<0, size<Tuple> - 1>, Tuple>;

    template <typename T, std::size_t I, tuple_r Tuple>
    using insert = cat<head<I, Tuple>, T, tail<size<Tuple> - I, Tuple>>;

    template <std::size_t I, tuple_r Tuple>
    using erase = cat<head<I, Tuple>, tail<size<Tuple> - (I + 1), Tuple>>;

} // export

template <typename, typename, bool>
struct where_
{};

template <typename... T, predicate_r<std::tuple<T...>> Pred, bool Criterion>
struct where_<std::tuple<T...>, Pred, Criterion>
{
    using type =
        sequence::sequence<bool, (Pred::template value<T> == Criterion)...>;
};

template <typename... T, typename V, bool Criterion>
    requires(!predicate_r<V, std::tuple<T...>>)
struct where_<std::tuple<T...>, V, Criterion>
{
    struct pred
    {
        template <typename T_>
        static constexpr auto value = std::same_as<T_, V>;
    };

    using type = typename where_<std::tuple<T...>, pred, Criterion>::type;
};

template <typename, typename>
struct unique_
{};

template <typename Unique>
struct unique_<empty, Unique>
{
    using type = Unique;
};

template <typename Tuple, typename Unique>
    requires(!contains_r<Unique, front<Tuple>>)
struct unique_<Tuple, Unique>
    : unique_<pop_front<Tuple>, cat<Unique, front<Tuple>>>
{};

template <typename Tuple, typename Unique>
    requires(contains_r<Unique, front<Tuple>>)
struct unique_<Tuple, Unique> : unique_<pop_front<Tuple>, Unique>
{};

export {

    template <tuple_r Tuple, typename T, bool Criterion = true>
    using where = typename where_<Tuple, T, Criterion>::type;

    template <tuple_r Tuple, typename T, bool Criterion = true>
    using filter = select<where<Tuple, T, Criterion>, Tuple>;

    template <tuple_r Tuple, typename T, bool Criterion = true>
    using erase_if = filter<Tuple, T, !Criterion>;

    template <tuple_r Tuple, typename T, bool Criterion = true>
    constexpr auto count_if = where<Tuple, T, Criterion>::size();

    template <tuple_r Tuple, typename T, bool Criterion = true>
    constexpr bool any_of = count_if<Tuple, T, Criterion> > 0;

    template <tuple_r Tuple, typename T, bool Criterion = true>
    constexpr bool all_of = count_if<Tuple, T, Criterion> == size<Tuple>;

    template <tuple_r Tuple>
    using unique = typename unique_<Tuple, empty>::type;
} // export

template <typename, typename, std::size_t, bool>
struct find_
{};

template <typename Tuple, typename T, std::size_t I, bool Criterion>
    requires(count_if<Tuple, T, Criterion> > I)
struct find_<Tuple, T, I, Criterion>
{
    static constexpr bool value = sequence::at<where<Tuple, T, Criterion>, I>;
};

template <typename Tuple, typename T, std::size_t I, bool Criterion>
    requires(count_if<Tuple, T, Criterion> <= I)
struct find_<Tuple, T, I, Criterion>
{
    static constexpr bool value = size<Tuple>;
};

template <typename, typename, std::size_t, bool>
struct find_last_
{};

template <typename Tuple, typename T, std::size_t I, bool Criterion>
    requires(count_if<Tuple, T, Criterion> > I)
struct find_last_<Tuple, T, I, Criterion>
{
    static constexpr bool value =
        sequence::at<where<Tuple, T, Criterion>,
                     count_if<Tuple, T, Criterion> - (I + 1)>;
};

template <typename Tuple, typename T, std::size_t I, bool Criterion>
    requires(count_if<Tuple, T, Criterion> <= I)
struct find_last_<Tuple, T, I, Criterion>
{
    static constexpr bool value = size<Tuple>;
};

export {

    template <tuple_r Tuple, typename T, bool Criterion = true>
    constexpr auto find = find_<Tuple, T, 0, Criterion>::value;

    template <tuple_r Tuple, typename T, std::size_t I, bool Criterion = true>
    constexpr auto find_first = find_<Tuple, T, I, Criterion>::value;

    template <tuple_r Tuple, typename T, std::size_t I, bool Criterion = true>
    constexpr auto find_last = find_last_<Tuple, T, I, Criterion>::value;

} // export

} // namespace meta_utils::tuple
