#ifndef AE_SKELETON_H
#define AE_SKELETON_H

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>
#include <vector>

using namespace glm;

namespace ae {

class Skeleton
{
public:
    struct Node
    {
        std::string name;
        int32_t parent = -1;
        std::vector<int32_t> children;
        mat4 transform{1.0f};
        int32_t bone_index = -1;
    };

    struct Bone
    {
        int32_t node_index = -1;
        mat4 offset_transform{1.0f};
    };

    Skeleton();
    ~Skeleton() = default;

    int32_t addNode(const std::string &name, const mat4 &transform);
    void setParent(int32_t index, int32_t parent_index);
    int32_t setBone(int32_t index, const mat4 &offset_transform);

    const Node &getNode(int32_t index) const;
    int32_t getNodeIndex(const std::string &name) const;
    const Bone &getBone(int32_t index) const;

    int32_t getRootIndex() const;
    void setRootIndex(int32_t index);

    int32_t getNodeCount() const;
    int32_t getBoneCount() const;

private:
    int32_t m_root_index;
    std::vector<Node> m_nodes;
    std::unordered_map<std::string, int32_t> m_name_to_index;
    std::vector<int32_t> m_bones_indexes;
    std::vector<Bone> m_bones;
};

} // namespace ae

#endif // AE_SKELETON_H
