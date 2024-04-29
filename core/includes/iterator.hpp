#ifndef RG_CORE_ITERATOR_HPP
#define RG_CORE_ITERATOR_HPP

#include "def.hpp"
#include "exception.hpp"
#include "pair.hpp"

namespace Rong
{

    template <class T>
    class DummyIterator
    {
        /* Dummy iterator. */
    public:
        using ValueType = T;

    private:
    public:
        inline DummyIterator() = delete;
        inline auto operator*() const -> const ValueType & { throw Exception<LOGICAL>("`XIterator` is a dummy iterator."); }
        inline auto operator++() -> DummyIterator { throw Exception<LOGICAL>("`XIterator` is a dummy iterator."); }
        inline auto operator--() -> DummyIterator { throw Exception<LOGICAL>("`XIterator` is a dummy iterator."); }
        inline auto operator==(const DummyIterator &p_right) const -> B { throw Exception<LOGICAL>("`XIterator` is a dummy iterator."); }
    };

    template <class T>
    class AccessibleDummyIterator
    {
        /* Dummy iterator. */
    public:
        using ValueType = T;

    private:
    public:
        inline AccessibleDummyIterator() = delete;
        inline auto operator*() -> ValueType & { throw Exception<LOGICAL>("`XAccessibleIterator` is a dummy iterator."); }
        inline auto operator++() -> AccessibleDummyIterator { throw Exception<LOGICAL>("`XAccessibleIterator` is a dummy iterator."); }
        inline auto operator--() -> AccessibleDummyIterator { throw Exception<LOGICAL>("`XAccessibleIterator` is a dummy iterator."); }
        inline auto operator==(const AccessibleDummyIterator &p_right) const -> B { throw Exception<LOGICAL>("`XAccessibleIterator` is a dummy iterator."); }
    };

    template <class T, class R = const typename T::ValueType &>
    class IteratorWrapper
    {
    public:
        using ValueType = R;

    private:
        T begin_iterator;
        T end_iterator;

    public:
        constexpr IteratorWrapper(const T &p_begin_iterator, const T &p_end_iterator) : begin_iterator(p_begin_iterator), end_iterator(p_end_iterator) {}
        constexpr auto cbegin() const -> T { return begin_iterator; }
        constexpr auto cend() const -> T { return end_iterator; }
    };

    template <class T, class R = typename T::ValueType &>
        requires IsForwardIterator<T, R>
    class AccessibleIteratorWrapper
    {
    public:
        using ValueType = R;

    private:
        T begin_iterator;
        T end_iterator;

    public:
        constexpr AccessibleIteratorWrapper(const T &p_begin_iterator, const T &p_end_iterator) : begin_iterator(p_begin_iterator), end_iterator(p_end_iterator) {}
        constexpr auto begin() const -> T { return begin_iterator; }
        constexpr auto end() const -> T { return end_iterator; }
    };

    template <class T, class R = const typename T::ValueType &>
        requires IsForwardIterator<T, R>
    class EnumerateIterator
    {
    public:
        using ValueType = Pair<U, R>;

    private:
        T iterator;
        U index;

    public:
        constexpr EnumerateIterator(const T &p_iterator) : iterator(p_iterator), index(0) {}
        constexpr auto operator*() const -> ValueType { return ValueType(index, *iterator); }
        constexpr auto operator++() -> EnumerateIterator
        {
            ++iterator;
            ++index;
            return *this;
        }
        constexpr auto operator==(const EnumerateIterator &p_right) const -> B { return iterator == p_right.iterator; }
    };

    template <class T, class R = const typename T::ValueType &>
        requires IsForwardIterator<T, R>
    constexpr auto enumerate(T &&p_begin_iterator, T &&p_end_iterator)
    {
        return IteratorWrapper<EnumerateIterator<T, R>, typename EnumerateIterator<T, R>::ValueType>(forward<T>(p_begin_iterator), forward<T>(p_end_iterator));
    }

    template <class T, class R = typename T::ValueType &>
        requires IsForwardIterator<T, R>
    class AccessibleEnumerateIterator
    {
    public:
        using ValueType = Pair<U, R>;

    private:
        T iterator;
        U index;

    public:
        inline AccessibleEnumerateIterator(const T &p_iterator) : iterator(p_iterator), index(0) {}
        inline auto operator*() -> ValueType { return ValueType(index, *iterator); }
        inline auto operator++() -> AccessibleEnumerateIterator
        {
            ++iterator;
            ++index;
            return *this;
        }
        inline auto operator==(const AccessibleEnumerateIterator &p_right) const -> B { return iterator == p_right.iterator; }
    };

    template <class T, class R = typename T::ValueType &>
        requires IsForwardIterator<T, R>
    inline auto accessible_enumerate(T &&p_begin_iterator, T &&p_end_iterator)
    {
        return AccessibleIteratorWrapper<AccessibleEnumerateIterator<T, R>, typename AccessibleEnumerateIterator<T, R>::ValueType>(forward<T>(p_begin_iterator), forward<T>(p_end_iterator));
    }

    template <class T, class W, class R = const typename T::ValueType &, class S = const typename W::ValueType &>
        requires IsForwardIterator<T, R> && IsForwardIterator<W, S>
    class ZipIterator
    {
    public:
        using ValueType = Pair<R, S>;

    private:
        T first;
        W second;

    public:
        constexpr ZipIterator(const T &p_first, const W &p_second) : first(p_first), second(p_second) {}
        constexpr auto operator*() const -> ValueType { return ValueType(*first, *second); }
        constexpr auto operator++() -> ZipIterator
        {
            ++first;
            ++second;
            return *this;
        }
        constexpr auto operator==(const ZipIterator &p_right) const -> B { return first == p_right.first || second == p_right.second; }
    };

    template <class T, class W, class R = const typename T::ValueType &, class S = const typename W::ValueType &>
        requires IsForwardIterator<T, R> && IsForwardIterator<W, S>
    constexpr auto zip(T &&p_first_begin_iterator, T &&p_first_end_iterator, W &&p_second_begin_iterator, W &&p_second_end_iterator)
    {
        using Iterator = ZipIterator<T, W, R, S>;
        auto begin_iterator = Iterator(forward<T>(p_first_begin_iterator), forward<W>(p_second_begin_iterator));
        auto end_iterator = Iterator(forward<T>(p_first_end_iterator), forward<W>(p_second_end_iterator));
        return IteratorWrapper<Iterator, typename Iterator::ValueType>(begin_iterator, end_iterator);
    }

    template <class T, class W, class R = const typename T::ValueType &, class S = const typename W::ValueType &>
        requires IsIteratorAvailable<T, R> && IsIteratorAvailable<W, S>
    constexpr auto zip(const T &p_first_iterable, const W &p_second_iterable)
    {
        return zip(p_first_iterable.cbegin(), p_first_iterable.cend(), p_second_iterable.cbegin(), p_second_iterable.cend());
    }

    template <class T, class W, class R = typename T::ValueType &, class S = typename W::ValueType &>
        requires IsForwardIterator<T, R> && IsForwardIterator<W, S>
    class AccessibleZipIterator
    {
    public:
        using ValueType = Pair<R, S>;

    private:
        T first;
        W second;

    public:
        inline AccessibleZipIterator(const T &p_first, const W &p_second) : first(p_first), second(p_second) {}
        inline auto operator*() -> ValueType { return ValueType(*first, *second); }
        inline auto operator++() -> AccessibleZipIterator
        {
            ++first;
            ++second;
            return *this;
        }
        inline auto operator==(const AccessibleZipIterator &p_right) const -> B { return first == p_right.first || second == p_right.second; }
    };

    template <class T, class W, class R = typename T::ValueType &, class S = typename W::ValueType &>
        requires IsForwardIterator<T, R> && IsForwardIterator<W, S>
    inline auto accessible_zip(T &&p_first_begin_iterator, T &&p_first_end_iterator, W &&p_second_begin_iterator, W &&p_second_end_iterator)
    {
        using Iterator = AccessibleZipIterator<T, W, R, S>;
        auto begin_iterator = Iterator(forward<T>(p_first_begin_iterator), forward<W>(p_second_begin_iterator));
        auto end_iterator = Iterator(forward<T>(p_first_end_iterator), forward<W>(p_second_end_iterator));
        return AccessibleIteratorWrapper<Iterator, typename Iterator::ValueType>(begin_iterator, end_iterator);
    }

    template <class T, class W, class R = typename T::ValueType &, class S = typename W::ValueType &>
        requires IsIteratorAccessible<T, R> && IsIteratorAccessible<W, S>
    inline auto accessible_zip(T &p_first_iterable, W &p_second_iterable)
    {
        return accessible_zip(p_first_iterable.begin(), p_first_iterable.end(), p_second_iterable.begin(), p_second_iterable.end());
    }

    template <class T, class R = const typename T::ValueType &>
        requires IsBidirectionalIterator<T, R>
    class ReverseIterator
    {
    public:
        using ValueType = R;

    private:
        T iterator;

    public:
        constexpr ReverseIterator(const T &p_iterator) : iterator(p_iterator) {}
        constexpr auto operator*() const -> ValueType { return *iterator; }
        constexpr auto operator++() -> ReverseIterator
        {
            --iterator;
            return *this;
        }
        constexpr auto operator==(const ReverseIterator &p_right) const -> B { return iterator == p_right.iterator; }
    };

    template <class T, class R = const typename T::ValueType &>
        requires IsBidirectionalIterator<T, R>
    constexpr auto reverse(T &&p_begin_iterator, T &&p_end_iterator)
    {
        using Iterator = ReverseIterator<T, R>;
        return IteratorWrapper<Iterator, typename Iterator::ValueType>(Iterator(--p_end_iterator), Iterator(--p_begin_iterator));
    }

    template <class T, class R = typename T::ValueType &>
        requires IsBidirectionalIterator<T, R>
    class AccessibleReverseIterator
    {
    public:
        using ValueType = R;

    private:
        T iterator;

    public:
        inline AccessibleReverseIterator(const T &p_iterator) : iterator(p_iterator) {}
        inline auto operator*() -> ValueType { return *iterator; }
        inline auto operator++() -> AccessibleReverseIterator
        {
            --iterator;
            return *this;
        }
        inline auto operator==(const AccessibleReverseIterator &p_right) const -> B { return iterator == p_right.iterator; }
    };

    template <class T, class R = typename T::ValueType &>
        requires IsBidirectionalIterator<T, R>
    constexpr auto accessible_reverse(T &&p_begin_iterator, T &&p_end_iterator)
    {
        using Iterator = AccessibleReverseIterator<T, R>;
        return AccessibleIteratorWrapper<Iterator, typename Iterator::ValueType>(Iterator(--p_end_iterator), Iterator(--p_begin_iterator));
    }

#ifdef FEATURE_ASSERTION
    static_assert(IsForwardIterator<DummyIterator<X>>, "`XIterator` is malformed.");
    static_assert(IsForwardIterator<AccessibleDummyIterator<X>, typename AccessibleDummyIterator<X>::ValueType &>, "`XAccessibleIterator` is malformed.");
    static_assert(IsIteratorAvailable<IteratorWrapper<DummyIterator<X>>>, "`IteratorWrapper` is maleformed.");
    static_assert(IsIteratorAccessible<AccessibleIteratorWrapper<AccessibleDummyIterator<X>>>, "`AccessibleIteratorWrapper` is maleformed.");
    static_assert(IsForwardIterator<EnumerateIterator<DummyIterator<X>>, typename EnumerateIterator<DummyIterator<X>>::ValueType>, "`EnumerateIterator` is malformed.");
    static_assert(IsForwardIterator<AccessibleEnumerateIterator<AccessibleDummyIterator<X>>, typename AccessibleEnumerateIterator<AccessibleDummyIterator<X>>::ValueType>, "`AccessibleEnumerateIterator` is malformed.");
    static_assert(IsForwardIterator<ZipIterator<DummyIterator<X>, DummyIterator<Y>>, typename ZipIterator<DummyIterator<X>, DummyIterator<Y>>::ValueType>, "`EnumerateIterator` is malformed.");
    static_assert(IsForwardIterator<AccessibleZipIterator<AccessibleDummyIterator<X>, AccessibleDummyIterator<Y>>, typename AccessibleZipIterator<AccessibleDummyIterator<X>, AccessibleDummyIterator<Y>>::ValueType>, "`EnumerateIterator` is malformed.");
    static_assert(IsForwardIterator<ReverseIterator<DummyIterator<X>>, typename ReverseIterator<DummyIterator<X>>::ValueType>, "`ReverseIterator` is malformed.");
    static_assert(IsForwardIterator<AccessibleReverseIterator<AccessibleDummyIterator<X>>, typename AccessibleReverseIterator<AccessibleDummyIterator<X>>::ValueType>, "`AccessibleReverseIterator` is malformed.");
#endif // FEATURE_ASSERTION

} // namespace Rong

#endif // RG_CORE_ITERATOR_HPP