#include <catch2/catch_test_macros.hpp>
#include <leash.hpp>

using namespace Rong;

TEST_CASE("Leash base feature.")
{
    auto x = Leash<X>::spawn();
    REQUIRE(x);
    x.clean();
    REQUIRE(!x);
    x.instantiate();
    REQUIRE(x);
}