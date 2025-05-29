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
    ModelInstance(const s_ptr<Model> &model);
    ~ModelInstance() = default;

    const s_ptr<Model> &getModel() const;
    void setModel(const s_ptr<Model> &model);

    const s_ptr<Pose> &getPose() const;

    const AABB &getAABB() const;
    bool isTransparent() const;
    void draw(const RenderState &render_state) const;

private:
    void recursiveDraw(const s_ptr<MeshNode> &node, const RenderState &render_state) const;

private:
    s_ptr<Model> m_model;
    s_ptr<Pose> m_pose;
    AABB m_aabb;
    bool m_transparent;
};

} // namespace ae

#endif // AE_MODEL_INSTANCE_H
