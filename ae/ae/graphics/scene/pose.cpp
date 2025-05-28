#include "pose.h"

#include <glm/ext.hpp>
#include <glm/gtx/quaternion.hpp>

namespace ae {

Pose::Pose()
    : m_skeleton{nullptr}
    , m_dirty{false}
{}

Pose::Pose(const SharedPtr<Skeleton> &skeleton)
    : m_skeleton{skeleton}
    , m_dirty{true}
{
    setSkeleton(skeleton);
}

const SharedPtr<Skeleton> &Pose::getSkeleton() const
{
    return m_skeleton;
}

void Pose::setSkeleton(const SharedPtr<Skeleton> &skeleton)
{
    m_skeleton = nullptr;
    m_bone_states.clear();
    m_final_transforms.clear();
    m_bone_name_to_index.clear();

    if (skeleton) {
        m_skeleton = skeleton;
        int32_t bone_count = skeleton->getBoneCount();

        m_bone_states.resize(bone_count, BoneState{});
        m_final_transforms.resize(bone_count, mat4{1.0f});

        for (int32_t i = 0; i < skeleton->getNodeCount(); ++i) {
            const auto &node = skeleton->getNode(i);
            if (node.bone_index != -1)
                m_bone_name_to_index[node.name] = node.bone_index;
        }

        m_dirty = true;
    }
}

vec3 Pose::getPosition(const std::string &bone_name) const
{
    auto it = m_bone_name_to_index.find(bone_name);
    if (it != m_bone_name_to_index.end())
        return m_bone_states[it->second].position;
    return vec3{0.0f};
}

void Pose::setPosition(const std::string &bone_name, const vec3 &position)
{
    auto it = m_bone_name_to_index.find(bone_name);
    if (it != m_bone_name_to_index.end()) {
        m_bone_states[it->second].position = position;
        m_bone_states[it->second].dirty = true;
        m_dirty = true;
    }
}

vec3 Pose::getRotation(const std::string &bone_name) const
{
    auto it = m_bone_name_to_index.find(bone_name);
    if (it != m_bone_name_to_index.end())
        return m_bone_states[it->second].rotation;
    return vec3{0.0f};
}

void Pose::setRotation(const std::string &bone_name, const vec3 &rotation)
{
    auto it = m_bone_name_to_index.find(bone_name);
    if (it != m_bone_name_to_index.end()) {
        m_bone_states[it->second].rotation = rotation;
        m_bone_states[it->second].dirty = true;
        m_dirty = true;
    }
}

vec3 Pose::getScale(const std::string &bone_name) const
{
    auto it = m_bone_name_to_index.find(bone_name);
    if (it != m_bone_name_to_index.end())
        return m_bone_states[it->second].scale;
    return vec3{0.0f};
}

void Pose::setScale(const std::string &bone_name, const vec3 &scale)
{
    auto it = m_bone_name_to_index.find(bone_name);
    if (it != m_bone_name_to_index.end()) {
        m_bone_states[it->second].scale = scale;
        m_bone_states[it->second].dirty = true;
        m_dirty = true;
    }
}

void Pose::translate(const std::string &bone_name, const vec3 &translate)
{
    auto it = m_bone_name_to_index.find(bone_name);
    if (it != m_bone_name_to_index.end()) {
        m_bone_states[it->second].position += translate;
        m_bone_states[it->second].dirty = true;
        m_dirty = true;
    }
}

void Pose::rotate(const std::string &bone_name, const vec3 &rotation)
{
    auto it = m_bone_name_to_index.find(bone_name);
    if (it != m_bone_name_to_index.end()) {
        m_bone_states[it->second].rotation += rotation;
        m_bone_states[it->second].dirty = true;
        m_dirty = true;
    }
}

void Pose::scale(const std::string &bone_name, const vec3 &scale)
{
    auto it = m_bone_name_to_index.find(bone_name);
    if (it != m_bone_name_to_index.end()) {
        m_bone_states[it->second].scale += scale;
        m_bone_states[it->second].dirty = true;
        m_dirty = true;
    }
}

const std::vector<mat4> &Pose::getFinalTransforms() const
{
    if (m_dirty && m_skeleton) {
        const_cast<Pose *>(this)->recursiveUpdate(m_skeleton->getRootIndex(), mat4(1.0f));
        m_dirty = false;
    }
    return m_final_transforms;
}

mat4 Pose::makeLocalTransform(const vec3 &position, const vec3 &rotation, const vec3 &scale)
{
    quat q = quat(rotation);
    mat4 rot = glm::toMat4(q);
    return glm::translate(mat4(1.0f), position) * rot * glm::scale(mat4(1.0f), scale);
}

void Pose::recursiveUpdate(int32_t node_index, const mat4 &parent_transform)
{
    const auto &node = m_skeleton->getNode(node_index);
    mat4 global;

    if (node.bone_index != -1) {
        BoneState &state = m_bone_states[node.bone_index];
        const auto &bone = m_skeleton->getBone(node.bone_index);

        if (state.dirty) {
            const bool is_default = state.position == vec3(0.0f) && state.rotation == vec3(0.0f)
                                    && state.scale == vec3(1.0f);

            state.local_transform = is_default ? node.transform
                                               : makeLocalTransform(state.position,
                                                                    state.rotation,
                                                                    state.scale);

            state.dirty = false;
        }

        global = parent_transform * state.local_transform;

        state.global_transform = global;
        m_final_transforms[node.bone_index] = global * bone.offset_transform;
    } else {
        global = parent_transform * node.transform;
    }

    for (int32_t child : node.children)
        recursiveUpdate(child, global);
}

} // namespace ae
