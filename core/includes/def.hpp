#ifndef RG_CORE_DEF_HPP
#define RG_CORE_DEF_HPP

#include <stdint.h>

namespace Rong
{
    using I8 = int8_t;
    using I16 = int16_t;
    using I32 = int32_t;
    using I64 = int64_t;
    using I = I32;

    using U8 = uint8_t;
    using U16 = uint16_t;
    using U32 = uint32_t;
    using U64 = uint64_t;
    using U = U32;

    using F32 = float;
    using F64 = double;
    using F = F32;

    using C = char;
    using B = bool;

    using Nullptr = decltype(nullptr);

    template <class T, T v>
    struct Item
    {
        using Type = T;
        static constexpr const Type value = v;
    };

    using FalseItem = Item<B, false>;
    using TrueItem = Item<B, true>;

    template <class T>
    struct Referenceless
    {
        using Type = T;
    };

    template <class T>
    struct Referenceless<T &>
    {
        using Type = T;
    };

    template <class T>
    struct Referenceless<T &&>
    {
        using Type = T;
    };

    template <class T, class V>
    struct Same : FalseItem
    {
    };

    template <class T>
    struct Same<T, T> : TrueItem
    {
    };

    template <class T, class V>
    concept IsSame = Same<T, V>::value && Same<V, T>::value;

    template <class From, class To>
    concept IsConvertible = requires(const From &p_object) {
        {
            To(p_object)
        } -> IsSame<To>;
    };

    template <class T>
    concept IsDefaultAvailable = requires {
        {
            T()
        } -> IsSame<T>;
    };

    template <class T>
    concept IsCountAccessible = requires(const T &p_object) {
        {
            p_object.count
        } -> IsSame<U>;
    };

    template <class T>
    concept IsCountAvailable = requires(const T &p_object) {
        {
            p_object.get_count()
        } -> IsSame<U>;
    };

    template <class T, class E>
    concept IsDataAccessible = requires(const T &p_object) {
        {
            p_object.data
        } -> IsSame<E *>;
    };

    template <class T, class E>
    concept IsDataAvailable = requires(const T &p_object) {
        {
            p_object.view_data()
        } -> IsSame<const E *>;
    };

    template <class T, class E>
    concept IsConstructableFromSpan = requires(E *p_pointer, U p_count) {
        {
            T(p_pointer, p_count)
        } -> IsSame<T>;
    };

    template <class T, class V>
    concept IsEqualAvailable = requires(const T &p_left, const V &p_right) {
        {
            p_left == p_right
        } -> IsSame<B>;
    };

    template <class T, class V>
    concept IsGreaterAvailable = requires(const T &p_left, const V &p_right) {
        {
            p_left > p_right
        } -> IsSame<B>;
    };

    template <class T, class V>
    concept IsGreaterEqualAvailable = requires(const T &p_left, const V &p_right) {
        {
            p_left >= p_right
        } -> IsSame<B>;
    };

    template <class T, class V>
    concept IsLessAvailable = requires(const T &p_left, const V &p_right) {
        {
            p_left < p_right
        } -> IsSame<B>;
    };

    template <class T, class V>
    concept IsLessEqualAvailable = requires(const T &p_left, const V &p_right) {
        {
            p_left <= p_right
        } -> IsSame<B>;
    };

    template <class T, class V>
    concept IsComparable = IsEqualAvailable<T, V> && IsGreaterAvailable<T, V> && IsGreaterEqualAvailable<T, V> && IsLessAvailable<T, V> && IsLessEqualAvailable<T, V>;

    template <class R, class T, class... Args>
    concept IsCallable = requires(const T &p_object, Args... p_items) {
        {
            p_object(p_items...)
        } -> IsSame<R>;
    };

    template <class T>
    concept IsElementTypeAccessbile = requires {
        typename T::ElementType;
    };

    template <class T>
    constexpr auto move(T &p_object) noexcept -> Referenceless<T>::Type { return static_cast<typename Referenceless<T>::Type &&>(p_object); }

    template <class T>
    constexpr auto forward(T &p_object) noexcept -> T && { return static_cast<T &&>(p_object); }

} // namespace Rong

#endif // RG_CORE_DEF_HPP