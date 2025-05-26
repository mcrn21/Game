#ifndef AE_FILES_H
#define AE_FILES_H

#include <filesystem>
#include <string>

namespace ae {

struct Files
{
    static std::string readFile(const std::filesystem::path &path);
};

} // namespace ae

#endif // AE_FILES_H
