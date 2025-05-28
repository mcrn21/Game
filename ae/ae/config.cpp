#include "config.h"

#include <spdlog/spdlog.h>
#include <toml++/toml.h>

namespace ae {

std::optional<Config> Config::loadFromFile(const std::filesystem::path &path)
{
    try {
        std::optional<Config> config = Config{};

        auto toml_config = toml::parse_file(path.string());

        // Window
        config->window_width = toml_config["window"]["width"].value_or(config->window_width);
        config->window_height = toml_config["window"]["height"].value_or(config->window_height);
        config->window_title = toml_config["window"]["title"].value_or(config->window_title);
        config->msaa = toml_config["window"]["msaa"].value_or(config->msaa);

        //Game
        config->game_frame_rate = toml_config["game"]["frame_rate"].value_or(
            config->game_frame_rate);

        return config;
    } catch (const std::exception &e) {
        spdlog::error("Error: {}", e.what());
        return {};
    }
}

} // namespace ae
