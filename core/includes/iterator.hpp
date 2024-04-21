#ifndef RG_CORE_ITERATOR_HPP
#define RG_CORE_ITERATOR_HPP

#include "def.hpp"
#include "exception.hpp"
#include "pair.hpp"

namespace Rong
{

    class XIterator
    {
        /* Dummy iterator. */
    public:
        using ValueType = X;

    private:
    public:
        inline XIterator() = delete;
        inline auto operator*() const -> const ValueType & { throw Exception<LOGICAL>("`XIterator` is a dummy iterator."); }
        inline auto operator++() -> XIterator & { throw Exception<LOGICAL>("`XIterator` is a dummy iterator."); }
        inline auto operator==(const XIterator &p_right) const -> B { throw Exception<LOGICAL>("`XIterator` is a dummy iterator."); }
    };

    class XAccessibleIterator
    {
        /* Dummy iterator. */
    public:
        using ValueType = X;

    private:
    public:
        inline XAccessibleIterator() = delete;
        inline auto operator*() -> ValueType & { throw Exception<LOGICAL>("`XAccessibleIterator` is a dummy iterator."); }
        inline auto operator++() -> XAccessibleIterator & { throw Exception<LOGICAL>("`XAccessibleIterator` is a dummy iterator."); }
        inline auto operator==(const XAccessibleIterator &p_right) const -> B { throw Exception<LOGICAL>("`XAccessibleIterator` is a dummy iterator."); }
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
        constexpr auto operator++() -> EnumerateIterator &
        {
            ++iterator;
            ++index;
            return *this;
        }
        constexpr auto operator==(const EnumerateIterator &p_right) const -> B { return iterator == p_right.iterator; }
    };

    template <class T, class R = const typename T::ValueType &>
        requires IsForwardIterator<T, R>
    auto enumerate(T &&p_begin_iterator, T &&p_end_iterator)
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
        inline auto operator++() -> AccessibleEnumerateIterator &
        {
            ++iterator;
            ++index;
            return *this;
        }
        inline auto operator==(const AccessibleEnumerateIterator &p_right) const -> B { return iterator == p_right.iterator; }
    };

    template <class T, class R = typename T::ValueType &>
        requires IsForwardIterator<T, R>
    auto accessible_enumerate(T &&p_begin_iterator, T &&p_end_iterator)
    {
        return AccessibleIteratorWrapper<AccessibleEnumerateIterator<T, R>, typename AccessibleEnumerateIterator<T, R>::ValueType>(forward<T>(p_begin_iterator), forward<T>(p_end_iterator));
    }

#ifdef FEATURE_ASSERTION
    static_assert(IsForwardIterator<XIterator>, "`XIterator` is malformed.");
    static_assert(IsForwardIterator<XAccessibleIterator, XAccessibleIterator::ValueType &>, "`XAccessibleIterator` is malformed.");
    static_assert(IsIteratorAvailable<IteratorWrapper<XIterator>>, "`IteratorWrapper` is maleformed.");
    static_assert(IsIteratorAccessible<AccessibleIteratorWrapper<XAccessibleIterator>>, "`AccessibleIteratorWrapper` is maleformed.");
    static_assert(IsForwardIterator<EnumerateIterator<XIterator>, Pair<U, const XIterator::ValueType &>>, "`EnumerateIterator` is malformed.");
    static_assert(IsForwardIterator<AccessibleEnumerateIterator<XAccessibleIterator>, Pair<U, XAccessibleIterator::ValueType &>>, "`AccessibleEnumerateIterator` is malformed.");
#endif // FEATURE_ASSERTION

} // namespace Rong

#endif // RG_CORE_ITERATOR_HPP