#ifndef AE_CONFIG_H
#define AE_CONFIG_H

#include <glm/glm.hpp>

#include <filesystem>
#include <optional>
#include <string>

using namespace glm;

namespace ae {

struct Config
{
    static std::optional<Config> loadFromFile(const std::filesystem::path &path);

    // Window
    int32_t window_width = 800;
    int32_t window_height = 600;
    std::string window_title = "ae";

    // Game
    int32_t game_frame_rate = 60;
};

} // namespace ae

#endif // AE_CONFIG_H
