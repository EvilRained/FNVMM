// symlink_utils.hpp
#ifndef MOD_UTILS_H
#define MOD_UTILS_H

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include "config_utils.h"

namespace fs = std::filesystem;

namespace mod_utils {
void create_symlinks_recursive(const fs::path& source_dir,
                               const fs::path& target_dir,
                               bool overwrite = false);



void install_nvse(const std::string& version);

void link_mod(const std::string& modname, const Config &cfg);

void nvse_mod(const std::string& modname, const Config &cfg);




}

#endif // MOD_UTILS_H
