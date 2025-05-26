#ifndef AE_POSE_ANIMATION_H
#define AE_POSE_ANIMATION_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>
#include <unordered_map>
#include <vector>

using namespace glm;

namespace ae {

class PoseAnimation
{
public:
    struct KeyPosition
    {
        vec3 position;
        float time;
    };

    struct KeyRotation
    {
        quat rotation;
        float time;
    };

    struct KeyScale
    {
        vec3 scale;
        float time;
    };

    struct Bone
    {
        std::vector<KeyPosition> positions;
        std::vector<KeyRotation> rotations;
        std::vector<KeyScale> scales;
    };

    PoseAnimation();
    ~PoseAnimation() = default;

    const std::string &getName() const { return m_name; }
    void setName(const std::string &name);

    float getDuration() const { return m_duration; }
    void setDuration(float duration);

    int32_t getTicksPerSecond() const { return m_ticks_per_second; }
    void setTicksPerSecond(int32_t ticks_per_second);

    void addBone(const std::string &name, const Bone &bone);
    bool contains(const std::string &name) const;
    const Bone &getBone(const std::string &name) const;

    vec3 interpolatePosition(const std::string &name, float animation_time) const;
    quat interpolateRotation(const std::string &name, float animation_time) const;
    vec3 interpolateScaling(const std::string &name, float animation_time) const;

private:
    float getScaleFactor(float last_time, float next_time, float animation_time) const;
    template<typename T>
    int32_t getIndex(const std::vector<T> &keys, float animation_time) const
    {
        int32_t left = 0, right = keys.size() - 1;
        while (left <= right) {
            int32_t mid = (left + right) / 2;
            if (animation_time < keys[mid].time)
                right = mid - 1;
            else
                left = mid + 1;
        }
        return std::max(0, left - 1);
    }

private:
    std::string m_name;
    float m_duration;
    int32_t m_ticks_per_second;
    std::vector<Bone> m_bones;
    std::unordered_map<std::string, int32_t> m_name_to_index;
};

} // namespace ae

#endif // AE_POSE_ANIMATION_H
