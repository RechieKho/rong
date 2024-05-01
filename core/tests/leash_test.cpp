#include <catch2/catch_test_macros.hpp>
#include <leash.hpp>

using namespace Rong;

TEST_CASE("Leash base feature.")
{
    auto data = Allocator<X>::allocate();
    auto x = Leash(data);
    REQUIRE(*x == data);
}