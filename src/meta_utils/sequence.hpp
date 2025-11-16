#pragma once

#include <concepts>
#include <type_traits>
#include <utility>

namespace meta_utils {

namespace internal {

template<typename>
struct is_integer_sequence
: std::false_type
{};

template<typename Int, Int... I>
struct is_integer_sequence<std::integer_sequence<Int, I...>>
: std::true_type
{};

template<typename Seq, typename Op>
struct sequence_transform
{};

template<typename Int, Int... I, typename Op>
struct sequence_transform<std::integer_sequence<Int, I...>, Op>
{
    using type = std::integer_sequence<Int, Op::template operator()<I>()...>;
};

template<typename Int, Int Offset>
struct integer_plus
{
    template <Int I>
    [[nodiscard]] static constexpr auto
    operator()()
    {
        return I + Offset;
    }
};

template<typename Int, Int Factor>
struct integer_times
{
    template <Int I>
    [[nodiscard]] static constexpr auto
    operator()()
    {
        return I * Factor;
    }
};

template<typename, typename>
struct sequence_cat
{};

template<typename Int, Int... I, Int... J>
struct sequence_cat<std::integer_sequence<Int, I...>, std::integer_sequence<Int, J...>>
{
    using type = std::integer_sequence<Int, I..., J...>;
};

template<typename Seq, typename Seq::value_type I>
struct sequence_append
: sequence_cat<Seq, std::integer_sequence<typename Seq::value_type, I>>
{};

template<typename>
struct sequence_reverse
{};

template<typename Int>
struct sequence_reverse<std::integer_sequence<Int>>
{
    using type = std::integer_sequence<Int>;
};

template<typename Int, Int First, Int... Rest>
struct sequence_reverse<std::integer_sequence<Int, First, Rest...>>
{
    using type = integer_append<Int, typename std::sequence_reverse<Rest...>::type, First>;
};

} // namespace internal

template<typename T, typename Int = void>
concept integer_sequence = requires
{
    requires internal::is_integer_sequence<T>::value;
    requires std::same_as<typename T::value_type, Int> || std::same_as<Int, void>;
};

template<integer_sequence Seq, typename Func>
using sequence_transform = typename internal::sequence_transform<Seq, Func>::type;

template<integer_sequence Seq, typename Seq::value_type Offset>
using sequence_translate = typename internal::sequence_transform<Seq, internal::integer_plus<typename Seq::value_type, Offset>>::type;

template<integer_sequence Seq, typename Seq::value_type Scale>
using sequence_scale = typename internal::sequence_transform<Seq, internal::integer_times<typename Seq::value_type, Factor>>::type;

template<integer_sequence Seq1, integer_sequence<typename Seq1::value_type> Seq2>
using sequence_cat = typename internal::sequence_cat<Seq1, Seq2>::type;

template<integer_sequence Seq, typename Seq::value_type I>
using sequence_append = typename internal::sequence_append<Seq, I>::type;

template<integer_sequence Seq>
using sequence_reverse = typename internal::sequence_reverse<Seq>::type;


namespace internal {

template<typename Int, Int, Int, Int>
struct make_integer_sequence {};

template<typename Int, Int Begin, Int End, Int Step>
requires ((End >= Begin) && (Step > 0))
struct make_integer_sequence<Int, Begin, End, Step>
{
    static constexpr auto num_steps = (End - Begin + Step - 1) / Step;
    using value = sequence_translate<sequence_scale<std::make_integer_sequence<Int, num_steps>, Step>, Begin>::type;
};

template<typename Int, Int Begin, Int End, Int Step>
requires ((Begin > End) && (Step != 0))
struct make_integer_sequence<Int, Begin, End, Step>
{
    static constexpr auto abs_step = abs(Step);
    static constexpr auto num_steps = (Begin - End + abs_step - 1) / abs_step;
    static constexpr auto smallest = Begin - abs_step * (num_steps - 1);
    static constexpr auto after_largest = Begin + 1;
    using type = sequence_reverse<make_integer_sequence<Int, smallest, after_largest, abs_step>::type>;
};

} // namespace internal

template<std::integral Int, Int Begin, Int End, Int Step = 1>
using make_integer_sequence = typename internal::make_integer_sequence<Int, Begin, End, Step>::type;

template<size_t Begin, size_t End, size_t Step = 1>
using make_index_sequence = typename internal::make_integer_sequence<size_t, Begin, End, Step>::type;

} // namespace meta_utils
