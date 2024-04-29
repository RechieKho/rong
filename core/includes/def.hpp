#ifndef RG_CORE_DEF_HPP
#define RG_CORE_DEF_HPP

#include <stdint.h>
#include <stdlib.h>

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

    struct Inconstructible
    {
        ~Inconstructible() = delete;
    };

    template <class T, T v>
    struct Item : Inconstructible
    {
        using Type = T;
        static constexpr const Type value = v;
    };

    using FalseItem = Item<B, false>;
    using TrueItem = Item<B, true>;

    template <class T>
    struct Referenceless : Inconstructible
    {
        using Type = T;
    };

    template <class T>
    struct Referenceless<T &> : Inconstructible
    {
        using Type = T;
    };

    template <class T>
    struct Referenceless<T &&> : Inconstructible
    {
        using Type = T;
    };

    template <class... T>
    struct Parameters : Inconstructible
    {
        template <template <class...> class W>
        using Apply = W<T...>;
    };

    template <U Index, class T, class... W>
    struct TypeAt : Inconstructible
    {
        static_assert(Index < (sizeof...(W) + 1), "Given index is beyond parameter pack's count.");

        template <template <class...> class V>
        using Rest = TypeAt<Index - 1, W...>::template Rest<V>;
        template <template <class...> class V>
        using Set = TypeAt<Index - 1, W...>::template Set<V>;
        using Type = TypeAt<Index - 1, W...>::Type;
    };

    template <class T, class... W>
    struct TypeAt<0, T, W...> : Inconstructible
    {
        template <template <class...> class V>
        using Rest = Parameters<W...>::template Apply<V>;
        template <template <class...> class V>
        using Set = Parameters<T, W...>::template Apply<V>;
        using Type = T;
    };

    template <class T, class... W>
    struct TypeIndex : Inconstructible
    {
    private:
        template <U Index, class TargetType, class CurrentType, class... RestTypes>
        struct IndexCounter : Inconstructible
        {
            static constexpr U INDEX = IndexCounter<Index + 1, TargetType, RestTypes...>::INDEX;
        };

        template <U Index, class TargetType, class... RestTypes>
        struct IndexCounter<Index, TargetType, TargetType, RestTypes...> : Inconstructible
        {
            static constexpr U INDEX = Index;
        };

    public:
        static constexpr U INDEX = IndexCounter<0, T, W...>::INDEX;
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

    template <class T, class W = T>
    concept IsEqualAvailable = requires(const T &p_left, const W &p_right) {
        {
            p_left == p_right
        } -> IsSame<B>;
    };

    template <class T, class W = T>
    concept IsNotEqualAvailable = requires(const T &p_left, const W &p_right) {
        {
            p_left == p_right
        } -> IsSame<B>;
    };

    template <class T, class W = T>
    concept IsGreaterAvailable = requires(const T &p_left, const W &p_right) {
        {
            p_left > p_right
        } -> IsSame<B>;
    };

    template <class T, class W = T>
    concept IsGreaterEqualAvailable = requires(const T &p_left, const W &p_right) {
        {
            p_left >= p_right
        } -> IsSame<B>;
    };

    template <class T, class W = T>
    concept IsLessAvailable = requires(const T &p_left, const W &p_right) {
        {
            p_left < p_right
        } -> IsSame<B>;
    };

    template <class T, class W = T>
    concept IsLessEqualAvailable = requires(const T &p_left, const W &p_right) {
        {
            p_left <= p_right
        } -> IsSame<B>;
    };

    template <class T, class W = T>
    concept IsComparable = IsEqualAvailable<T, W> && IsNotEqualAvailable<T, W> && IsGreaterAvailable<T, W> && IsGreaterEqualAvailable<T, W> && IsLessAvailable<T, W> && IsLessEqualAvailable<T, W>;

    template <class T, class W = T>
    concept IsSliceAvailable = IsKeyTypeAvailable<T> && requires(const T &p_object, const T::KeyType &p_begin, const T::KeyType &p_end) {
        {
            p_object.slice(p_begin, p_end)
        } -> IsSame<W>;
    };

    template <class T, class R = const typename T::ValueType &>
    concept IsIndexAvailable = IsKeyTypeAvailable<T> && requires(T p_object, const T::KeyType &p_index) {
        {
            p_object[p_index]
        } -> IsSame<R>;
    };

    template <class T, class R = const typename T::ValueType &>
    concept IsDerefAvailable = requires(T p_object) {
        {
            *p_object
        } -> IsSame<R>;
    };

    template <class T>
    concept IsIncrememntAvailable = requires(T &p_object) {
        {
            ++p_object
        } -> IsSame<T>;
    };

    template <class T>
    concept IsDecrementAvailable = requires(T &p_object) {
        {
            --p_object
        } -> IsSame<T>;
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
    constexpr auto move(T &p_object) -> Referenceless<T>::Type { return static_cast<typename Referenceless<T>::Type &&>(p_object); }

    template <class T>
    constexpr auto forward(T &p_object) -> T && { return static_cast<T &&>(p_object); }

    template <class T, class R = const typename T::ValueType &>
    concept IsForwardIterator =
        IsDerefAvailable<T, R> &&
        IsIncrememntAvailable<T> &&
        IsEqualAvailable<T, T>;

    template <class T, class R = const typename T::ValueType &>
    concept IsBidirectionalIterator =
        IsForwardIterator<T, R> &&
        IsDecrementAvailable<T>;

    template <class T, class R = const typename T::ValueType &>
    concept IsRandomAccessIterator =
        IsBidirectionalIterator<T, R> &&
        IsIndexAvailable<T, R>;

    template <class T, class R = const typename T::ValueType &>
    concept IsIteratorAvailable = requires(const T &p_object) {
        {
            p_object.cbegin()
        } -> IsForwardIterator<R>;

        {
            p_object.cend()
        } -> IsForwardIterator<R>;
    };

    template <class T, class R = typename T::ValueType &>
    concept IsIteratorAccessible = requires(T &p_object) {
        {
            p_object.begin()
        } -> IsForwardIterator<R>;

        {
            p_object.end()
        } -> IsForwardIterator<R>;
    };

    template <class T, class C, class R = const typename T::ValueType &>
        requires IsForwardIterator<T, R> && IsFunction<void, C, U, R>
    constexpr auto for_each(C &&p_callable, T p_begin, T p_end, U p_index = 0) -> void
    {
        if constexpr (p_begin != p_end)
        {
            p_callable(p_index, *p_begin);
            ++p_begin;
            for_each(forward<C>(p_callable), p_begin, p_end, p_index + 1);
        }
    }

} // namespace Rong

#endif // RG_CORE_DEF_HPP