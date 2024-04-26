#ifndef RG_CORE_LEASH_HPP
#define RG_CORE_LEASH_HPP

#include "def.hpp"
#include "allocator.hpp"

namespace Rong
{
    template <class T, class A = Allocator<T>>
        requires IsAllocatorFeaturesAvailable<A, T>
    class Leash;

    template <class T, class A>
        requires IsAllocatorFeaturesAvailable<A, T>
    class Leash
    {
    public:
        using ValueType = T;

    private:
        ValueType *value_pointer;

    public:
        template <class... W>
        static auto spawn(W... p_arguments) -> Leash
        {
            auto leash = Leash();
            leash.instantiate(p_arguments...);
            return leash;
        }

        constexpr Leash() : value_pointer(nullptr) {}
        Leash(const Leash &p_leash) = delete;
        Leash(Leash &&p_leash) : value_pointer(p_leash.value_pointer) { p_leash.value_pointer = nullptr; }

        inline operator B() { return value_pointer != nullptr; }
        inline auto operator->() -> ValueType * { return value_pointer; }
        inline auto operator->() const -> const ValueType * { return value_pointer; }
        inline auto operator*() -> ValueType * { return value_pointer; }
        inline auto operator*() const -> const ValueType * { return value_pointer; }
        inline auto operator==(const Leash &p_leash) const -> B { return value_pointer == p_leash.value_pointer; }

        template <class... W>
        inline auto instantiate(W... p_arguments) -> void
        {
            value_pointer = A::allocate();
            *value_pointer = ValueType(p_arguments...);
        }

        inline auto clean() -> void
        {
            if (value_pointer != nullptr)
            {
                value_pointer->~ValueType();
                A::deallocate(value_pointer);
            }
            value_pointer = nullptr;
        }
    };
} // namespace Rong

#endif // RG_CORE_LEASH_HPP