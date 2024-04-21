#include <catch2/catch_test_macros.hpp>
#include <list.hpp>

using namespace Rong;

TEST_CASE("List view indexing.", "[ListView::indexing]")
{
    constexpr auto view = ListView<C>("HELLO", 5);
    REQUIRE(view[0] == 'H');
    REQUIRE(view[1] == 'E');
    REQUIRE(view[2] == 'L');
    REQUIRE(view[3] == 'L');
    REQUIRE(view[4] == 'O');
    REQUIRE_THROWS(view[5]);
    REQUIRE_THROWS(view[-1]);
}

TEST_CASE("List indexing.", "[List::indexing]")
{
    auto view = List<C>("HELLO", 5);
    auto second = List<C>("HELsS", 5);
    REQUIRE(view != second);
    REQUIRE(view[0] == 'H');
    REQUIRE(view[1] == 'E');
    REQUIRE(view[2] == 'L');
    REQUIRE(view[3] == 'L');
    REQUIRE(view[4] == 'O');
    REQUIRE_THROWS(view[5]);
    REQUIRE_THROWS(view[-1]);
}