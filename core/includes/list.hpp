#ifndef RG_CORE_LIST_HPP
#define RG_CORE_LIST_HPP

#include <string.h>

#include "def.hpp"
#include "exception.hpp"

namespace Rong
{
    template <class T>
    concept IsListFeaturesAvailable =
        IsKeyTypeAvailable<T> &&
        IsValueTypeAvailable<T> &&
        IsElementTypeAvailable<T> &&
        IsDefaultAvailable<T> &&
        IsConstructableFromSpan<T> &&
        IsCountAvailable<T> &&
        IsDataAvailable<T>;

    template <class T>
    class List;

    template <class T>
    class ListView;

    template <IsListFeaturesAvailable T>
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

    template <IsListFeaturesAvailable T>
    constexpr auto list_view_element(const T &p_list, const typename T::KeyType &p_index) -> const T::ValueType &
    {
        if (p_index >= p_list.get_count())
            throw Exception<LOGICAL>("Given index is beyond list's element count.");
        return p_list.view_data()[p_index];
    }

    template <IsListFeaturesAvailable T>
        requires IsEqualAvailable<typename T::ValueType, typename T::ValueType>
    constexpr auto list_contains(const T &p_list, const typename T::ValueType &p_thing) -> B
    {
        auto data = p_list.view_data();
        for (U i = 0; i < p_list.get_count(); i++)
            if (data[i] == p_thing)
                return true;
        return false;
    }

    template <IsListFeaturesAvailable T, class C>
        requires IsFunction<void, C, const typename T::KeyType &, const typename T::ValueType &>
    auto list_for_each(const T &p_list, const C &p_callable) -> void
    {
        auto data = p_list.view_data();
        for (U i = 0; i < p_list.get_count(); i++)
            p_callable(i, data[i]);
    }

    template <class R, IsListFeaturesAvailable T, class C>
        requires IsFunction<R, C, const typename T::KeyType &, const typename T::ValueType &>
    auto list_map(const T &p_list, const C &p_callable) -> List<R>
    {
        auto result = List<R>();
        result.reserve(p_list.get_count());

        auto data = p_list.view_data();
        for (U i = 0; i < p_list.get_count(); i++)
            result.append(p_callable(i, data[i]));

        return result;
    }

    template <IsListFeaturesAvailable T, class C>
        requires IsFunction<B, C, const typename T::KeyType &, const typename T::ValueType &>
    auto list_filter(const T &p_list, const C &p_callable) -> List<typename T::ValueType>
    {
        auto result = List<typename T::ValueType>();
        result.reserve(p_list.get_count());

        auto data = p_list.view_data();
        for (U i = 0; i < p_list.get_count(); i++)
            if (p_callable(i, data[i]))
                result.append(data[i]);

        return result;
    }

    template <IsListFeaturesAvailable T, IsListFeaturesAvailable V>
        requires IsSame<typename T::ValueType, typename V::ValueType>
    auto list_concat(const T &p_left, const V &p_right) -> List<typename T::ValueType>
    {
        auto result = List<typename T::ValueType>();
        result.reserve(p_left.get_count() + p_right.get_count());

        auto data_left = p_left.view_data();
        for (U i = 0; i < p_left.get_count(); i++)
            result.append(data_left[i]);

        auto data_right = p_right.view_data();
        for (U i = 0; i < p_right.get_count(); i++)
            result.append(data_right[i]);

        return result;
    }

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
        constexpr ListView(const ValueType *p_data, U p_count) noexcept : data(p_data), count(p_count) {}
        constexpr ListView() noexcept : data(nullptr), count(0) {}

        constexpr auto view_data() const noexcept -> const ValueType * { return data; }
        constexpr auto get_count() const noexcept -> U { return count; }

        operator List<ValueType>() const { return List<ValueType>(data, count); }
        constexpr auto operator[](const KeyType &p_index) const -> const ValueType & { return Rong::list_view_element(*this, p_index); }

        constexpr auto slice(const KeyType &p_begin_index, const KeyType &p_end_index) const -> ListView { return Rong::list_slice(*this, p_begin_index, p_end_index); }
        constexpr auto contains(const ValueType &p_thing) const -> B { return Rong::list_contains(*this, p_thing); }

        template <class C>
        auto for_each(const C &p_callable) const -> void { Rong::list_for_each(*this, p_callable); }
        template <class R, class C>
        auto map(const C &p_callable) const -> List<R> { return Rong::list_map<R>(*this, p_callable); }
        template <class C>
        auto filter(const C &p_callable) const -> List<ValueType> { return Rong::list_filter(*this, p_callable); }
        template <class V>
        auto concat(const V &p_list) const -> List<ValueType> { return Rong::list_concat(*this, p_list); }
    };

    template <class T>
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
        constexpr List() noexcept : data(nullptr), count(0), capacity(0) {}

        inline auto view_data() const noexcept -> const ValueType * { return data; }
        inline auto get_count() const noexcept -> U { return count; }
        inline auto get_capacity() const noexcept -> U { return capacity; }

        operator ListView<ValueType>() const { return ListView<ValueType>(data, count); }
        inline auto operator[](const KeyType &p_index) const -> const ValueType & { return Rong::list_view_element(*this, p_index); }

        inline auto slice(const KeyType &p_begin_index, const KeyType &p_end_index) const -> ListView<ValueType> { return Rong::list_slice(*this, p_begin_index, p_end_index); }
        inline auto contains(const ValueType &p_thing) const -> B { return Rong::list_contains(*this, p_thing); }

        template <class C>
        auto for_each(const C &p_callable) const -> void { Rong::list_for_each(*this, p_callable); }
        template <class R, class C>
        auto map(const C &p_callable) const -> List<R> { return Rong::list_map<R>(*this, p_callable); }
        template <class C>
        auto filter(const C &p_callable) const -> List<ValueType> { return Rong::list_filter(*this, p_callable); }
        template <class V>
        auto concat(const V &p_list) const -> List<ValueType> { return Rong::list_concat(*this, p_list); }

        List(const ValueType *p_data, U p_count) : data(nullptr), count(0), capacity(0)
        {
            reserve(p_count);
            for (U i = 0; i < p_count; i++)
                data[i] = p_data[i];
            count = p_count;
        }

        List(const List<ValueType> &p_list) : data(nullptr), count(0), capacity(0)
        {
            reserve(p_list.count);
            for (U i = 0; i < p_list.count; i++)
                data[i] = p_list.data[i];
            count = p_list.count;
        }

        List(List<ValueType> &&p_list) : data(p_list.data), count(p_list.count), capacity(p_list.capacity)
        {
            p_list.data = nullptr;
            p_list.count = 0;
            p_list.capacity = 0;
        }

        ~List()
        {
            clean();
        }

        auto reserve(U p_min_capacity) -> void
        {
            if (p_min_capacity < capacity)
                return;

            auto new_capacity = capacity == 0 ? INITIAL_CAPACITY : capacity;
            while (new_capacity < p_min_capacity)
                new_capacity *= 2;
            auto new_data = new ValueType[new_capacity];
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
                delete[] data;
            capacity = 0;
            count = 0;
        }

        auto insert(const KeyType &p_index, const ValueType &p_thing) -> void
        {
            if (p_index > count)
                throw Exception<LOGICAL>("Given index is out of bound.");
            reserve(capacity + 1);
            for (U i = count; i > p_index + 1; i--)
                data[i] = move(data[i - 1]);
            data[p_index] = p_thing;
            count++;
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
            for (U i = p_index; i < count - 1; i++)
                data[i] = move(data[i + 1]);
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

    static_assert(IsListFeaturesAvailable<List<X>>, "`List` features are malformed.");
    static_assert(IsListFeaturesAvailable<ListView<X>>, "`ListView` features are malformed.");

    static_assert(IsForEachAvailable<List<X>, Function<void, List<X>::KeyType, List<X>::ValueType>>, "`List::for_each` is malformed.");
    static_assert(IsForEachAvailable<ListView<X>, Function<void, List<X>::KeyType, List<X>::ValueType>>, "`ListView::for_each` is malformed.");
    static_assert(IsMapAvailable<Y, List<X>, Function<Y, List<X>::KeyType, List<X>::ValueType>, List<Y>>, "`List::map` is malformed.");
    static_assert(IsMapAvailable<Y, ListView<X>, Function<Y, ListView<X>::KeyType, ListView<X>::ValueType>, List<Y>>, "`ListView::map` is malformed.");
    static_assert(IsFilterAvailable<List<X>, Function<B, List<X>::KeyType, List<X>::ValueType>>, "`List::filter` is malformed.");
    static_assert(IsFilterAvailable<ListView<X>, Function<B, ListView<X>::KeyType, ListView<X>::ValueType>, List<X>>, "`ListView::filter` is malformed.");

} // namespace Rong

#endif // RG_CORE_LIST_HPP