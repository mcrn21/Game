#include "model.h"
#include "assimp_helper.h"

namespace ae {

const std::vector<SharedPtr<Mesh>> &MeshNode::getMeshes() const
{
    return m_meshes;
}

void MeshNode::setMeshes(const std::vector<SharedPtr<Mesh>> &meshes)
{
    m_meshes = meshes;
}

const std::vector<SharedPtr<MeshNode>> &MeshNode::getChildren() const
{
    return m_children;
}

void MeshNode::setChildren(const std::vector<SharedPtr<MeshNode>> &child_nodes)
{
    m_children = child_nodes;
}

const mat4 &MeshNode::getTransform() const
{
    return m_transform;
}

void MeshNode::setTransform(const mat4 &transform)
{
    m_transform = transform;
}

Model::Model()
    : m_transparent{false}
{}

bool Model::loadFromFile(const std::filesystem::path &path)
{
    AssimpHelper assimp_helper{path, nullptr};
    if (!assimp_helper.load(this))
        return false;

    return true;
}

const SharedPtr<MeshNode> &Model::getRootNode() const
{
    return m_root_node;
}

void Model::setRootNode(const SharedPtr<MeshNode> &node)
{
    m_root_node = node;
    m_aabb = AABB{};
    m_transparent = false;

    if (node) {
        m_aabb = makeRecursiveAABB(node);
        m_transparent = makeRecursiveTransparent(node);
    }
}

const AABB &Model::getAABB() const
{
    return m_aabb;
}

bool Model::isTransparent() const
{
    return m_transparent;
}

const SharedPtr<Skeleton> &Model::getSkeleton() const
{
    return m_skeleton;
}

void Model::setSkeleton(const SharedPtr<Skeleton> &skeleton)
{
    m_skeleton = skeleton;
}

SharedPtr<PoseAnimation> Model::getAnimation(const std::string &name) const
{
    if (!m_name_animation_to_index.contains(name))
        return nullptr;
    return m_animations.at(m_name_animation_to_index.at(name));
}

std::vector<SharedPtr<PoseAnimation>> &Model::getAnimations()
{
    return m_animations;
}

void Model::setAnimations(const std::vector<SharedPtr<PoseAnimation>> &animations)
{
    m_animations = animations;
    m_name_animation_to_index.clear();
    for (int32_t i = 0; i < animations.size(); ++i)
        m_name_animation_to_index[animations[i]->getName()] = i;
}

AABB Model::makeRecursiveAABB(const SharedPtr<MeshNode> &node) const
{
    AABB aabb{vec3{std::numeric_limits<float>::max()}, vec3{std::numeric_limits<float>::lowest()}};

    for (const auto &mesh : node->getMeshes())
        aabb = aabb.merge(mesh->getAABB());

    for (const auto &child : node->getChildren())
        aabb = aabb.merge(makeRecursiveAABB(child));

    return aabb;
}

bool Model::makeRecursiveTransparent(const SharedPtr<MeshNode> &node) const
{
    for (const auto &mesh : node->getMeshes()) {
        if (mesh->isTransparent())
            return true;
    }

    for (const auto &child : node->getChildren()) {
        if (makeRecursiveTransparent(child))
            return true;
    }

    return false;
}

} // namespace ae
