#include "shader.h"
#include "../../system/files.h"
#include "../../system/log.h"
#include "../common/utils.h"
#include "default_shaders.h"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <regex>

namespace ae {

Shader::Shader()
    : m_id{0}
{}

Shader::Shader(const std::filesystem::path &path, ShaderType type)
    : m_id{0}
{
    loadFromFile(path, type);
}

Shader::Shader(const std::filesystem::path &vertex_path, const std::filesystem::path &fragment_path)
    : m_id{0}
{
    loadFromFile(vertex_path, fragment_path);
}

Shader::Shader(const std::filesystem::path &vertex_path,
               const std::filesystem::path &geometry_path,
               const std::filesystem::path &fragment_path)
    : m_id{0}
{
    loadFromFile(vertex_path, geometry_path, fragment_path);
}

Shader::Shader(const std::string &shader, ShaderType type)
    : m_id{0}
{
    loadFromMemory(shader, type);
}

Shader::Shader(const std::string &vertex_shader, const std::string &fragment_shader)
    : m_id{0}
{
    loadFromMemory(vertex_shader, fragment_shader);
}

Shader::Shader(const std::string &vertex_shader,
               const std::string &geometry_shader,
               const std::string &fragment_shader)
    : m_id{0}
{
    loadFromMemory(vertex_shader, geometry_shader, fragment_shader);
}

Shader::~Shader()
{
    destroy();
}

bool Shader::loadFromFile(const std::filesystem::path &path, ShaderType type)
{
    try {
        std::string shader = Files::readFile(path);
        return loadFromMemory(shader, type);

    } catch (const std::exception &e) {
        l_error("Shader loading error: {}", e.what());
        return false;
    }
}

bool Shader::loadFromFile(const std::filesystem::path &vertex_path,
                          const std::filesystem::path &fragment_path)
{
    try {
        std::string vertex_shader = Files::readFile(vertex_path);
        std::string fragment_shader = Files::readFile(fragment_path);
        return loadFromMemory(vertex_shader, fragment_shader);

    } catch (const std::exception &e) {
        l_error("Shader loading error: {}", e.what());
        return false;
    }
}

bool Shader::loadFromFile(const std::filesystem::path &vertex_path,
                          const std::filesystem::path &geometry_path,
                          const std::filesystem::path &fragment_path)
{
    try {
        std::string vertex_shader = Files::readFile(vertex_path);
        std::string geometry_shader = Files::readFile(geometry_path);
        std::string fragment_shader = Files::readFile(fragment_path);
        return loadFromMemory(vertex_shader, geometry_shader, fragment_shader);

    } catch (const std::exception &e) {
        l_error("Shader loading error: {}", e.what());
        return false;
    }
}

bool Shader::loadFromMemory(const std::string &shader, ShaderType type)
{
    destroy();

    if (shader.empty()) {
        l_error("Shader loading error: shader is empty");
        return false;
    }

    auto p_shader = preprocessor(shader);

    m_id = createProgramm({createShader(p_shader, type)});
    if (m_id == 0)
        return false;

    return true;
}

bool Shader::loadFromMemory(const std::string &vertex_shader, const std::string &fragment_shader)
{
    destroy();

    if (vertex_shader.empty() || fragment_shader.empty()) {
        l_error("Shader loading error: shader is empty");
        return false;
    }

    auto p_vertex_shader = preprocessor(vertex_shader);
    auto p_fragment_shader = preprocessor(fragment_shader);

    m_id = createProgramm({createShader(p_vertex_shader, ShaderType::VERTEX),
                           createShader(p_fragment_shader, ShaderType::FRAGMENT)});
    if (m_id == 0)
        return false;

    return true;
}

bool Shader::loadFromMemory(const std::string &vertex_shader,
                            const std::string &geometry_shader,
                            const std::string &fragment_shader)
{
    destroy();

    if (vertex_shader.empty() || geometry_shader.empty() || fragment_shader.empty()) {
        l_error("Shader loading error: shader is empty");
        return false;
    }

    auto p_vertex_shader = preprocessor(vertex_shader);
    auto p_geometry_shader = preprocessor(geometry_shader);
    auto p_fragment_shader = preprocessor(fragment_shader);

    m_id = createProgramm({createShader(p_vertex_shader, ShaderType::VERTEX),
                           createShader(p_geometry_shader, ShaderType::GEOMETRY),
                           createShader(p_fragment_shader, ShaderType::FRAGMENT)});
    if (m_id == 0)
        return false;

    return true;
}

uint32_t Shader::getId() const
{
    return m_id;
}

bool Shader::isValid() const
{
    return m_id != 0;
}

void Shader::destroy()
{
    if (m_id != 0)
        glDeleteProgram(m_id);
}

void Shader::uniformMatrix(const std::string &name, const mat4 &matrix) const
{
    uint32_t location = getUniformLocation(name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::uniformVec3(const std::string &name, const vec3 &vec) const
{
    uint32_t location = getUniformLocation(name);
    glUniform3f(location, vec.x, vec.y, vec.z);
}

void Shader::uniformVec4(const std::string &name, const vec4 &vec) const
{
    uint32_t location = getUniformLocation(name);
    glUniform4f(location, vec.x, vec.y, vec.z, vec.w);
}

void Shader::uniformFloat(const std::string &name, float value) const
{
    uint32_t location = getUniformLocation(name);
    glUniform1f(location, value);
}

void Shader::uniformInt(const std::string &name, int32_t value) const
{
    GLuint location = getUniformLocation(name);
    glUniform1i(location, value);
}

void Shader::use(const Shader &shader)
{
    if (shader.m_id != 0)
        glUseProgram(shader.m_id);
}

void Shader::unuse()
{
    glUseProgram(0);
}

std::string Shader::preprocessor(const std::string &shader)
{
    std::string result;
    std::regex include_pattern("\\s*#include\\s\"(.*)\"");

    auto words_begin = std::sregex_iterator(shader.begin(), shader.end(), include_pattern);
    auto words_end = std::sregex_iterator();

    int32_t p = 0;

    for (std::sregex_iterator it = words_begin; it != words_end; ++it) {
        std::smatch match = *it;

        result += shader.substr(p, match.position() - p) + "\n";

        std::string included_file = match[1].str();
        result += DefaultShaders::getShaderSource(included_file) + "\n";

        p = match.position() + match.length(); // Правильно
    }

    result += shader.substr(p);

    return result;
}

uint32_t Shader::getUniformLocation(const std::string &name) const
{
    auto found = m_uniform_locations.find(name);
    if (found == m_uniform_locations.end()) {
        uint32_t location = glGetUniformLocation(m_id, name.c_str());
        if (location != -1)
            m_uniform_locations.try_emplace(name, location);
        return location;
    }
    return found->second;
}

uint32_t Shader::createShader(const std::string &shader, ShaderType type) const
{
    const GLchar *code = shader.c_str();
    GLuint shader_id = 0;
    GLint success = 0;
    GLchar info_log[512];

    shader_id = glCreateShader(graphics_utils::shaderTypeToGl(type));
    glShaderSource(shader_id, 1, &code, nullptr);
    glCompileShader(shader_id);
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader_id, 512, nullptr, info_log);
        l_error("Shader compilation error: {}", info_log);
        glDeleteShader(shader_id);
        return 0;
    }

    return shader_id;
}

uint32_t Shader::createProgramm(const std::vector<uint32_t> &shaders) const
{
    if (std::find(shaders.begin(), shaders.end(), 0) != shaders.end()) {
        for (const auto &shader_id : shaders) {
            if (shader_id != 0)
                glDeleteShader(shader_id);
        }
        return 0;
    }

    GLuint programm_id = glCreateProgram();

    for (uint32_t shader : shaders)
        glAttachShader(programm_id, shader);

    glLinkProgram(programm_id);

    GLint success = 0;
    GLchar info_log[512];

    glGetProgramiv(programm_id, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(programm_id, 512, nullptr, info_log);
        l_error("Shader programm linking error: {}", info_log);
        glDeleteProgram(programm_id);
        return 0;
    }

    for (const auto &shader_id : shaders)
        glDeleteShader(shader_id);

    return programm_id;
}

} // namespace ae
