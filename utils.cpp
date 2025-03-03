#include "utils.h"
#include <fstream>

namespace utils {

    namespace fs = std::filesystem;

    void append(const fs::path& filePath, const std::string& text) {
        std::ofstream out(filePath, std::ios::app); // Open in append mode
        if (out.is_open()) {
            out << text;
            out.close();
        } else {
            // Optional: Add error handling
            throw std::runtime_error("Failed to open file: " + filePath.string());
        }

    }

}
