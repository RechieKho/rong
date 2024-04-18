#include <list.hpp>
#include <stdio.h>

using namespace Rong;

int main()
{
    constexpr auto hello = ListView<C>("Hello ", 6);
    constexpr auto world = ListView<C>("world", 5);

    hello.concat(world).for_each([](U i, const C &c)
                                 { fputc(c, stdout); });
    fputc('\n', stdout);
}
