#ifndef RG_CORE_TUPLE_HPP
#define RG_CORE_TUPLE_HPP

#include "def.hpp"

namespace Rong
{

    template <class T, class... V>
    class TupleNode : public TupleNode<V...>
    {
    public:
        using ValueType = T;

    private:
    protected:
        ValueType value;

    public:
        constexpr TupleNode(ValueType p_value, V... p_values) : TupleNode<V...>(p_values...), value(p_value) {}
    };

    template <class T>
    class TupleNode<T>
    {
    public:
        using ValueType = T;

    private:
    protected:
        ValueType value;

    public:
        constexpr TupleNode(ValueType p_value) : value(p_value) {}
    };

    template <class... T>
    class Tuple : public TupleNode<T...>
    {
    public:
        template <U Index>
        using ValueType = TypeAt<Index, T...>::Type;

    private:
    public:
        constexpr Tuple(T... p_values) : TupleNode<T...>(p_values...) {}

        template <U Index>
            requires(Index == 0)
        constexpr auto get() const -> ValueType<Index>
        {
            return TupleNode<T...>::value;
        }

        template <U Index>
            requires(Index > 0)
        constexpr auto get() const -> ValueType<Index>
        {
            return TypeAt<Index - 1, T...>::template Rest<TupleNode>::value;
        }

        template <class V>
        constexpr auto get() const -> ValueType<TypeIndex<V, T...>::INDEX>
        {
            return get<TypeIndex<V, T...>::INDEX>();
        }
    };

} // namespace Rong

#endif // RG_CORE_TUPLE_HPP