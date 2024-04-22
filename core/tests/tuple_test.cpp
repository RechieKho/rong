#include <catch2/catch_test_macros.hpp>
#include <tuple.hpp>

using namespace Rong;

TEST_CASE("Tuple base feature")
{
    constexpr auto tuple = Tuple(1, 2, 3);
    REQUIRE(tuple.get<0>() == 1);
    REQUIRE(tuple.get<1>() == 2);
    REQUIRE(tuple.get<2>() == 3);
}
