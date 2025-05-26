#include "pose_animator.h"

namespace ae {

PoseAnimator::PoseAnimator()
    : m_current_time{0.0f}
    , m_delta_time{0.0f}
{}

PoseAnimator::PoseAnimator(const std::shared_ptr<PoseAnimation> &animation,
                           const std::shared_ptr<Pose> &pose)
    : m_animation{animation}
    , m_pose{pose}
{}

void PoseAnimator::setAnimation(const std::shared_ptr<PoseAnimation> &animation)
{
    m_animation = animation;
}

void PoseAnimator::setPose(const std::shared_ptr<Pose> &pose)
{
    m_pose = pose;
}

void PoseAnimator::play()
{
    if (!m_animation || !m_pose)
        return;
    m_current_time = 0.0f;
    recursiveUpdate(m_pose->getSkeleton()->getRootIndex());
}

void PoseAnimator::update(float dt)
{
    if (!m_animation || !m_pose)
        return;

    m_delta_time = dt;
    m_current_time += m_animation->getTicksPerSecond() * dt;
    m_current_time = fmod(m_current_time, m_animation->getDuration());

    recursiveUpdate(m_pose->getSkeleton()->getRootIndex());
}

void PoseAnimator::recursiveUpdate(int32_t node_index)
{
    const auto &node = m_pose->getSkeleton()->getNode(node_index);
    if (m_animation->contains(node.name)) {
        m_pose->setPosition(node.name, m_animation->interpolatePosition(node.name, m_current_time));
        m_pose->setRotation(node.name,
                            glm::eulerAngles(
                                m_animation->interpolateRotation(node.name, m_current_time)));
        m_pose->setScale(node.name, m_animation->interpolateScaling(node.name, m_current_time));
    }

    for (int32_t child : node.children)
        recursiveUpdate(child);
}

} // namespace ae
