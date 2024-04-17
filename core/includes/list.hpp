#ifndef RG_CORE_LIST_HPP
#define RG_CORE_LIST_HPP

#include <string.h>

#include "def.hpp"
#include "exception.hpp"

namespace Rong
{
    template <class T>
    class List;

    template <class T>
    class ListView;

    template <class T>
    concept IsList = IsElementTypeAccessbile<T> && IsConstructableFromSpan<T, typename T::ElementType> && IsDefaultAvailable<T> && IsCountAvailable<T> && IsDataAvailable<T, typename T::ElementType>;

    template <class T>
    concept IsAccessibleList = IsList<T> && IsCountAccessible<T> && IsDataAccessible<T, typename T::ElementType>;

    template <IsList T>
    constexpr auto list_slice(const T &p_list, U p_begin_index, U p_end_index) -> ListView<typename T::ElementType>
    {
        if (p_begin_index >= p_list.get_count())
            throw Exception<LOGICAL>("Given begin index beyond list's element count.");
        if (p_end_index > p_list.get_count())
            throw Exception<LOGICAL>("Given end index beyond list's element count.");
        if (p_begin_index > p_end_index)
            throw Exception<LOGICAL>("Begin index is larger than end index.");
        if (p_begin_index == p_end_index)
            return ListView<typename T::ElementType>();

        auto new_count = p_end_index - p_begin_index;
        auto new_data = p_list.view_data() + p_begin_index;

        return ListView<typename T::ElementType>(new_data, new_count);
    }

    template <IsList T>
    constexpr auto list_view_element(const T &p_list, U p_index) -> const T::ElementType &
    {
        if (p_index >= p_list.get_count())
            throw Exception<LOGICAL>("Given index is beyond list's element count.");
        return p_list.view_data()[p_index];
    }

    template <IsList T>
        requires IsEqualAvailable<typename T::ElementType, typename T::ElementType>
    constexpr auto list_contains(const T &p_list, const typename T::ElementType &p_thing) -> B
    {
        auto data = p_list.view_data();
        for (U i = 0; i < p_list.get_count(); i++)
            if (data[i] == p_thing)
                return true;
        return false;
    }

    template <IsList T, class C>
        requires IsCallable<void, C, const typename T::ElementType &>
    auto list_for_each(const T &p_list, const C &p_callable) -> void
    {
        auto data = p_list.view_data();
        for (U i = 0; i < p_list.get_count(); i++)
            p_callable(data[i]);
    }

    template <class R, IsList T, class C>
        requires IsCallable<R, C, const typename T::ElementType &>
    auto list_map(const T &p_list, const C &p_callable) -> List<R>
    {
        auto result = List<R>();
        result.reserve(p_list.get_count());

        auto data = p_list.view_data();
        for (U i = 0; i < p_list.get_count(); i++)
            result.append(p_callable(data[i]));

        return result;
    }

    template <IsList T, class C>
        requires IsCallable<B, C, const typename T::ElementType &>
    auto list_filter(const T &p_list, const C &p_callable) -> List<typename T::ElementType>
    {
        auto result = List<typename T::ElementType>();
        result.reserve(p_list.get_count());

        auto data = p_list.view_data();
        for (U i = 0; i < p_list.get_count(); i++)
            if (p_callable(data[i]))
                result.append(data[i]);

        return result;
    }

    template <class T>
    class ListView
    {
    public:
        using ElementType = T;

    private:
        const ElementType *data;
        U count;

    public:
        constexpr ListView(const ElementType *p_data, U p_count) noexcept : data(p_data), count(p_count) {}
        constexpr ListView() noexcept : data(nullptr), count(0) {}

        constexpr auto view_data() const noexcept -> const ElementType * { return data; }
        constexpr auto get_count() const noexcept -> U { return count; }

        operator List<T>() const { return List<T>(data, count); }
        constexpr auto operator[](U p_index) const -> const ElementType & { return Rong::list_view_element(*this, p_index); }

        constexpr auto slice(U p_begin_index, U p_end_index) const -> ListView { return Rong::list_slice(*this, p_begin_index, p_end_index); }
        constexpr auto contains(const ElementType &p_thing) const -> B { return Rong::list_contains(*this, p_thing); }

        template <class C>
        auto for_each(const C &p_callable) const -> void { Rong::list_for_each(*this, p_callable); }
        template <class R, class C>
        auto map(const C &p_callable) const -> List<R> { return Rong::list_map<R>(*this, p_callable); }
        template <class C>
        auto filter(const C &p_callable) const -> List<ElementType> { return Rong::list_filter(*this, p_callable); }
    };

    template <class T>
    class List
    {
    public:
        using ElementType = T;
        static constexpr const U INITIAL_CAPACITY = 16;

    private:
        ElementType *data;
        U count;
        U capacity;

    public:
        constexpr List() noexcept : data(nullptr), count(0), capacity(0) {}

        constexpr auto view_data() const noexcept -> const ElementType * { return data; }
        constexpr auto get_count() const noexcept -> U { return count; }
        constexpr auto get_capacity() const noexcept -> U { return capacity; }

        operator ListView<T>() const { return ListView<T>(data, count); }
        constexpr auto operator[](U p_index) const -> const ElementType & { return Rong::list_view_element(*this, p_index); }

        constexpr auto slice(U p_begin_index, U p_end_index) const -> ListView<T> { return Rong::list_slice(*this, p_begin_index, p_end_index); }
        constexpr auto contains(const ElementType &p_thing) const -> B { return Rong::list_contains(*this, p_thing); }

        template <class C>
        auto for_each(const C &p_callable) const -> void { Rong::list_for_each(*this, p_callable); }
        template <class R, class C>
        auto map(const C &p_callable) const -> List<R> { return Rong::list_map<R>(*this, p_callable); }
        template <class C>
        auto filter(const C &p_callable) const -> List<ElementType> { return Rong::list_filter(*this, p_callable); }

        List(const ElementType *p_data, U p_count) : data(nullptr), count(0), capacity(0)
        {
            reserve(p_count);
            for (U i = 0; i < p_count; i++)
                data[i] = p_data[i];
            count = p_count;
        }

        List(const List<T> &p_list) : data(nullptr), count(0), capacity(0)
        {
            reserve(p_list.count);
            for (U i = 0; i < p_list.count; i++)
                data[i] = p_list.data[i];
            count = p_list.count;
        }

        List(List<T> &&p_list) : data(p_list.data), count(p_list.count), capacity(p_list.capacity)
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
            auto new_data = new ElementType[new_capacity];
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

        auto append(const ElementType &p_thing) -> void
        {
            reserve(capacity + 1);
            data[count] = p_thing;
            count++;
        }

        auto prepend(const ElementType &p_thing) -> void
        {
            reserve(capacity + 1);
            for (U i = count; i > 1; i--)
                data[i] = move(data[i - 1]);
            data[0] = p_thing;
            count++;
        }

        auto pop_back() -> ElementType
        {
            if (count == 0)
                throw Exception<LOGICAL>("Popping an empty list.");

            auto popped = move(data[count - 1]);
            count--;
            return popped;
        }

        auto pop_front() -> ElementType
        {
            if (count == 0)
                throw Exception<LOGICAL>("Popping an empty list.");

            auto popped = move(data[0]);
            for (U i = 0; i < count - 1; i++)
                data[i] = move(data[i + 1]);
            count--;
            return popped;
        }

        auto set(U p_index, const ElementType &p_thing) -> void
        {
            if (p_index >= count)
                throw Exception<LOGICAL>("Given index is beyond list's element count.");
            data[p_index] = p_thing;
        }
    };

} // namespace Rong

#endif // RG_CORE_LIST_HPP