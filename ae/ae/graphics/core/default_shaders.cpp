#include "default_shaders.h"

#include "battery/embed.hpp"

namespace ae {

const std::unordered_map<std::string, std::string> &DefaultShaders::getShaderSources()
{
    static const std::unordered_map<std::string, std::string> sources
        = {{"shaders/main.vert", b::embed<"shaders/main.vert">().str()},
           {"shaders/main.frag", b::embed<"shaders/main.frag">().str()},
           {"shaders/light.inc", b::embed<"shaders/light.inc">().str()},
           {"shaders/material.inc", b::embed<"shaders/material.inc">().str()},
           {"shaders/skybox.vert", b::embed<"shaders/skybox.vert">().str()},
           {"shaders/skybox.frag", b::embed<"shaders/skybox.frag">().str()},
           {"shaders/screen_quad.vert", b::embed<"shaders/screen_quad.vert">().str()},
           {"shaders/screen_quad.frag", b::embed<"shaders/screen_quad.frag">().str()},
           {"shaders/gui.vert", b::embed<"shaders/gui.vert">().str()},
           {"shaders/gui.frag", b::embed<"shaders/gui.frag">().str()}};
    return sources;
}

const std::string DefaultShaders::getShaderSource(const std::string &name)
{
    auto sources = getShaderSources();
    return sources.contains(name) ? sources.at(name) : std::string{};
}

s_ptr<Shader> DefaultShaders::getSkybox()
{
    static auto shader = createShared<Shader>(getShaderSource("shaders/skybox.vert"),
                                              getShaderSource("shaders/skybox.frag"));
    return shader;
}

s_ptr<Shader> DefaultShaders::getMain()
{
    static auto shader = createShared<Shader>(getShaderSource("shaders/main.vert"),
                                              getShaderSource("shaders/main.frag"));
    return shader;
}

s_ptr<Shader> DefaultShaders::getScreenQuad()
{
    static auto shader = createShared<Shader>(getShaderSource("shaders/screen_quad.vert"),
                                              getShaderSource("shaders/screen_quad.frag"));
    return shader;
}

s_ptr<Shader> DefaultShaders::getGui()
{
    static auto shader = createShared<Shader>(getShaderSource("shaders/gui.vert"),
                                              getShaderSource("shaders/gui.frag"));
    return shader;
}

} // namespace ae
