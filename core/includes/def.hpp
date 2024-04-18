#ifndef RG_CORE_DEF_HPP
#define RG_CORE_DEF_HPP

#include <stdint.h>
#include <functional>

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

    template <class R, class... Args>
    using Function = R(Args...);

    struct X
    {
        /* Dummy type. */
    };

    struct Y
    {
        /* Dummy type. */
    };

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

    template <class R, class T, class... Args>
    concept IsFunction = requires(const T &p_object, Args... p_items) {
        {
            p_object(p_items...)
        } -> IsSame<R>;
    };

    template <class T>
    concept IsElementTypeAvailable = requires {
        typename T::ElementType;
    };

    template <class T>
    concept IsValueTypeAvailable = requires {
        typename T::ValueType;
    };

    template <class T>
    concept IsKeyTypeAvailable = requires {
        typename T::KeyType;
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

    template <class T>
    concept IsDataAccessible = IsElementTypeAvailable<T> && requires(const T &p_object) {
        {
            p_object.data
        } -> IsSame<typename T::ElementType *>;
    };

    template <class T>
    concept IsDataAvailable = IsElementTypeAvailable<T> && requires(const T &p_object) {
        {
            p_object.view_data()
        } -> IsSame<const typename T::ElementType *>;
    };

    template <class T>
    concept IsConstructableFromSpan = IsValueTypeAvailable<T> && requires(const T::ValueType *p_pointer, U p_count) {
        {
            T(p_pointer, p_count)
        } -> IsSame<T>;
    };

    template <class T, class W>
    concept IsEqualAvailable = requires(const T &p_left, const W &p_right) {
        {
            p_left == p_right
        } -> IsSame<B>;
    };

    template <class T, class W>
    concept IsGreaterAvailable = requires(const T &p_left, const W &p_right) {
        {
            p_left > p_right
        } -> IsSame<B>;
    };

    template <class T, class W>
    concept IsGreaterEqualAvailable = requires(const T &p_left, const W &p_right) {
        {
            p_left >= p_right
        } -> IsSame<B>;
    };

    template <class T, class W>
    concept IsLessAvailable = requires(const T &p_left, const W &p_right) {
        {
            p_left < p_right
        } -> IsSame<B>;
    };

    template <class T, class W>
    concept IsLessEqualAvailable = requires(const T &p_left, const W &p_right) {
        {
            p_left <= p_right
        } -> IsSame<B>;
    };

    template <class T, class W>
    concept IsComparable = IsEqualAvailable<T, W> && IsGreaterAvailable<T, W> && IsGreaterEqualAvailable<T, W> && IsLessAvailable<T, W> && IsLessEqualAvailable<T, W>;

    template <class T, class W = T>
    concept IsSliceAvailable = IsKeyTypeAvailable<T> && requires(const T &p_object, const T::KeyType &p_begin, const T::KeyType &p_end) {
        {
            p_object.slice(p_begin, p_end)
        } -> IsSame<W>;
    };

    template <class T>
    concept IsIndexAvailable = IsKeyTypeAvailable<T> && IsValueTypeAvailable<T> && requires(const T &p_object, const T::KeyType &p_index) {
        {
            p_object[p_index]
        } -> IsSame<const typename T::ValueType &>;
    };

    template <class T>
    concept IsContainsAvailable = IsValueTypeAvailable<T> && requires(const T &p_object, const T::ValueType &p_thing) {
        {
            p_object.contains(p_thing)
        } -> IsSame<B>;
    };

    template <class T, class L>
    concept IsForEachAvailable =
        IsKeyTypeAvailable<T> &&
        IsValueTypeAvailable<T> &&
        IsFunction<void, L, const typename T::KeyType &, const typename T::ValueType &> &&
        requires(const T &p_object, const C &p_callable) {
            {
                p_object.for_each(p_callable)
            } -> IsSame<void>;
        };

    template <class R, class T, class L, class W = T>
    concept IsMapAvailable =
        IsKeyTypeAvailable<T> &&
        IsValueTypeAvailable<T> &&
        IsFunction<R, L, const typename T::KeyType &, const typename T::ValueType &> &&
        requires(const T &p_object, const C &p_callable) {
            {
                p_object.template map<R>(p_callable)
            } -> IsSame<W>;
        };

    template <class T, class L, class W = T>
    concept IsFilterAvailable =
        IsKeyTypeAvailable<T> &&
        IsValueTypeAvailable<T> &&
        IsFunction<B, L, const typename T::KeyType &, const typename T::ValueType &> && requires(const T &p_object, const C &p_callable) {
            {
                p_object.filter(p_callable)
            } -> IsSame<W>;
        };

    template <class T, class W = T, class R = T>
    concept IsConcatAvailable = requires(const T &p_left, const W &p_right) {
        {
            p_left.concat(p_right)
        } -> IsSame<R>;
    };

    template <class T>
    concept IsCapacityAvailable = requires(const T &p_object) {
        {
            p_object.get_capacity()
        } -> IsSame<U>;
    };

    template <class T>
    concept IsCapacityAccessible = requires(const T &p_object) {
        {
            p_object.capacity
        } -> IsSame<U>;
    };

    template <class T>
    concept IsReserveAvailable = requires(T p_object, U p_min_capacity) {
        {
            p_object.reserve(p_min_capacity)
        } -> IsSame<void>;
    };

    template <class T>
    concept IsCleanAvailable = requires(T p_object) {
        {
            p_object.clean()
        } -> IsSame<void>;
    };

    template <class T>
    concept IsInsertAvailable =
        IsKeyTypeAvailable<T> &&
        IsValueTypeAvailable<T> &&
        requires(T p_object, const T::KeyType &p_key, const T::ValueType &p_value) {
            {
                p_object.insert(p_key, p_value)
            } -> IsSame<void>;
        };

    template <class T>
    concept IsAppendAvailable =
        IsValueTypeAvailable<T> &&
        requires(T p_object, const T::ValueType &p_value) {
            {
                p_object.append(p_value)
            } -> IsSame<void>;
        };

    template <class T>
    concept IsPrependAvailable =
        IsValueTypeAvailable<T> &&
        requires(T p_object, const T::ValueType &p_value) {
            {
                p_object.prepend(p_value)
            } -> IsSame<void>;
        };

    template <class T>
    concept IsRemoveAvailable =
        IsKeyTypeAvailable<T> &&
        IsValueTypeAvailable<T> &&
        requires(T p_object, const T::KeyType &p_key) {
            {
                p_object.remove(p_key)
            } -> IsSame<typename T::ValueType>;
        };

    template <class T>
    concept IsPopBackAvailable =
        IsValueTypeAvailable<T> &&
        requires(T p_object) {
            {
                p_object.pop_back()
            } -> IsSame<typename T::ValueType>;
        };

    template <class T>
    concept IsPopFrontAvailable =
        IsValueTypeAvailable<T> &&
        requires(T p_object) {
            {
                p_object.pop_front()
            } -> IsSame<typename T::ValueType>;
        };

    template <class T>
    concept IsSetAvailable =
        IsKeyTypeAvailable<T> &&
        IsValueTypeAvailable<T> &&
        requires(T p_object, const T::KeyType &p_key, const T::ValueType &p_value) {
            {
                p_object.set(p_key, p_value)
            } -> IsSame<void>;
        };

    template <class T>
    constexpr auto move(T &p_object) noexcept -> Referenceless<T>::Type { return static_cast<typename Referenceless<T>::Type &&>(p_object); }

    template <class T>
    constexpr auto forward(T &p_object) noexcept -> T && { return static_cast<T &&>(p_object); }

} // namespace Rong

#endif // RG_CORE_DEF_HPP