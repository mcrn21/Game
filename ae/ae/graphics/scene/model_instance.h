#ifndef AE_MODEL_INSTANCE_H
#define AE_MODEL_INSTANCE_H

#include "../../system/memory.h"
#include "drawable.h"
#include "model.h"
#include "pose.h"

#include <glm/glm.hpp>

using namespace glm;

namespace ae {

class ModelInstance : public Drawable
{
public:
    ModelInstance();
    ModelInstance(const SharedPtr<Model> &model);
    ~ModelInstance() = default;

    const SharedPtr<Model> &getModel() const;
    void setModel(const SharedPtr<Model> &model);

    const SharedPtr<Pose> &getPose() const;

    const AABB &getAABB() const;
    bool isTransparent() const;
    void draw(const RenderState &render_state) const;

private:
    void recursiveDraw(const SharedPtr<MeshNode> &node, const RenderState &render_state) const;

private:
    SharedPtr<Model> m_model;
    SharedPtr<Pose> m_pose;
    AABB m_aabb;
    bool m_transparent;
};

} // namespace ae

#endif // AE_MODEL_INSTANCE_H
