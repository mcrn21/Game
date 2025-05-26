#ifndef AE_DEFAULT_SHADERS_H
#define AE_DEFAULT_SHADERS_H

#include "shader.h"

namespace ae {

struct DefaultShaders
{
    static const std::unordered_map<std::string, std::string> &getShaderSources();
    static const std::string getShaderSource(const std::string &name);

    static std::shared_ptr<Shader> getSkybox();
    static std::shared_ptr<Shader> getMain();
    static std::shared_ptr<Shader> getScreenQuad();
    static std::shared_ptr<Shader> getGui();
};

} // namespace ae

#endif // AE_DEFAULT_SHADERS_H
