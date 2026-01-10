module;

#include <utility>

export module meta_utils:sequence;

import :const_math;

namespace meta_utils::sequence::detail {

template <typename T, T... Vals>
struct value_sequence
{
    using value_type = T;

    [[nodiscard]] static constexpr auto
    size()
    {
        return sizeof...(Vals);
    }
};

template <typename T, T... Vals>
struct sequence
{
    using type = typename value_sequence<T, Vals...>::type;
};

template <std::integral Int, Int... I>
struct sequence<Int, I...>
{
    using type = std::integer_sequence<Int, I...>;
};

} // namespace meta_utils::sequence::detail

export namespace meta_utils::sequence {

template <typename T, T... Vals>
using sequence = typename detail::sequence<T, Vals...>::type;

} // namespace meta_utils::sequence

namespace meta_utils::sequence::detail {

template <typename>
struct is_sequence : std::false_type
{};

template <typename T, T... Vals>
struct is_sequence<sequence<T, Vals...>> : std::true_type
{};

template <typename Seq, typename Op>
struct transform
{};

template <typename T, T... Vals, typename Op>
struct transform<sequence<T, Vals...>, Op>
{
    using type = sequence<T, Op::template operator()<Vals>()...>;
};

template <typename T, T Offset>
struct add
{
    template <T I>
    [[nodiscard]] static constexpr auto
    operator()()
    {
        return I + Offset;
    }
};

template <typename T, T Offset>
struct subtract
{
    template <T I>
    [[nodiscard]] static constexpr auto
    operator()()
    {
        return I - Offset;
    }
};

template <typename T, T Pivot>
struct mirror
{
    template <T I>
    [[nodiscard]] static constexpr auto
    operator()()
    {
        return Pivot - I;
    }
};

template <typename T, T Factor>
struct multiply
{
    template <T I>
    [[nodiscard]] static constexpr auto
    operator()()
    {
        return I * Factor;
    }
};

template <typename, typename>
struct cat
{};

template <typename T, T... Vals1, T... Vals2>
struct cat<sequence<T, Vals1...>, sequence<T, Vals2...>>
{
    using type = sequence<T, Vals1..., Vals2...>;
};

template <typename Seq, typename Seq::value_type Val>
struct append : cat<Seq, sequence<typename Seq::value_type, Val>>
{};

template <typename Seq, typename Seq::value_type Val>
struct prepend : cat<sequence<typename Seq::value_type, Val>, Seq>
{};

template <typename, typename>
struct select
{};

template <typename T, T... Vals, size_t... I>
    requires((I < sizeof...(Vals)) && ...)
struct select<sequence<T, Vals...>, std::index_sequence<I...>>
{
    using type = sequence<T, Vals...[I]...>;
};

template <typename>
struct reverse
{};

template <typename T, T... Vals>
struct reverse<sequence<T, Vals...>>
{
    static constexpr auto size = sizeof...(Vals);
    using type = typename select<
        sequence<T, Vals...>,
        typename mirror<std::make_index_sequence<size>, size - 1>::type>::type;
};

} // namespace meta_utils::sequence::detail

export namespace meta_utils::sequence {

template <typename Seq, typename T = void>
concept sequence_r = requires {
    requires detail::is_sequence<Seq>::value;
    requires std::same_as<T, void> || std::same_as<typename Seq::value_type, T>;
};

template <sequence_r Seq, typename Func>
using transform = typename detail::transform<Seq, Func>::type;

template <sequence_r Seq, typename Seq::value_type Offset>
    requires(arithmetic_r<typename Seq::value_type>)
using translate = typename detail::transform<
    Seq, detail::add<typename Seq::value_type, Offset>>::type;

template <sequence_r Seq, typename Seq::value_type Offset>
    requires(arithmetic_r<typename Seq::value_type> &&
             (Offset >= typename Seq::value_type{0}))
using translate_up = typename detail::transform<
    Seq, detail::add<typename Seq::value_type, Offset>>::type;

template <sequence_r Seq, typename Seq::value_type Offset>
    requires(arithmetic_r<typename Seq::value_type> &&
             (Offset >= typename Seq::value_type{0}))
using translate_down = typename detail::transform<
    Seq, detail::subtract<typename Seq::value_type, Offset>>::type;

template <sequence_r Seq, typename Seq::value_type Pivot>
    requires(arithmetic_r<typename Seq::value_type>)
using mirror = typename detail::transform<
    Seq, detail::mirror<typename Seq::value_type, Pivot>>::type;

template <sequence_r Seq, typename Seq::value_type Scale>
    requires(arithmetic_r<typename Seq::value_type>)
using scale = typename detail::transform<
    Seq, detail::multiply<typename Seq::value_type, Scale>>::type;

template <sequence_r Seq1, sequence_r<typename Seq1::value_type> Seq2>
using cat = typename detail::cat<Seq1, Seq2>::type;

template <sequence_r Seq, typename Seq::value_type Val>
using append = typename detail::append<Seq, Val>::type;

template <sequence_r Seq, typename Seq::value_type Val>
using prepend = typename detail::prepend<Seq, Val>::type;

template <sequence_r Seq, sequence_r<size_t> Indexes>
using select = typename detail::select<Seq, Indexes>::type;

template <sequence_r Seq>
using reverse = typename detail::reverse<Seq>::type;

} // namespace meta_utils::sequence

namespace meta_utils::sequence::detail {

template <typename Int, Int, Int, Int>
struct make_sequence
{};

template <typename Int, Int Begin, Int End, Int Step>
    requires((End >= Begin) && (Step > 0))
struct make_sequence<Int, Begin, End, Step>
{
    static constexpr auto num_steps = (End - Begin + Step - 1) / Step;
    using value =
        translate<scale<std::make_integer_sequence<Int, num_steps>, Step>,
                  Begin>;
};

template <typename Int, Int Begin, Int End, Int Step>
    requires((Begin > End) && (Step != 0))
struct make_sequence<Int, Begin, End, Step>
{
    static constexpr auto abs_step = abs(Step);
    static constexpr auto num_steps = (Begin - End + abs_step - 1) / abs_step;
    static constexpr auto smallest = Begin - abs_step * (num_steps - 1);
    static constexpr auto after_largest = Begin + 1;
    using type = reverse<make_sequence<Int, smallest, after_largest, abs_step>>;
};

} // namespace meta_utils::sequence::detail

export namespace meta_utils::sequence {

template <arithmetic_r T, T Begin, T End, T Step = 1>
using make_sequence = typename detail::make_sequence<T, Begin, End, Step>::type;

template <size_t Begin, size_t End, size_t Step = 1>
using make_index_sequence =
    typename detail::make_sequence<size_t, Begin, End, Step>::type;

template <double Begin, double End, double Step = 1.0>
using make_double_sequence =
    typename detail::make_sequence<double, Begin, End, Step>::type;

} // namespace meta_utils::sequence
