#ifndef AE_MODEL_H
#define AE_MODEL_H

#include "pose_animation.h"
#include "mesh.h"
#include "skeleton.h"

#include <filesystem>
#include <unordered_map>

namespace ae {

class MeshNode
{
public:
    MeshNode() = default;
    ~MeshNode() = default;

    const std::vector<std::shared_ptr<Mesh>> &getMeshes() const;
    void setMeshes(const std::vector<std::shared_ptr<Mesh>> &meshes);

    const std::vector<std::shared_ptr<MeshNode>> &getChildren() const;
    void setChildren(const std::vector<std::shared_ptr<MeshNode>> &child_nodes);

    const mat4 &getTransform() const;
    void setTransform(const mat4 &transform);

private:
    std::vector<std::shared_ptr<Mesh>> m_meshes;
    std::vector<std::shared_ptr<MeshNode>> m_children;
    mat4 m_transform{1.0f};
};

class Model
{
public:
    Model();
    ~Model() = default;

    bool loadFromFile(const std::filesystem::path &path);

    const std::shared_ptr<MeshNode> &getRootNode() const;
    void setRootNode(const std::shared_ptr<MeshNode> &node);

    const AABB &getAABB() const;
    bool isTransparent() const;

    const std::shared_ptr<Skeleton> &getSkeleton() const;
    void setSkeleton(const std::shared_ptr<Skeleton> &skeleton);

    std::shared_ptr<PoseAnimation> getAnimation(const std::string &name) const;
    std::vector<std::shared_ptr<PoseAnimation>> &getAnimations();
    void setAnimations(const std::vector<std::shared_ptr<PoseAnimation>> &animations);

private:
    AABB makeRecursiveAABB(const std::shared_ptr<MeshNode> &node) const;
    bool makeRecursiveTransparent(const std::shared_ptr<MeshNode> &node) const;

private:
    std::shared_ptr<MeshNode> m_root_node;
    AABB m_aabb;
    bool m_transparent;

    std::shared_ptr<Skeleton> m_skeleton;
    std::vector<std::shared_ptr<PoseAnimation>> m_animations;
    std::unordered_map<std::string, int32_t> m_name_animation_to_index;
};

} // namespace ae

#endif // AE_MODEL_H
