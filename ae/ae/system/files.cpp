#include "files.h"

#include <fstream>

namespace ae {

std::string Files::readFile(const std::filesystem::path &path)
{
    if (path.empty())
        throw std::runtime_error("error when reading the file: the path is empty");

    auto a_path = std::filesystem::absolute(path);

    std::ifstream file_stream;
    file_stream.open(a_path.string());

    std::stringstream str_stream;
    str_stream << file_stream.rdbuf();

    file_stream.close();

    return str_stream.str();
}

} // namespace ae
