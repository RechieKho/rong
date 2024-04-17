#ifndef RG_CORE_EXCEPTION_HPP
#define RG_CORE_EXCEPTION_HPP

#include "def.hpp"

namespace Rong
{

    enum ExceptionKind
    {
        LOGICAL,
        RUNTIME
    };

    /// Base class for all exceptions.
    template <ExceptionKind>
    class Exception
    {
    public:
    private:
        const C *description;

    public:
        constexpr Exception(const C *p_description) noexcept : description(p_description) {}

        constexpr auto what() const noexcept -> const C * { return description; }
    };

} // namespace Rong

#endif // RG_CORE_EXCPETION_HPP