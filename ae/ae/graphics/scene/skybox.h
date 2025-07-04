#ifndef AE_SKYBOX_H
#define AE_SKYBOX_H

#include "../core/texture.h"
#include "../core/vertex_array.h"

namespace ae {

struct SkyboxVertex
{
    vec3 position;
};

template<>
inline const std::vector<VertexAttrib> &VertexAttrib::get<SkyboxVertex>()
{
    static std::vector<VertexAttrib> attribs = {
        {0, 3, DataType::FLOAT, false, offsetof(SkyboxVertex, position)}};
    return attribs;
}

class Skybox
{
public:
    Skybox();
    ~Skybox() = default;

    const s_ptr<Texture> &getTexture() const;
    void setTexture(const s_ptr<Texture> &texture);

    void create(const s_ptr<Texture> &texture);
    bool isValid() const;
    void destroy();

    void draw() const;

private:
    s_ptr<Texture> m_texture;
    VertexArray m_vertex_array;
};

} // namespace ae

#endif // AE_SKYBOX_H
