#ifndef RG_CORE_ALLOCATOR_HPP
#define RG_CORE_ALLOCATOR_HPP

#include "def.hpp"
#include "exception.hpp"

namespace Rong
{

    /// Allocator only handles memory allocation, but not initialization and deinitialization.
    template <class T, class R>
    concept IsAllocatorFeaturesAvailable = requires(R *p_pointer, Size p_count) {
        {
            T::allocate(p_count)
        } -> IsSame<R *>;

        {
            T::deallocate(p_pointer)
        } -> IsSame<void>;
    };

    template <class T>
    struct Allocator : Inconstructible
    {
        using Type = T;
        static auto allocate(Size p_count = 1) -> Type *
        {
            auto allocated = (Type *)calloc(p_count, sizeof(Type));
            if (allocated == nullptr)
                throw Exception<RUNTIME>("Fail to allocate memory.");
            return allocated;
        }
        static auto deallocate(Type *p_pointer) -> void
        {
            if (p_pointer == nullptr)
                throw Exception<LOGICAL>("Deallocating a nullptr is forbidden.");
            free(p_pointer);
        }
    };
    static_assert(IsAllocatorFeaturesAvailable<Allocator<X>, X>, "`Allocator` is malformed.");

} // namespace Rong

#endif // RG_CORE_ALLOCATOR_HPP