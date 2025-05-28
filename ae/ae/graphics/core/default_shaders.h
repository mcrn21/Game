#ifndef AE_DEFAULT_SHADERS_H
#define AE_DEFAULT_SHADERS_H

#include "../../system/memory.h"
#include "shader.h"

namespace ae {

struct DefaultShaders
{
    static const std::unordered_map<std::string, std::string> &getShaderSources();
    static const std::string getShaderSource(const std::string &name);

    static SharedPtr<Shader> getSkybox();
    static SharedPtr<Shader> getMain();
    static SharedPtr<Shader> getScreenQuad();
    static SharedPtr<Shader> getGui();
};

} // namespace ae

#endif // AE_DEFAULT_SHADERS_H
