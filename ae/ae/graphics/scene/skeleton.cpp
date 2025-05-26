#include "skeleton.h"

namespace ae {

Skeleton::Skeleton()
    : m_root_index{0}
{}

int32_t Skeleton::addNode(const std::string &name, const mat4 &transform)
{
    if (m_name_to_index.contains(name))
        return m_name_to_index[name];

    Node node;
    node.name = name;
    node.transform = transform;
    int32_t index = static_cast<int32_t>(m_nodes.size());
    m_nodes.push_back(node);
    m_name_to_index[name] = index;

    return index;
}

void Skeleton::setParent(int32_t index, int32_t parent_index)
{
    m_nodes[index].parent = parent_index;
    m_nodes[parent_index].children.push_back(index);
}

int32_t Skeleton::setBone(int32_t index, const mat4 &offset_transform)
{
    if (m_nodes[index].bone_index != -1) {
        m_bones[m_nodes[index].bone_index].offset_transform = offset_transform;
        return m_nodes[index].bone_index;
    }

    Bone bone;
    bone.node_index = index;
    bone.offset_transform = offset_transform;

    int32_t bone_index = static_cast<int32_t>(m_bones.size());
    m_bones.push_back(bone);

    m_nodes[index].bone_index = bone_index;

    return bone_index;
}

const Skeleton::Node &Skeleton::getNode(int32_t index) const
{
    return m_nodes[index];
}

int32_t Skeleton::getNodeIndex(const std::string &name) const
{
    return m_name_to_index.at(name);
}

const Skeleton::Bone &Skeleton::getBone(int32_t index) const
{
    return m_bones[index];
}

int32_t Skeleton::getRootIndex() const
{
    return m_root_index;
}

void Skeleton::setRootIndex(int32_t index)
{
    m_root_index = index;
}

int32_t Skeleton::getNodeCount() const
{
    return m_nodes.size();
}

int32_t Skeleton::getBoneCount() const
{
    return m_bones.size();
}

} // namespace ae
