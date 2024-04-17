#include <list.hpp>
#include <stdio.h>

using namespace Rong;

int main()
{
    constexpr auto view = ListView<C>("Hello world", 11);
    constexpr auto sliced = view.slice(2, 6);
    sliced.for_each([](const C &c)
                    { fputc(c, stdout); });
    fputc('\n', stdout);

    auto list = List<C>(sliced);
    list.prepend('H');
    list.for_each([](const C &c)
                  { fputc(c, stdout); });
    fputc('\n', stdout);

    auto mapped = list.map<C>([](const C &c)
                              { return 'E'; });
    mapped.for_each([](const C &c)
                    { fputc(c, stdout); });
    fputc('\n', stdout);

    auto filtered = view.filter([](const C &c)
                                { return c != 'l'; });
    filtered.for_each([](const C &c)
                      { fputc(c, stdout); });
    fputc('\n', stdout);
}
