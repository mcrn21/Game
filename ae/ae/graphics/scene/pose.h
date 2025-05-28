#ifndef AE_POSE_H
#define AE_POSE_H

#include "../../system/memory.h"
#include "skeleton.h"

namespace ae {

class Pose
{
public:
    Pose();
    Pose(const SharedPtr<Skeleton> &skeleton);
    ~Pose() = default;

    const SharedPtr<Skeleton> &getSkeleton() const;
    void setSkeleton(const SharedPtr<Skeleton> &skeleton);

    vec3 getPosition(const std::string &bone_name) const;
    void setPosition(const std::string &bone_name, const vec3 &position);

    vec3 getRotation(const std::string &bone_name) const;
    void setRotation(const std::string &bone_name, const vec3 &rotation);

    vec3 getScale(const std::string &bone_name) const;
    void setScale(const std::string &bone_name, const vec3 &scale);

    void translate(const std::string &bone_name, const vec3 &translate);
    void rotate(const std::string &bone_name, const vec3 &rotation);
    void scale(const std::string &bone_name, const vec3 &scale);

    const std::vector<mat4> &getFinalTransforms() const;

private:
    mat4 makeLocalTransform(const vec3 &position, const vec3 &rotation, const vec3 &scale);
    void recursiveUpdate(int32_t node_index, const mat4 &parent_transform);

private:
    struct BoneState
    {
        vec3 position{0.0f};
        vec3 rotation{0.0f};
        vec3 scale{1.0f};
        mat4 local_transform{1.0f};
        mat4 global_transform{1.0f};
        bool dirty = true;
    };

    SharedPtr<Skeleton> m_skeleton;

    std::vector<BoneState> m_bone_states;
    std::vector<mat4> m_final_transforms;

    std::unordered_map<std::string, int32_t> m_bone_name_to_index;

    mutable bool m_dirty;
};

} // namespace ae

#endif // AE_POSE_H
