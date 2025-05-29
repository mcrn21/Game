#include "utils.h"

namespace ae::utils {

std::string trimString(const std::string &str)
{
    auto begin = std::ranges::find_if_not(str.begin(), str.end(), ::isspace);
    auto end = std::ranges::find_if_not(str.rbegin(), str.rend(), ::isspace).base();
    return (begin >= end) ? std::string{} : std::string(begin, end);
}

} // namespace ae::utils
