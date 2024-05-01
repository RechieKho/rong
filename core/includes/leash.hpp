#ifndef RG_CORE_LEASH_HPP
#define RG_CORE_LEASH_HPP

#include "def.hpp"
#include "allocator.hpp"

namespace Rong
{
    /// Function to deinitialize and deallocate.
    template <class T>
    using LeashDestroyFunction = Function<void, T *>;

    template <class T, class A = Allocator<T>>
    auto leash_default_destroy(T *p_pointer) -> void
    {
        p_pointer->~T();
        A::deallocate(p_pointer);
    }

    template <class T, LeashDestroyFunction<T> A = leash_default_destroy<T>>
    class Leash;

    template <class T, LeashDestroyFunction<T> A>
    class Leash
    {
    public:
        using ValueType = T;

    private:
        ValueType *value_pointer;

    public:
        Leash(ValueType *p_value_pointer) : value_pointer(p_value_pointer) {}
        Leash(const Leash &p_leash) = delete;
        Leash(Leash &&p_leash) : value_pointer(p_leash.value_pointer) { p_leash.value_pointer = nullptr; }

        ~Leash()
        {
            if (value_pointer != nullptr)
                A(value_pointer);
            value_pointer = nullptr;
        }

        inline operator B() { return value_pointer != nullptr; }
        inline auto operator->() -> ValueType * { return value_pointer; }
        inline auto operator->() const -> const ValueType * { return value_pointer; }
        inline auto operator*() -> ValueType * { return value_pointer; }
        inline auto operator*() const -> const ValueType * { return value_pointer; }
        inline auto operator==(const Leash &p_leash) const -> B { return value_pointer == p_leash.value_pointer; }
    };
} // namespace Rong

#endif // RG_CORE_LEASH_HPP