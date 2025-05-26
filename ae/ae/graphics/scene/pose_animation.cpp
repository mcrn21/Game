#include "pose_animation.h"

namespace ae {

PoseAnimation::PoseAnimation()
    : m_duration{0.0f}
    , m_ticks_per_second{0}
{}

void PoseAnimation::setName(const std::string &name)
{
    m_name = name;
}

void PoseAnimation::setDuration(float duration)
{
    m_duration = duration;
}

void PoseAnimation::setTicksPerSecond(int32_t ticks_per_second)
{
    m_ticks_per_second = ticks_per_second;
}

void PoseAnimation::addBone(const std::string &name, const Bone &bone)
{
    int32_t index = static_cast<int32_t>(m_bones.size());
    m_bones.push_back(bone);
    m_name_to_index[name] = index;
}

bool PoseAnimation::contains(const std::string &name) const
{
    return m_name_to_index.contains(name);
}

const PoseAnimation::Bone &PoseAnimation::getBone(const std::string &name) const
{
    return m_bones.at(m_name_to_index.at(name));
}

vec3 PoseAnimation::interpolatePosition(const std::string &name, float animation_time) const
{
    if (!m_name_to_index.contains(name))
        return vec3{0.0f};

    const Bone &bone = getBone(name);

    if (bone.positions.size() == 1)
        return bone.positions[0].position;

    int p0_index = getIndex(bone.positions, animation_time);
    int p1_index = p0_index + 1;

    float scale_factor = getScaleFactor(bone.positions[p0_index].time,
                                        bone.positions[p1_index].time,
                                        animation_time);
    vec3 final_position = glm::mix(bone.positions[p0_index].position,
                                   bone.positions[p1_index].position,
                                   scale_factor);

    return final_position;
}

quat PoseAnimation::interpolateRotation(const std::string &name, float animation_time) const
{
    if (!m_name_to_index.contains(name))
        return quat{};

    const Bone &bone = getBone(name);

    if (bone.rotations.size() == 1) {
        auto rotation = glm::normalize(bone.rotations[0].rotation);
        return rotation;
    }

    int p0_index = getIndex(bone.rotations, animation_time);
    int p1_index = p0_index + 1;

    float scaleFactor = getScaleFactor(bone.rotations[p0_index].time,
                                       bone.rotations[p1_index].time,
                                       animation_time);
    glm::quat final_rotation = glm::slerp(bone.rotations[p0_index].rotation,
                                          bone.rotations[p1_index].rotation,
                                          scaleFactor);

    return final_rotation;
}

vec3 PoseAnimation::interpolateScaling(const std::string &name, float animation_time) const
{
    if (!m_name_to_index.contains(name))
        return vec3{0.0f};

    const Bone &bone = getBone(name);

    if (bone.scales.size() == 1)
        return bone.scales[0].scale;

    int p0_index = getIndex(bone.scales, animation_time);
    int p1_index = p0_index + 1;

    float scale_factor = getScaleFactor(bone.scales[p0_index].time,
                                        bone.scales[p1_index].time,
                                        animation_time);
    glm::vec3 final_scale = glm::mix(bone.scales[p0_index].scale,
                                     bone.scales[p1_index].scale,
                                     scale_factor);

    return final_scale;
}

float PoseAnimation::getScaleFactor(float last_time, float next_time, float animation_time) const
{
    float mid_way_length = animation_time - last_time;
    float frames_diff = next_time - last_time;
    return mid_way_length / frames_diff;
}

} // namespace ae
