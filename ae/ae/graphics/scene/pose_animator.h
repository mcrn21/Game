#ifndef AE_POSE_ANIMATOR_H
#define AE_POSE_ANIMATOR_H

#include "pose_animation.h"
#include "pose.h"

namespace ae {

class PoseAnimator
{
public:
    PoseAnimator();
    PoseAnimator(const SharedPtr<PoseAnimation> &animation, const SharedPtr<Pose> &pose);
    ~PoseAnimator() = default;

    void setAnimation(const SharedPtr<PoseAnimation> &animation);
    void setPose(const SharedPtr<Pose> &pose);

    void play();
    void update(float dt);

private:
    void recursiveUpdate(int32_t node_index);
    void calculateBoneLocalTransform();

private:
    SharedPtr<PoseAnimation> m_animation;
    SharedPtr<Pose> m_pose;
    float m_current_time;
    float m_delta_time;
};

} // namespace ae

#endif // AE_POSE_ANIMATOR_H
