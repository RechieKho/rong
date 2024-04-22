#include <catch2/catch_test_macros.hpp>
#include <list.hpp>

using namespace Rong;

TEST_CASE("List view base feature.")
{
    constexpr auto data = "Hello";
    constexpr auto count = 5;
    constexpr auto view = ListView<C>(data, count);
    REQUIRE(view.view_data() == data); // Since a viewer never own.
    REQUIRE(view.get_count() == count);
}

TEST_CASE("List base feature.")
{
    constexpr auto data = "Hello";
    constexpr auto count = 5;
    auto list = List<C>(data, count);
    REQUIRE(list.view_data() != data); // Since a container must own.
    REQUIRE(list.get_count() == count);
}

TEST_CASE("List view indexing.")
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

TEST_CASE("List indexing.")
{
    auto view = List<C>("HELLO", 5);
    REQUIRE(view[0] == 'H');
    REQUIRE(view[1] == 'E');
    REQUIRE(view[2] == 'L');
    REQUIRE(view[3] == 'L');
    REQUIRE(view[4] == 'O');
    REQUIRE_THROWS(view[5]);
    REQUIRE_THROWS(view[-1]);
}

TEST_CASE("List view equality")
{
    constexpr auto data = "Hello";
    constexpr auto count = 5;
    constexpr auto view = ListView<C>(data, count);

    constexpr auto data2 = "Bye";
    constexpr auto count2 = 3;
    constexpr auto view2 = ListView<C>(data2, count2);

    constexpr auto view3 = ListView<C>(data, count);

    REQUIRE(view != view2);
    REQUIRE(view == view3);
}

TEST_CASE("List equality")
{
    constexpr auto data = "Hello";
    constexpr auto count = 5;
    auto view = List<C>(data, count);

    constexpr auto data2 = "Bye";
    constexpr auto count2 = 3;
    auto view2 = List<C>(data2, count2);

    auto view3 = List<C>(data, count);

    REQUIRE(view != view2);
    REQUIRE(view == view3);
}