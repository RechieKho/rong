#include <catch2/catch_test_macros.hpp>
#include <iterator.hpp>
#include <list.hpp>

using namespace Rong;

TEST_CASE("Reverse Iterator")
{
    constexpr auto data = "12345";
    constexpr auto count = 5;
    constexpr auto view = ListView(data, count);
    auto iterable = reverse(view.cbegin(), view.cend());
    auto iterator = iterable.cbegin();
    REQUIRE(*iterator == '5');
    ++iterator;
    REQUIRE(*iterator == '4');
    ++iterator;
    REQUIRE(*iterator == '3');
    ++iterator;
    REQUIRE(*iterator == '2');
    ++iterator;
    REQUIRE(*iterator == '1');
    ++iterator;
}