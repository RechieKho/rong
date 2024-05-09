#ifndef RG_CORE_BINARY_TREE
#define RG_CORE_BINARY_TREE

#include "def.hpp"
#include "leash.hpp"

namespace Rong
{
    template <IsConstrastAvailable K, class V, template <class E> class A = Allocator>
        requires IsAllocatorFeaturesAvailable<A<X>, X>
    class BinaryTree
    {
    public:
        using KeyType = K;
        using ValueType = V;
        using ElementType = BinaryTree;
        using Allocator = A<ElementType>;

    private:
        KeyType key;
        ValueType value;
        Leash<ElementType> left;
        Leash<ElementType> right;

    public:
        BinaryTree() : value(), left(nullptr), right(nullptr) {}

        BinaryTree(const KeyType &p_key, const ValueType &p_value, Leash<ElementType> p_left, Leash<ElementType> p_right) : key(p_key), value(p_value), left(move(p_left)), right(move(p_right)) {}
        BinaryTree(const BinaryTree &p_tree) : key(p_tree.key), value(p_tree.value), left(nullptr), right(nullptr)
        {
            const auto left_copied = **left;
            const auto right_copied = **right;
            left = Leash(left_copied);
            right = Leash(right_copied);
        }
        BinaryTree(BinaryTree &&p_tree) = default;

        auto operator[](const KeyType &p_key) -> ValueType &
        {
            const auto key_contrast = contrast(p_key, key);
            if (key_contrast == 0)
                return value;
            else if (key_contrast < 0)
            {
                if (!left)
                    throw Exception<LOGICAL>("Given key is not in the tree.");
                return (**left)[p_key];
            }
            else
            {
                if (!right)
                    throw Exception<LOGICAL>("Given key is not in the tree.");
                return (**right)[p_key];
            }
        }

        inline auto operator[](const KeyType &p_key) const -> const ValueType &
        {
            const auto key_contrast = contrast(p_key, key);
            if (key_contrast == 0)
                return value;
            else if (key_contrast < 0)
            {
                if (!left)
                    throw Exception<LOGICAL>("Given key is not in the tree.");
                return (**left)[p_key];
            }
            else
            {
                if (!right)
                    throw Exception<LOGICAL>("Given key is not in the tree.");
                return (**right)[p_key];
            }
        }

        auto compute_height(Size p_initial_height = 0) -> Size const
        {
            const auto height = p_initial_height + 1;
            const auto left_count = left ? left->compute_height(height) : height;
            const auto right_count = right ? right->compute_height(height) : height;
            return left_count > right_count ? left_count : right_count;
        }

        auto compute_left_count(Size p_initial_count = 0) -> Size const
        {
            const auto count = p_initial_count + 1;
            if (left)
                return left->compute_left_count(count);
            return count;
        }

        auto compute_right_count(Size p_initial_count = 0) -> Size const
        {
            const auto count = p_initial_count + 1;
            if (right)
                return right->compute_right_count(count);
            return count;
        }

        auto set(Leash<ElementType> p_tree) -> void
        {
            if (!p_tree)
                throw Exception<LOGICAL>("An empty tree is given.");

            const auto key_contrast = contrast(p_tree->key, key);
            if (key_contrast == 0)
            {
                /// TODO: Implement this. Merge the `p_tree` children with current children.
                throw Exception<LOGICAL>("Unable to update tree.");
            }
            else if (key_contrast > 0)
            {
                if (right)
                    right->set(move(p_tree));
                else
                    right = move(p_tree);
            }
            else
            {
                if (left)
                    left->set(move(p_tree));
                else
                    left = move(p_tree);
            }
        }

        inline auto set(const KeyType &p_key, const ValueType &p_value) -> void
        {
            auto tree = Leash(Allocator::allocate());
            tree->key = p_key;
            tree->value = p_value;
            tree->left = Leash<ElementType>(nullptr);
            tree->right = Leash<ElementType>(nullptr);
            set(move(tree));
        }

        auto rotate_left(Size p_count = 1) -> B
        {
            for (Size i = 0; i < p_count; i++)
            {
                if (!right)
                    return false;

                auto new_key = move(right->key);
                auto new_value = move(right->value);
                auto new_right = move(right->right);
                auto popped = move(right->left);

                auto new_left = move(right);

                new_left->key = move(key);
                new_left->value = move(value);
                new_left->left = move(left);

                key = move(new_key);
                value = move(new_value);
                left = move(new_left);
                right = move(new_right);

                if (popped)
                    set(move(popped));
            }
            return true;
        }

        auto rotate_right(Size p_count = 1) -> B
        {
            for (Size i = 0; i < p_count; i++)
            {
                if (!left)
                    return false;

                auto new_key = move(left->key);
                auto new_value = move(left->value);
                auto new_left = move(left->left);
                auto popped = move(left->right);

                auto new_right = move(left);

                new_right->key = move(key);
                new_right->value = move(value);
                new_right->right = move(right);

                key = move(new_key);
                value = move(new_value);
                left = move(new_left);
                right = move(new_right);

                if (popped)
                    set(move(popped));
            }

            return true;
        }

        auto balance() -> void
        {
            if (left)
                left->balance();
            if (right)
                right->balance();

            const auto left_count = compute_left_count();
            const auto right_count = compute_right_count();

            if (left_count == right_count)
                return;
            else if (left_count > right_count)
                rotate_right((left_count - right_count) / 2);
            else
                rotate_left((right_count - left_count) / 2);
        }
    };

} // namespace Rong

#endif // RG_CORE_BINARY_TREE