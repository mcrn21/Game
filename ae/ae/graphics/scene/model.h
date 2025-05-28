#ifndef AE_MODEL_H
#define AE_MODEL_H

#include "../../system/memory.h"
#include "mesh.h"
#include "pose_animation.h"
#include "skeleton.h"

#include <filesystem>
#include <unordered_map>

namespace ae {

class MeshNode
{
public:
    MeshNode() = default;
    ~MeshNode() = default;

    const std::vector<SharedPtr<Mesh>> &getMeshes() const;
    void setMeshes(const std::vector<SharedPtr<Mesh>> &meshes);

    const std::vector<SharedPtr<MeshNode>> &getChildren() const;
    void setChildren(const std::vector<SharedPtr<MeshNode>> &child_nodes);

    const mat4 &getTransform() const;
    void setTransform(const mat4 &transform);

private:
    std::vector<SharedPtr<Mesh>> m_meshes;
    std::vector<SharedPtr<MeshNode>> m_children;
    mat4 m_transform{1.0f};
};

class Model
{
public:
    Model();
    ~Model() = default;

    bool loadFromFile(const std::filesystem::path &path);

    const SharedPtr<MeshNode> &getRootNode() const;
    void setRootNode(const SharedPtr<MeshNode> &node);

    const AABB &getAABB() const;
    bool isTransparent() const;

    const SharedPtr<Skeleton> &getSkeleton() const;
    void setSkeleton(const SharedPtr<Skeleton> &skeleton);

    SharedPtr<PoseAnimation> getAnimation(const std::string &name) const;
    std::vector<SharedPtr<PoseAnimation>> &getAnimations();
    void setAnimations(const std::vector<SharedPtr<PoseAnimation>> &animations);

private:
    AABB makeRecursiveAABB(const SharedPtr<MeshNode> &node) const;
    bool makeRecursiveTransparent(const SharedPtr<MeshNode> &node) const;

private:
    SharedPtr<MeshNode> m_root_node;
    AABB m_aabb;
    bool m_transparent;

    SharedPtr<Skeleton> m_skeleton;
    std::vector<SharedPtr<PoseAnimation>> m_animations;
    std::unordered_map<std::string, int32_t> m_name_animation_to_index;
};

} // namespace ae

#endif // AE_MODEL_H
