// symlink_utils.hpp
#ifndef UTILS_H
#define UTILS_H
#include <filesystem>
#include <string>

namespace fs = std::filesystem;
namespace utils {
void append(const fs::path& filePath, const std::string& text);
}



#endif // UTILS_H
