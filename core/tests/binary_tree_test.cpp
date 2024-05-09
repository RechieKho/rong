#include <catch2/catch_test_macros.hpp>
#include <binary_tree.hpp>

using namespace Rong;

TEST_CASE("Binary Tree Base Feature")
{
    auto tree = BinaryTree<U, C>();
    tree.set(0, 'H');
    tree.set(2, 'E');
    tree.set(3, 'P');
    tree.set(8, '6');
    tree.set(9, 'm');
    tree.set(10, 'k');
    tree.set(11, '9');
    tree.set(12, '6');
    tree.set(13, 'a');
    tree.set(14, 's');

    tree.balance();

    REQUIRE(tree[14] == 's');
    REQUIRE_THROWS(tree[1]);
}
