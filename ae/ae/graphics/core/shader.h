#ifndef AE_SHADER_H
#define AE_SHADER_H

#include "../common/enums.h"

#include <glm/glm.hpp>

#include <filesystem>
#include <unordered_map>
#include <vector>

using namespace glm;

namespace ae {

class Shader
{
public:
    Shader();
    Shader(const std::filesystem::path &path, ShaderType type);
    Shader(const std::filesystem::path &vertex_path, const std::filesystem::path &fragment_path);
    Shader(const std::filesystem::path &vertex_path,
           const std::filesystem::path &geometry_path,
           const std::filesystem::path &fragment_path);
    Shader(const std::string &shader, ShaderType type);
    Shader(const std::string &vertex_shader, const std::string &fragment_shader);
    Shader(const std::string &vertex_shader,
           const std::string &geometry_shader,
           const std::string &fragment_shader);
    ~Shader();

    bool loadFromFile(const std::filesystem::path &path, ShaderType type);
    bool loadFromFile(const std::filesystem::path &vertex_path,
                      const std::filesystem::path &fragment_path);
    bool loadFromFile(const std::filesystem::path &vertex_path,
                      const std::filesystem::path &geometry_path,
                      const std::filesystem::path &fragment_path);

    bool loadFromMemory(const std::string &shader, ShaderType type);
    bool loadFromMemory(const std::string &vertex_shader, const std::string &fragment_shader);
    bool loadFromMemory(const std::string &vertex_shader,
                        const std::string &geometry_shader,
                        const std::string &fragment_shader);

    uint32_t getId() const;

    bool isValid() const;
    void destroy();

    void uniformMatrix(const std::string &name, const mat4 &matrix) const;
    void uniformVec3(const std::string &name, const vec3 &vec) const;
    void uniformVec4(const std::string &name, const vec4 &vec) const;
    void uniformFloat(const std::string &name, float value) const;
    void uniformInt(const std::string &name, int32_t value) const;

    static void use(const Shader &shader);
    static void unuse();

private:
    std::string preprocessor(const std::string &shader);

    uint32_t getUniformLocation(const std::string &name) const;
    uint32_t createShader(const std::string &shader, ShaderType type) const;
    uint32_t createProgramm(const std::vector<uint32_t> &shaders) const;

private:
    uint32_t m_id;
    mutable std::unordered_map<std::string, uint32_t> m_uniform_locations;
};

} // namespace ae

#endif // AE_SHADER_H
