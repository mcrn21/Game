#ifndef AE_BVH_H
#define AE_BVH_H

#include "../geometry/frustum.h"
#include "../geometry/primitives.h"

#include <functional>
#include <memory>
#include <vector>

namespace ae {

template<typename T, T invalid_value>
class BVH
{
public:
    struct Node
    {
        AABB aabb;
        T value = invalid_value;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;

        bool isLeaf() const { return value != invalid_value; }
    };

    BVH() = default;
    ~BVH() = default;

    void insert(const T &value, const AABB &aabb) { insertRecursive(m_root, value, aabb); }
    void update(const T &value, const AABB &new_aabb)
    {
        removeRecursive(m_root, value);
        insertRecursive(m_root, value, new_aabb);
    }
    void remove(const T &value) { removeRecursive(m_root, value); }

    void clear() { m_root.reset(); }

    void query(const AABB &aabb, std::vector<T> &results) const
    {
        queryRecursive(m_root, aabb, results);
    }

    template<typename Callback>
    void query(const AABB &aabb, Callback &&callback) const
    {
        queryRecursive(m_root, aabb, std::forward<Callback>(callback));
    }

    void query(const Frustum &frustum, std::vector<T> &results) const
    {
        queryRecursive(m_root, frustum, results);
    }

    template<typename Callback>
    void query(const Frustum &frustum, Callback &&callback) const
    {
        queryRecursive(m_root, frustum, std::forward<Callback>(callback));
    }

private:
    void insertRecursive(std::unique_ptr<Node> &node, const T &value, const AABB &aabb)
    {
        if (!node) {
            node = std::make_unique<Node>();
            node->aabb = aabb;
            node->value = value;
            return;
        }

        if (node->isLeaf()) {
            auto old_entity = node->value;
            auto old_bounds = node->aabb;

            node->value = invalid_value;
            node->left = std::make_unique<Node>();
            node->right = std::make_unique<Node>();

            node->left->aabb = old_bounds;
            node->left->value = old_entity;

            node->right->aabb = aabb;
            node->right->value = value;

            node->aabb = node->left->aabb.merge(node->right->aabb);
        } else {
            float left_growth = computeGrowth(node->left->aabb, aabb);
            float right_growth = computeGrowth(node->right->aabb, aabb);

            if (left_growth < right_growth) {
                insertRecursive(node->left, value, aabb);
            } else {
                insertRecursive(node->right, value, aabb);
            }

            node->aabb = node->left->aabb.merge(node->right->aabb);
        }
    }

    bool removeRecursive(std::unique_ptr<Node> &node, const T &value)
    {
        if (!node)
            return false;

        if (node->isLeaf()) {
            if (node->value == value) {
                node.reset();
                return true;
            }
            return false;
        }

        bool removed_left = removeRecursive(node->left, value);
        bool removed_right = removeRecursive(node->right, value);

        if (removed_left || removed_right) {
            if (!node->left && !node->right) {
                node.reset();
            } else if (node->left && !node->right) {
                node = std::move(node->left);
            } else if (!node->left && node->right) {
                node = std::move(node->right);
            } else {
                node->aabb = node->left->aabb.merge(node->right->aabb);
            }
        }

        return removed_left || removed_right;
    }

    void queryRecursive(const std::unique_ptr<Node> &node,
                        const AABB &aabb,
                        std::vector<T> &results) const
    {
        if (!node || !node->aabb.intersects(aabb))
            return;

        if (node->isLeaf()) {
            results.push_back(node->value);
        } else {
            queryRecursive(node->left, aabb, results);
            queryRecursive(node->right, aabb, results);
        }
    }

    template<typename Callback>
    void queryRecursive(const std::unique_ptr<Node> &node,
                        const AABB &aabb,
                        Callback &&callback) const
    {
        if (!node || !node->aabb.intersects(aabb))
            return;

        if (node->isLeaf()) {
            callback(node->value);
        } else {
            queryRecursive(node->left, aabb, callback);
            queryRecursive(node->right, aabb, callback);
        }
    }

    void queryRecursive(const std::unique_ptr<Node> &node,
                        const Frustum &frustum,
                        std::vector<T> &results) const
    {
        if (!node || !frustum.intersectWithAABB(node->aabb))
            return;

        if (node->isLeaf()) {
            results.push_back(node->value);
        } else {
            queryRecursive(node->left, frustum, results);
            queryRecursive(node->right, frustum, results);
        }
    }

    template<typename Callback>
    void queryRecursive(const std::unique_ptr<Node> &node,
                        const Frustum &frustum,
                        Callback &&callback) const
    {
        if (!node || !frustum.intersectWithAABB(node->aabb))
            return;

        if (node->isLeaf()) {
            callback(node->value);
        } else {
            queryRecursive(node->left, frustum, callback);
            queryRecursive(node->right, frustum, callback);
        }
    }

    float computeGrowth(const AABB &aabb, const AABB &new_aabb) const
    {
        AABB merged_aabb = aabb.merge(new_aabb);
        return glm::length(merged_aabb.max - merged_aabb.min) - glm::length(aabb.max - aabb.min);
    }

private:
    std::unique_ptr<Node> m_root;
};

} // namespace ae

#endif // AE_BVH_H
