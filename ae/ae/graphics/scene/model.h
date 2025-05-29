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

    const std::vector<s_ptr<Mesh>> &getMeshes() const;
    void setMeshes(const std::vector<s_ptr<Mesh>> &meshes);

    const std::vector<s_ptr<MeshNode>> &getChildren() const;
    void setChildren(const std::vector<s_ptr<MeshNode>> &child_nodes);

    const mat4 &getTransform() const;
    void setTransform(const mat4 &transform);

private:
    std::vector<s_ptr<Mesh>> m_meshes;
    std::vector<s_ptr<MeshNode>> m_children;
    mat4 m_transform{1.0f};
};

class Model
{
public:
    Model();
    ~Model() = default;

    bool loadFromFile(const std::filesystem::path &path);

    const s_ptr<MeshNode> &getRootNode() const;
    void setRootNode(const s_ptr<MeshNode> &node);

    const AABB &getAABB() const;
    bool isTransparent() const;

    const s_ptr<Skeleton> &getSkeleton() const;
    void setSkeleton(const s_ptr<Skeleton> &skeleton);

    s_ptr<PoseAnimation> getAnimation(const std::string &name) const;
    std::vector<s_ptr<PoseAnimation>> &getAnimations();
    void setAnimations(const std::vector<s_ptr<PoseAnimation>> &animations);

private:
    AABB makeRecursiveAABB(const s_ptr<MeshNode> &node) const;
    bool makeRecursiveTransparent(const s_ptr<MeshNode> &node) const;

private:
    s_ptr<MeshNode> m_root_node;
    AABB m_aabb;
    bool m_transparent;

    s_ptr<Skeleton> m_skeleton;
    std::vector<s_ptr<PoseAnimation>> m_animations;
    std::unordered_map<std::string, int32_t> m_name_animation_to_index;
};

} // namespace ae

#endif // AE_MODEL_H
