#ifndef AE_MATERIAL_H
#define AE_MATERIAL_H

#include "../../system/memory.h"
#include "color.h"
#include "texture.h"

namespace ae {

struct Material
{
    Material()
        : diffuse_texture{Texture::getDefaultDiffuseTexture()}
        , specular_texture{Texture::getDefaultSpecularTexture()}
        , color{Color::white}
        , shininess{32.0f}
    {}

    Material(const SharedPtr<Texture> &diffuse_texture,
             const SharedPtr<Texture> &specular_texture = {},
             const Color &color = Color::white,
             float shininess = 32.0f)
        : diffuse_texture{diffuse_texture}
        , specular_texture{specular_texture ? specular_texture
                                            : Texture::getDefaultSpecularTexture()}
        , color{color}
        , shininess{shininess}
    {}

    bool isTransparent() const
    {
        return (diffuse_texture && diffuse_texture->getFormat() == TextureFormat::RGBA)
               || (color.getAlpha() < 1.0f);
    }

    SharedPtr<Texture> diffuse_texture;
    SharedPtr<Texture> specular_texture;
    Color color;
    float shininess;
};

} // namespace ae

#endif // AE_MATERIAL_H
