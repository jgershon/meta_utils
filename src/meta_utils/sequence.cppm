module;

#ifdef STL_MODULE
import std;
#else
#include <algorithm>
#include <array>
#include <cstddef>
#include <functional>
#include <utility>
#endif

export module meta_utils:sequence;

import :concepts;
import :math;

namespace meta_utils::sequence {

template <typename T, T... Vals>
struct value_sequence_
{
    using value_type = T;

    [[nodiscard]] static constexpr auto
    size()
    {
        return sizeof...(Vals);
    }
};

template <typename>
struct to_value_sequence_
{};

template <typename T, T... Vals>
struct to_value_sequence_<value_sequence_<T, Vals...>>
{
    using type = value_sequence_<T, Vals...>;
};

template <typename T, T... Vals>
struct to_value_sequence_<std::integer_sequence<T, Vals...>>
{
    using type = value_sequence_<T, Vals...>;
};

template <typename Seq>
using in_ = typename to_value_sequence_<Seq>::type;

template <typename T, T... Vals>
struct sequence_
{
    using type = value_sequence_<T, Vals...>;
};

template <std::integral Int, Int... I>
struct sequence_<Int, I...>
{
    using type = std::integer_sequence<Int, I...>;
};

template <typename>
struct from_value_sequence_
{};

template <typename T, T... Vals>
struct from_value_sequence_<value_sequence_<T, Vals...>>
{
    using type = sequence_<T, Vals...>::type;
};

template <typename T>
using out_ = typename from_value_sequence_<typename T::type>::type;

template <typename>
struct is_sequence_ : std::false_type
{};

template <typename T, T... Vals>
struct is_sequence_<value_sequence_<T, Vals...>> : std::true_type
{};

template <typename T, T... Vals>
struct is_sequence_<std::integer_sequence<T, Vals...>> : std::true_type
{};

template <typename>
struct to_array_
{};

template <typename T, T... Vals>
struct to_array_<value_sequence_<T, Vals...>>
{
    [[nodiscard]] static consteval std::array<T, sizeof...(Vals)>
    operator()()
    {
        return {Vals...};
    }
};

template <typename T, T, typename = void>
struct from_array_
{};

template <typename T, T Array, std::size_t... I>
struct from_array_<T, Array, std::index_sequence<I...>>
{
    using type = value_sequence_<typename T::value_type, Array[I]...>;
};

template <typename T, T Array>
struct from_array_<T, Array>
    : from_array_<T, Array, std::make_index_sequence<std::tuple_size_v<T>>>
{};

export {

    template <typename T, T... Vals>
    using sequence = typename sequence_<T, Vals...>::type;

    template <typename Seq, typename T = void>
    concept sequence_r = requires {
        requires is_sequence_<Seq>::value;
        requires std::same_as<T, void> ||
                     std::same_as<typename Seq::value_type, T>;
    };

    template <typename T>
    concept arithmetic_sequence_r = requires {
        requires sequence_r<T>;
        requires arithmetic_r<typename T::value_type>;
    };

    template <sequence_r Seq>
    using array_type = std::array<typename Seq::value_type, Seq::size()>;

    template <sequence_r Seq>
    [[nodiscard]] consteval array_type<Seq>
    to_array(Seq seq = {})
    {
        return to_array_<in_<Seq>>();
    }

} // export

template <typename Seq, typename Func>
struct apply_
{
    using result_array = std::invoke_result_t<Func, Seq>;
    using type = typename from_array_<result_array, [] {
        return Func{}(to_array<Seq>());
    }()>::type;
};

template <typename Seq, typename Op>
struct transform_
{
    using result_value = std::invoke_result_t<Op, typename Seq::value_type>;
    using result_array = std::array<result_value, Seq::size()>;
    using type = typename from_array_<result_array, [] {
        auto const array_in = to_array<Seq>();
        auto array_out = std::array<result_value, Seq::size()>{};
        std::ranges::transform(array_in, array_out.begin(), Op{});
        return array_out;
    }()>::type;
};

export {

    template <sequence_r Seq, typename Func>
    using apply = out_<apply_<in_<Seq>, Func>>;

    template <sequence_r Seq,
              typename Comp = std::less<typename Seq::value_type>>
    using sort = apply<Seq, decltype([](auto array) {
                           std::ranges::sort(array, Comp{});
                           return array;
                       })>;

    template <sequence_r Seq>
    using reverse = apply<Seq, decltype([](auto array) {
                              std::ranges::reverse(array);
                              return array;
                          })>;

    template <sequence_r Seq, typename Func>
    using transform = out_<transform_<in_<Seq>, Func>>;

    template <sequence_r Seq, typename T>
    using as = out_<
        transform<Seq, decltype([](auto val) { return static_cast<T>(val); })>>;

    template <arithmetic_sequence_r Seq, typename Seq::value_type Offset>
    using translate =
        transform<Seq, decltype([](auto val) { return val + Offset; })>;

    template <arithmetic_sequence_r Seq, typename Seq::value_type Offset>
        requires(Offset >= 0)
    using translate_up = translate<Seq, Offset>;

    template <arithmetic_sequence_r Seq, typename Seq::value_type Offset>
        requires(Offset >= 0)
    using translate_down =
        transform<Seq, decltype([](auto val) { return val - Offset; })>;

    template <arithmetic_sequence_r Seq, typename Seq::value_type Scale>
    using scale =
        transform<Seq, decltype([](auto val) { return val * Scale; })>;

    template <typename T, typename Seq>
    concept predicate_r = requires {
        requires sequence_r<Seq>;
        requires same_cvref_r<std::invoke_result<T, typename Seq::value_type>,
                              bool>;
    };

} // export

template <typename...>
struct cat_
{};

template <typename T, T... Vals>
struct cat_<value_sequence_<T, Vals...>>
{
    using type = value_sequence_<T, Vals...>;
};

template <typename T, T... Vals1, T... Vals2, typename... Seqs>
struct cat_<value_sequence_<T, Vals1...>, value_sequence_<T, Vals2...>, Seqs...>
    : cat_<value_sequence_<T, Vals1..., Vals2...>, Seqs...>
{};

template <typename, typename>
struct select_
{};

template <typename T, T... Vals, std::size_t... I>
    requires((I < sizeof...(Vals)) && ...)
struct select_<value_sequence_<T, Vals...>, std::index_sequence<I...>>
{
    using type = value_sequence_<T, Vals...[I]...>;
};

template <typename, std::size_t>
struct at_
{};

template <typename T, T... Vals, std::size_t I>
struct at_<value_sequence_<T, Vals...>, I>
{
    static constexpr auto value = Vals...[I];
};

template <typename T, T, T, T>
struct make_sequence_
{
    using type = value_sequence_<T>;
};

template <typename T, T Begin, T End, T Step>
    requires((End >= Begin) && (Step > 0))
struct make_sequence_<T, Begin, End, Step>
{
    static constexpr auto num_steps =
        static_cast<std::size_t>(floor((End - Begin + Step - 1) / Step));
    using result_array = std::array<T, num_steps>;
    using type = typename from_array_<result_array, [] {
        result_array array{};
        std::size_t index = 0;
        for (T value = Begin; value < End; value += Step)
        {
            array[index++] = value;
        }
        return array;
    }()>::type;
};

template <typename T, T Begin, T End, T Step>
    requires((Begin > End) && (Step != 0))
struct make_sequence_<T, Begin, End, Step>
{
    static constexpr auto abs_step = abs(Step);
    static constexpr auto num_steps =
        static_cast<std::size_t>(floor(Begin - End + abs_step - 1) / abs_step);
    static constexpr auto smallest = Begin - abs_step * (num_steps - 1);
    static constexpr auto after_largest = Begin + 1;
    using type = reverse<
        typename make_sequence_<T, smallest, after_largest, abs_step>::type>;
};

export {

    template <arithmetic_r T, T Begin, T End, T Step = 1>
    using make_sequence = typename make_sequence_<T, Begin, End, Step>::type;

    template <std::size_t Begin, std::size_t End, std::size_t Step = 1>
    using make_index_sequence = make_sequence<std::size_t, Begin, End, Step>;

    template <double Begin, double End, double Step = 1.0>
    using make_double_sequence = make_sequence<double, Begin, End, Step>;

    template <sequence_r Seq, std::size_t I>
    constexpr auto at = at_<Seq, I>::value;

    template <sequence_r Seq>
    constexpr auto front = at<Seq, 0>;

    template <sequence_r Seq>
    constexpr auto back = at<Seq, Seq::size() - 1>;

    template <sequence_r Seq, sequence_r<typename Seq::value_type>... Seqs>
    using cat = out_<cat_<Seq, Seqs...>>;

    template <sequence_r Seq, sequence_r<std::size_t> I>
    using select = out_<select_<Seq, I>>;

    template <sequence_r Seq, std::size_t I>
    using head = select<Seq, make_index_sequence<0, I>>;

    template <sequence_r Seq, std::size_t I>
    using tail = select<Seq, make_index_sequence<Seq::size() - I, Seq::size()>>;

    template <sequence_r Seq>
    using pop_front = select<Seq, make_index_sequence<1, Seq::size()>>;

    template <sequence_r Seq>
    using pop_back = select<Seq, make_index_sequence<0, Seq::size() - 1>>;

    template <sequence_r Seq, std::size_t I, typename Seq::value_type Val>
    using insert = cat<head<Seq, I>, sequence<typename Seq::ValueType, Val>,
                       tail<Seq, Seq::size() - I>>;

    template <sequence_r Seq, std::size_t I>
    using erase = cat<head<Seq, I>, tail<Seq, Seq::size() - (I + 1)>>;

} // export

template <typename, typename>
struct where_
{};

template <sequence_r Seq, predicate_r<Seq> Pred>
struct where_<Seq, Pred> : where_<Seq, transform<Seq, Pred>>
{};

template <typename T>
struct where_<T, sequence<bool>>
{
    using type = T;
};

template <sequence_r Seq, sequence_r<bool> Mask>
    requires requires {
        requires Mask::size() == Seq::size();
        requires at<Mask, 0>;
    }
struct where_<Seq, Mask>
{
    using type = cat<head<Seq, 1>,
                     typename where_<pop_front<Seq>, pop_front<Mask>>::type>;
};

template <sequence_r Seq, sequence_r<bool> Mask>
    requires requires {
        requires Mask::size() == Seq::size();
        requires !at<Mask, 0>;
    }
struct where_<Seq, Mask>
{
    using type = typename where_<pop_front<Seq>, pop_front<Mask>>::type;
};

export {

    template <sequence_r Seq, typename T>
        requires requires {
            requires(sequence_r<T, bool> && (T::size() == Seq::size())) ||
                        predicate_r<T, Seq>;
        }
    using where = out_<where_<in_<Seq>, T>>;

} // export

} // namespace meta_utils::sequence
