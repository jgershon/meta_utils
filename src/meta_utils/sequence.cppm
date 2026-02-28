module;

#ifdef STL_MODULE
import std;
#else
#include <utility>
#endif

export module meta_utils:sequence;

import :const_math;

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

export {

    template <typename T, T... Vals>
    using sequence = typename sequence_<T, Vals...>::type;
}

template <typename>
struct is_sequence_ : std::false_type
{};

template <typename T, T... Vals>
struct is_sequence_<sequence_<T, Vals...>> : std::true_type
{};

template <typename Seq, typename Op>
struct transform_
{};

template <typename T, T... Vals, typename Op>
struct transform_<sequence_<T, Vals...>, Op>
{
    using type = sequence<T, Op::template operator()<Vals>()...>;
};

template <typename T, T Offset>
struct add_
{
    template <T I>
    [[nodiscard]] static constexpr auto
    operator()()
    {
        return I + Offset;
    }
};

template <typename T, T Offset>
struct subtract_
{
    template <T I>
    [[nodiscard]] static constexpr auto
    operator()()
    {
        return I - Offset;
    }
};

template <typename T, T Pivot>
struct mirror_
{
    template <T I>
    [[nodiscard]] static constexpr auto
    operator()()
    {
        return Pivot - I;
    }
};

template <typename T, T Factor>
struct multiply_
{
    template <T I>
    [[nodiscard]] static constexpr auto
    operator()()
    {
        return I * Factor;
    }
};

template <typename, typename>
struct cat_
{};

template <typename T, T... Vals1, T... Vals2>
struct cat_<sequence_<T, Vals1...>, sequence_<T, Vals2...>>
{
    using type = sequence<T, Vals1..., Vals2...>;
};

template <typename Seq, typename Seq::value_type Val>
struct append_ : cat_<Seq, sequence_<typename Seq::value_type, Val>>
{};

template <typename Seq, typename Seq::value_type Val>
struct prepend_ : cat_<sequence_<typename Seq::value_type, Val>, Seq>
{};

template <typename, typename>
struct select_
{};

template <typename T, T... Vals, size_t... I>
    requires((I < sizeof...(Vals)) && ...)
struct select_<sequence_<T, Vals...>, std::index_sequence<I...>>
{
    using type = sequence<T, Vals...[I]...>;
};

template <typename>
struct reverse_
{};

template <typename T, T... Vals>
struct reverse_<sequence_<T, Vals...>>
{
    static constexpr auto size = sizeof...(Vals);
    using type = typename select_<
        sequence<T, Vals...>,
        typename mirror_<std::make_index_sequence<size>, size - 1>::type>::type;
};

export {

    template <typename Seq, typename T = void>
    concept sequence_r = requires {
        requires is_sequence_<Seq>::value;
        requires std::same_as<T, void> ||
                     std::same_as<typename Seq::value_type, T>;
    };

    template <sequence_r Seq, typename Func>
    using transform = typename transform_<Seq, Func>::type;

    template <sequence_r Seq, typename Seq::value_type Offset>
        requires(arithmetic_r<typename Seq::value_type>)
    using translate = transform<Seq, add_<typename Seq::value_type, Offset>>;

    template <sequence_r Seq, typename Seq::value_type Offset>
        requires(arithmetic_r<typename Seq::value_type> &&
                 (Offset >= typename Seq::value_type{0}))
    using translate_up = transform<Seq, add_<typename Seq::value_type, Offset>>;

    template <sequence_r Seq, typename Seq::value_type Offset>
        requires(arithmetic_r<typename Seq::value_type> &&
                 (Offset >= typename Seq::value_type{0}))
    using translate_down =
        transform<Seq, subtract_<typename Seq::value_type, Offset>>;

    template <sequence_r Seq, typename Seq::value_type Pivot>
        requires(arithmetic_r<typename Seq::value_type>)
    using mirror = transform<Seq, mirror_<typename Seq::value_type, Pivot>>;

    template <sequence_r Seq, typename Seq::value_type Scale>
        requires(arithmetic_r<typename Seq::value_type>)
    using scale = transform<Seq, multiply_<typename Seq::value_type, Scale>>;

    template <sequence_r Seq1, sequence_r<typename Seq1::value_type> Seq2>
    using cat = typename cat_<Seq1, Seq2>::type;

    template <sequence_r Seq, typename Seq::value_type Val>
    using append = typename append_<Seq, Val>::type;

    template <sequence_r Seq, typename Seq::value_type Val>
    using prepend = typename prepend_<Seq, Val>::type;

    template <sequence_r Seq, sequence_r<size_t> Indexes>
    using select = typename select_<Seq, Indexes>::type;

    template <sequence_r Seq>
    using reverse = typename reverse_<Seq>::type;

} // export

template <typename Int, Int, Int, Int>
struct make_sequence_
{};

template <typename Int, Int Begin, Int End, Int Step>
    requires((End >= Begin) && (Step > 0))
struct make_sequence_<Int, Begin, End, Step>
{
    static constexpr auto num_steps = (End - Begin + Step - 1) / Step;
    using type =
        translate<scale<std::make_integer_sequence<Int, num_steps>, Step>,
                  Begin>;
};

template <typename Int, Int Begin, Int End, Int Step>
    requires((Begin > End) && (Step != 0))
struct make_sequence_<Int, Begin, End, Step>
{
    static constexpr auto abs_step = abs(Step);
    static constexpr auto num_steps = (Begin - End + abs_step - 1) / abs_step;
    static constexpr auto smallest = Begin - abs_step * (num_steps - 1);
    static constexpr auto after_largest = Begin + 1;
    using type = reverse<
        typename make_sequence_<Int, smallest, after_largest, abs_step>::type>;
};

export {

    template <arithmetic_r T, T Begin, T End, T Step = 1>
    using make_sequence = typename make_sequence_<T, Begin, End, Step>::type;

    template <size_t Begin, size_t End, size_t Step = 1>
    using make_index_sequence = make_sequence<size_t, Begin, End, Step>;

    template <double Begin, double End, double Step = 1.0>
    using make_double_sequence = make_sequence<double, Begin, End, Step>;

} // export

} // namespace meta_utils::sequence
