#ifndef RG_CORE_LIST_HPP
#define RG_CORE_LIST_HPP

#include <string.h>

#include "def.hpp"
#include "exception.hpp"
#include "iterator.hpp"
#include "allocator.hpp"

namespace Rong
{
    template <class T>
    concept IsListBaseFeaturesAvailable =
        IsKeyTypeAvailable<T> &&
        IsValueTypeAvailable<T> &&
        IsElementTypeAvailable<T> &&
        IsDefaultAvailable<T> &&
        IsConstructableFromSpan<T> &&
        IsCountAvailable<T> &&
        IsDataAvailable<T> &&
        IsIteratorAvailable<T>;

    template <class T, class A = Allocator<T>>
    class List;

    template <class T>
    class ListView;

    template <IsListBaseFeaturesAvailable T>
    constexpr auto list_slice(const T &p_list, const typename T::KeyType &p_begin_index, const typename T::KeyType &p_end_index) -> ListView<typename T::ValueType>
    {
        if (p_begin_index >= p_list.get_count())
            throw Exception<LOGICAL>("Given begin index beyond list's element count.");
        if (p_end_index > p_list.get_count())
            throw Exception<LOGICAL>("Given end index beyond list's element count.");
        if (p_begin_index > p_end_index)
            throw Exception<LOGICAL>("Begin index is larger than end index.");
        if (p_begin_index == p_end_index)
            return ListView<typename T::ValueType>();

        auto new_count = p_end_index - p_begin_index;
        auto new_data = p_list.view_data() + p_begin_index;

        return ListView<typename T::ValueType>(new_data, new_count);
    }

    template <IsListBaseFeaturesAvailable T, IsListBaseFeaturesAvailable W>
        requires IsSame<typename T::ValueType, typename W::ValueType>
    constexpr auto list_equal(const T &p_left, const W &p_right) -> B
    {
        if (p_left.get_count() != p_right.get_count())
            return false;

        auto iterable = zip(p_left, p_right);
        for (auto it = iterable.cbegin(); it != iterable.cend(); ++it)
        {
            auto [first, second] = *it;
            if (first != second)
                return false;
        }

        return true;
    }

    template <IsListBaseFeaturesAvailable T>
        requires IsEqualAvailable<typename T::ValueType, typename T::ValueType>
    constexpr auto list_contains(const T &p_list, const typename T::ValueType &p_thing) -> B
    {
        for (auto it = p_list.cbegin(); it != p_list.cend(); ++it)
            if (*it == p_thing)
                return true;
        return false;
    }

    template <IsListBaseFeaturesAvailable T, class C>
        requires IsFunction<void, C, const typename T::KeyType &, const typename T::ValueType &>
    constexpr auto list_for_each(const T &p_list, C &&p_callable) -> void
    {
        for_each(forward<C>(p_callable), p_list.cbegin(), p_list.cend());
    }

    template <class R, IsListBaseFeaturesAvailable T, class C>
        requires IsFunction<R, C, const typename T::KeyType &, const typename T::ValueType &>
    auto list_map(const T &p_list, C &&p_callable) -> List<R>
    {
        auto result = List<R>();
        result.reserve(p_list.get_capacity());

        for_each([&](U p_index, auto p_element)
                 { result.append(p_callable(p_index, p_element)); }, p_list.cbegin(), p_list.cend());

        return result;
    }

    template <IsListBaseFeaturesAvailable T, class C>
        requires IsFunction<B, C, const typename T::KeyType &, const typename T::ValueType &>
    auto list_filter(const T &p_list, C &&p_callable) -> List<typename T::ValueType>
    {
        auto result = List<typename T::ValueType>();
        result.reserve(p_list.get_count());

        for_each([&](U p_index, auto p_element)
                 { if constexpr (p_callable(p_index, p_element)) result.append(p_element); }, p_list.cbegin(), p_list.cend());

        return result;
    }

    template <IsListBaseFeaturesAvailable T, IsListBaseFeaturesAvailable W>
        requires IsSame<typename T::ValueType, typename W::ValueType>
    auto list_concat(const T &p_left, const W &p_right) -> List<typename T::ValueType>
    {
        auto result = List<typename T::ValueType>();
        result.reserve(p_left.get_count() + p_right.get_count());

        for_each([&](U p_index, auto p_element)
                 { result.append(p_element); }, p_left.cbegin(), p_left.cend());
        for_each([&](U p_index, auto p_element)
                 { result.append(p_element); }, p_right.cbegin(), p_right.cend());

        return result;
    }

    template <class T>
        requires IsSame<T, List<typename T::ValueType>> ||
                 IsSame<T, ListView<typename T::ValueType>>
    class ListIterator
    {
    public:
        using ContainerType = T;
        using KeyType = ContainerType::KeyType;
        using ValueType = ContainerType::ValueType;

    private:
        const ContainerType &container;
        KeyType current_key;

    public:
        constexpr ListIterator(const ContainerType &p_container, KeyType p_starting_key = 0) : container(p_container), current_key(move(p_starting_key)) {}
        constexpr auto operator*() const -> const ValueType & { return container[current_key]; }
        constexpr auto operator++() -> ListIterator
        {
            ++current_key;
            return *this;
        }
        constexpr auto operator==(const ListIterator &p_right) const -> B { return &container == &p_right.container && current_key == p_right.current_key; }
        constexpr auto operator--() -> ListIterator
        {
            --current_key;
            return *this;
        }
    };

    template <class T>
    class ListView
    {
    public:
        using KeyType = U;
        using ValueType = T;
        using ElementType = ValueType;

    private:
        const ElementType *data;
        U count;

    public:
        constexpr ListView(const ValueType *p_data, U p_count) : data(p_data), count(p_count) {}
        constexpr ListView() : data(nullptr), count(0) {}

        constexpr auto view_data() const -> const ValueType * { return data; }
        constexpr auto get_count() const -> U { return count; }

        operator List<ValueType>() const { return List<ValueType>(data, count); }
        constexpr auto operator==(const ListView &p_right) const -> B { return Rong::list_equal(*this, p_right); }
        constexpr auto operator==(const List<ValueType> &p_right) const -> B { return Rong::list_equal(*this, p_right); }

        constexpr auto slice(const KeyType &p_begin_index, const KeyType &p_end_index) const -> ListView { return Rong::list_slice(*this, p_begin_index, p_end_index); }
        constexpr auto contains(const ValueType &p_thing) const -> B { return Rong::list_contains(*this, p_thing); }
        constexpr auto cbegin() const -> ListIterator<ListView> { return ListIterator<ListView>(*this); }
        constexpr auto cend() const -> ListIterator<ListView> { return ListIterator<ListView>(*this, count); }

        template <class C>
        constexpr auto for_each(const C &p_callable) const -> void { Rong::list_for_each(*this, p_callable); }
        template <class R, class C>
        inline auto map(const C &p_callable) const -> List<R> { return Rong::list_map<R>(*this, p_callable); }
        template <class C>
        inline auto filter(const C &p_callable) const -> List<ValueType> { return Rong::list_filter(*this, p_callable); }
        template <class V>
        inline auto concat(const V &p_list) const -> List<ValueType> { return Rong::list_concat(*this, p_list); }

        constexpr auto operator[](const KeyType &p_index) const -> const ValueType &
        {
            if (p_index >= count)
                throw Exception<LOGICAL>("Given index is beyond list's element count.");
            return data[p_index];
        }
    };

    template <class T, class A>
    class List
    {
    public:
        using KeyType = U;
        using ValueType = T;
        using ElementType = ValueType;
        static constexpr const U INITIAL_CAPACITY = 16;

    private:
        ElementType *data;
        U count;
        U capacity;

    public:
        class AccessibleIterator
        {
        public:
            using ContainerType = List;
            using KeyType = ContainerType::KeyType;
            using ValueType = ContainerType::ValueType;

        private:
            ContainerType &container;
            KeyType current_key;

        public:
            inline AccessibleIterator(ContainerType &p_container, KeyType p_starting_key = 0) : container(p_container), current_key(move(p_starting_key)) {}
            inline auto operator*() -> ValueType & { return container[current_key]; }
            inline auto operator++() -> AccessibleIterator
            {
                ++current_key;
                return *this;
            }
            inline auto operator==(const AccessibleIterator &p_right) const -> B { return &container == &p_right.container && current_key == p_right.current_key; }
            inline auto operator--() -> AccessibleIterator
            {
                --current_key;
                return *this;
            }
            inline auto operator+(KeyType p_increment) const -> AccessibleIterator { return AccessibleIterator(container, current_key + p_increment); }
        };

    public:
        constexpr List() : data(nullptr), count(0), capacity(0) {}

        inline auto view_data() const -> const ValueType * { return data; }
        inline auto get_count() const -> U { return count; }
        inline auto get_capacity() const -> U { return capacity; }

        operator ListView<ValueType>() const { return ListView<ValueType>(data, count); }
        constexpr auto operator==(const List &p_right) const -> B { return Rong::list_equal(*this, p_right); }
        constexpr auto operator==(const ListView<ValueType> &p_right) const -> B { return Rong::list_equal(*this, p_right); }

        inline auto slice(const KeyType &p_begin_index, const KeyType &p_end_index) const -> ListView<ValueType> { return Rong::list_slice(*this, p_begin_index, p_end_index); }
        inline auto contains(const ValueType &p_thing) const -> B { return Rong::list_contains(*this, p_thing); }
        inline auto cbegin() const -> ListIterator<List> { return ListIterator<List>(*this); }
        inline auto cend() const -> ListIterator<List> { return ListIterator<List>(*this, count); }
        inline auto begin() -> AccessibleIterator { return AccessibleIterator(*this); }
        inline auto end() -> AccessibleIterator { return AccessibleIterator(*this, count); }

        template <class C>
        inline auto for_each(const C &p_callable) const -> void { Rong::list_for_each(*this, p_callable); }
        template <class R, class C>
        inline auto map(const C &p_callable) const -> List<R> { return Rong::list_map<R>(*this, p_callable); }
        template <class C>
        inline auto filter(const C &p_callable) const -> List<ValueType> { return Rong::list_filter(*this, p_callable); }
        template <class V>
        inline auto concat(const V &p_list) const -> List<ValueType> { return Rong::list_concat(*this, p_list); }

        List(U p_capacity) : data(nullptr), count(0), capacity(0)
        {
            reserve(p_capacity);
        }

        List(const ValueType *p_data, U p_count) : data(nullptr), count(0), capacity(0)
        {
            reserve(p_count);
            for (U i = 0; i < p_count; i++)
                data[i] = p_data[i];
            count = p_count;
        }

        List(const List &p_list) : data(nullptr), count(0), capacity(0)
        {
            reserve(p_list.count);
            for (U i = 0; i < p_list.count; i++)
                data[i] = p_list.data[i];
            count = p_list.count;
        }

        List(List &&p_list) : data(0), count(0), capacity(0)
        {
            clean();

            data = p_list.data;
            count = p_list.count;
            capacity = p_list.capacity;

            p_list.data = nullptr;
            p_list.count = 0;
            p_list.capacity = 0;
        }

        ~List()
        {
            clean();
        }

        inline auto operator[](const KeyType &p_index) const -> ValueType &
        {
            if (p_index >= count)
                throw Exception<LOGICAL>("Given index is beyond list's element count.");
            return data[p_index];
        }

        auto reserve(U p_min_capacity) -> void
        {
            if (p_min_capacity < capacity)
                return;

            auto new_capacity = capacity == 0 ? INITIAL_CAPACITY : capacity;
            while (new_capacity < p_min_capacity)
                new_capacity *= 2;
            auto new_data = A::allocate(new_capacity);
            auto new_count = count;

            for (U i = 0; i < count; i++)
                new_data[i] = move(data[i]);

            clean();

            data = new_data;
            capacity = new_capacity;
            count = new_count;
        }

        auto clean() -> void
        {
            if (data != nullptr && capacity != 0)
            {
                for (auto element : *this)
                    element.~ValueType();
                A::deallocate(data);
            }
            data = nullptr;
            capacity = 0;
            count = 0;
        }

        auto insert(const KeyType &p_index, const ValueType &p_thing) -> void
        {
            if (p_index > count)
                throw Exception<LOGICAL>("Given index is out of bound.");
            reserve(capacity + 1);
            count++;

            auto target_iterable = accessible_reverse(begin() + p_index + 1, end());
            auto source_iterable = accessible_reverse(begin() + p_index, --end());
            for (auto [target, source] : accessible_zip(target_iterable, source_iterable))
                target = move(source);

            data[p_index] = p_thing;
        }

        auto append(const ValueType &p_thing) -> void
        {
            insert(count, p_thing);
        }

        auto prepend(const ValueType &p_thing) -> void
        {
            insert(0, p_thing);
        }

        auto remove(const KeyType &p_index) -> ValueType
        {
            if (p_index >= count)
                throw Exception<LOGICAL>("Given index is out of bound.");

            if (count == 0)
                throw Exception<LOGICAL>("Impossible to remove item from empty list.");

            auto popped = move(data[p_index]);

            auto iterable = accessible_zip(
                begin() + p_index, --end(),
                begin() + p_index + 1, end());
            for (auto [target, source] : iterable)
                target = move(source);

            count--;
            return popped;
        }

        auto pop_back() -> ValueType
        {
            return remove(count - 1);
        }

        auto pop_front() -> ValueType
        {
            return remove(0);
        }

        auto set(const KeyType &p_index, const ValueType &p_thing) -> void
        {
            if (p_index >= count)
                throw Exception<LOGICAL>("Given index is beyond list's element count.");
            data[p_index] = p_thing;
        }
    };

#ifdef FEATURE_ASSERTION
    static_assert(IsBidirectionalIterator<ListIterator<List<X>>>, "`ListIterator` is malformed.");
    static_assert(IsBidirectionalIterator<List<X>::AccessibleIterator, List<X>::AccessibleIterator::ValueType &>, "`List::AccessibleIterator` is malformed.");

    static_assert(IsListBaseFeaturesAvailable<ListView<X>>, "`ListView` features are malformed.");
    static_assert(IsEqualAvailable<ListView<X>, List<X>>, "`ListView::operator==` is malformed.");
    static_assert(IsEqualAvailable<ListView<X>, ListView<X>>, "`ListView::operator==` is malformed.");
    static_assert(IsSliceAvailable<ListView<X>, ListView<X>>, "`ListView::slice` is malformed.");
    static_assert(IsIndexAvailable<ListView<X>>, "`ListView::operator[]` is malformed.");
    static_assert(IsContainsAvailable<ListView<X>>, "`ListView::contains` is malformed.");
    static_assert(IsForEachAvailable<ListView<X>, Function<void, List<X>::KeyType, List<X>::ValueType>>, "`ListView::for_each` is malformed.");
    static_assert(IsMapAvailable<Y, ListView<X>, Function<Y, ListView<X>::KeyType, ListView<X>::ValueType>, List<Y>>, "`ListView::map` is malformed.");
    static_assert(IsFilterAvailable<ListView<X>, Function<B, ListView<X>::KeyType, ListView<X>::ValueType>, List<X>>, "`ListView::filter` is malformed.");
    static_assert(IsConcatAvailable<ListView<X>, List<X>, List<X>>, "`ListView::concat` is malformed.");
    static_assert(IsConcatAvailable<ListView<X>, ListView<X>, List<X>>, "`ListView::concat` is malformed.");
    static_assert(IsIteratorAvailable<ListView<X>>, "`ListView` iterator is malformed.");

    static_assert(IsListBaseFeaturesAvailable<List<X>>, "`List` features are malformed.");
    static_assert(IsEqualAvailable<List<X>, List<X>>, "`List::operator==` is malformed.");
    static_assert(IsEqualAvailable<List<X>, ListView<X>>, "`List::operator==` is malformed.");
    static_assert(IsSliceAvailable<List<X>, ListView<X>>, "`List::slice` is malformed.");
    static_assert(IsIndexAvailable<List<X>, typename List<X>::ValueType &>, "`List::operator[]` is malformed.");
    static_assert(IsContainsAvailable<List<X>>, "`List::contains` is malformed.");
    static_assert(IsForEachAvailable<List<X>, Function<void, List<X>::KeyType, List<X>::ValueType>>, "`List::for_each` is malformed.");
    static_assert(IsMapAvailable<Y, List<X>, Function<Y, List<X>::KeyType, List<X>::ValueType>, List<Y>>, "`List::map` is malformed.");
    static_assert(IsFilterAvailable<List<X>, Function<B, List<X>::KeyType, List<X>::ValueType>>, "`List::filter` is malformed.");
    static_assert(IsConcatAvailable<List<X>>, "`List::concat` is malformed.");
    static_assert(IsConcatAvailable<List<X>, ListView<X>>, "`List::concat` is malformed.");
    static_assert(IsCapacityAvailable<List<X>>, "`List::get_capacity` is malformed.");
    static_assert(IsReserveAvailable<List<X>>, "`List::reserve` is malformed.");
    static_assert(IsCleanAvailable<List<X>>, "`List::clean` is malformed.");
    static_assert(IsInsertAvailable<List<X>>, "`List::insert` is malformed.");
    static_assert(IsAppendAvailable<List<X>>, "`List::append` is malformed.");
    static_assert(IsPrependAvailable<List<X>>, "`List::prepend` is malformed.");
    static_assert(IsRemoveAvailable<List<X>>, "`List::remove` is malformed.");
    static_assert(IsPopBackAvailable<List<X>>, "`List::pop_back` is malformed.");
    static_assert(IsPopFrontAvailable<List<X>>, "`List::pop_front` is malformed.");
    static_assert(IsSetAvailable<List<X>>, "`List::set` is malformed.");
    static_assert(IsIteratorAvailable<List<X>>, "`List` iterator is malformed.");
    static_assert(IsIteratorAccessible<List<X>>, "`List` accessible iterator is malformed.");
#endif // FEATURE_ASSERTION

} // namespace Rong

#endif // RG_CORE_LIST_HPP