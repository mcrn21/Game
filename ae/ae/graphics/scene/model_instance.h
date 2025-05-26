#ifndef AE_MODEL_INSTANCE_H
#define AE_MODEL_INSTANCE_H

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
    ModelInstance(const std::shared_ptr<Model> &model);
    ~ModelInstance() = default;

    const std::shared_ptr<Model> &getModel() const;
    void setModel(const std::shared_ptr<Model> &model);

    const std::shared_ptr<Pose> &getPose() const;

    const AABB &getAABB() const;
    bool isTransparent() const;
    void draw(const RenderState &render_state) const;

private:
    void recursiveDraw(const std::shared_ptr<MeshNode> &node, const RenderState &render_state) const;

private:
    std::shared_ptr<Model> m_model;
    std::shared_ptr<Pose> m_pose;
    AABB m_aabb;
    bool m_transparent;
};

} // namespace ae

#endif // AE_MODEL_INSTANCE_H
