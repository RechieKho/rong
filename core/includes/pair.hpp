#ifndef RG_CORE_PAIR_HPP
#define RG_CORE_PAIR_HPP

#include "def.hpp"

namespace Rong
{

    template <class T, class W>
    class Pair
    {
    public:
        using FirstValueType = T;
        using SecondValueType = W;

    private:
    public:
        FirstValueType first;
        SecondValueType second;

        constexpr Pair(T p_first, W p_second) : first(p_first), second(p_second) {}
    };

} // namespace Rong

#endif // RG_CORE_PAIR_HPP