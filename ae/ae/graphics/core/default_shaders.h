#ifndef AE_DEFAULT_SHADERS_H
#define AE_DEFAULT_SHADERS_H

#include "../../system/memory.h"
#include "shader.h"

namespace ae {

struct DefaultShaders
{
    static const std::unordered_map<std::string, std::string> &getShaderSources();
    static const std::string getShaderSource(const std::string &name);

    static s_ptr<Shader> getSkybox();
    static s_ptr<Shader> getMain();
    static s_ptr<Shader> getScreenQuad();
    static s_ptr<Shader> getGui();
};

} // namespace ae

#endif // AE_DEFAULT_SHADERS_H
