#include "model_instance.h"

namespace ae {

ModelInstance::ModelInstance()
    : m_transparent{false}
{}

ModelInstance::ModelInstance(const s_ptr<Model> &model)
    : m_transparent{false}
{
    setModel(model);
}

const s_ptr<Model> &ModelInstance::getModel() const
{
    return m_model;
}

void ModelInstance::setModel(const s_ptr<Model> &model)
{
    m_model = model;
    m_aabb = model ? model->getAABB() : AABB{};
    m_transparent = model ? model->isTransparent() : false;
    m_pose = model && model->getSkeleton() ? createShared<Pose>(model->getSkeleton()) : nullptr;
}

const s_ptr<Pose> &ModelInstance::getPose() const
{
    return m_pose;
}

const AABB &ModelInstance::getAABB() const
{
    return m_aabb;
}

bool ModelInstance::isTransparent() const
{
    return m_transparent;
}

void ModelInstance::draw(const RenderState &render_state) const
{
    if (m_pose && !m_pose->getFinalTransforms().empty()) {
        render_state.shader->uniformInt("u_skeleton", true);

        auto transforms = m_pose->getFinalTransforms();
        for (int32_t i = 0; i < transforms.size(); ++i)
            render_state.shader->uniformMatrix("u_finalBonesMats[" + std::to_string(i) + "]",
                                               transforms[i]);
    }

    if (m_model && m_model->getRootNode())
        recursiveDraw(m_model->getRootNode(), render_state);
}

void ModelInstance::recursiveDraw(const s_ptr<MeshNode> &node,
                                  const RenderState &render_state) const
{
    auto new_render_state = render_state;
    new_render_state.transform *= node->getTransform();
    for (const auto &mesh : node->getMeshes())
        mesh->draw(new_render_state);

    for (const auto &child : node->getChildren())
        recursiveDraw(child, new_render_state);
}

} // namespace ae
