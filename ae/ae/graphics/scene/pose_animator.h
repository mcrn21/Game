#ifndef AE_POSE_ANIMATOR_H
#define AE_POSE_ANIMATOR_H

#include "pose_animation.h"
#include "pose.h"

namespace ae {

class PoseAnimator
{
public:
    PoseAnimator();
    PoseAnimator(const s_ptr<PoseAnimation> &animation, const s_ptr<Pose> &pose);
    ~PoseAnimator() = default;

    void setAnimation(const s_ptr<PoseAnimation> &animation);
    void setPose(const s_ptr<Pose> &pose);

    void play();
    void update(float dt);

private:
    void recursiveUpdate(int32_t node_index);
    void calculateBoneLocalTransform();

private:
    s_ptr<PoseAnimation> m_animation;
    s_ptr<Pose> m_pose;
    float m_current_time;
    float m_delta_time;
};

} // namespace ae

#endif // AE_POSE_ANIMATOR_H
