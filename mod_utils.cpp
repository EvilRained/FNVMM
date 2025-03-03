#include "mod_utils.hpp"
#include "config_utils.h"
#include "mod_db.h"
#include <iostream>
#include <fstream>
#include <regex>
#include "utils.h"

namespace mod_utils {


void modCopy(const std::string& modname, const Config &cfg, std::string option = "0") {
    sqlite3* db;
    sqlite3_open("mods.db", &db);
    initDatabase(db);

    std::filesystem::path gamedir_path(cfg.gamedir);
    std::string gamedir = gamedir_path.string() + "Data/";
    std::string mod_base_path = "mods/" + modname;

    std::regex esp_esm("\\.(esp|esm)$");
    std::regex nvse(R"(^nvse_.*\.(dll|pdb|exe|ini))");
    std::regex textures(R"(/[tT][eE][xX][tT][uU][rR][eE][sS]/)");
    std::regex meshes(R"(/[mM][eE][sS][hH][eE][sS]/)");
    std::regex plugins(R"(/[pP][lL][uU][gG][iI][nN][sS]/)");
    std::regex config(R"(/[cC][oO][nN][fF][iI][gG]/)");
    std::regex menu(R"(/menus/)", std::regex::icase);
    std::regex uio(R"(/uio/)", std::regex::icase);

    std::string ini_path = cfg.steamdir + "users/steamuser/AppData/Local/FalloutNV/plugins.txt";
    std::string nvse_dir_path = gamedir_path.string() + "Data/NVSE/";

    for (auto& file : std::filesystem::recursive_directory_iterator(mod_base_path)) {
        int counter = 0;
        std::string filename = file.path().filename().string();
        std::string file_path = file.path().string();

        // Handle ESP/ESM files
        if (std::regex_search(filename, esp_esm)) {
            std::string esp_path = gamedir_path.string() + "Data/";
            if (fs::exists(esp_path)) {
                fs::copy(file.path(), esp_path, std::filesystem::copy_options::skip_existing);
                std::cout << "\n Moving ESP Files: " << filename << "\n";
                utils::append(ini_path, filename + "\n");
                Mod mod{filename, "/path1/FalloutNV.esm"};
                saveMod(db, mod);
            }
        }
        // Handle NVSE files
        else if (std::regex_search(filename, nvse)) {
            std::string game_exe_path = gamedir_path.string() + "FalloutNVLauncher.exe";
            std::string backup_game_exe_path = gamedir_path.string() + "FalloutNVLauncher_old.exe";
            std::string nvse_exe_path = gamedir_path.string() + "nvse_loader.exe";
            std::string NVSE_path = gamedir + "NVSE/nvse_config.ini";
            std::string nvse_path = gamedir_path.string() + "nvse_config.ini";

            fs::copy(file.path(), gamedir_path.string(), fs::copy_options::skip_existing);

            if (filename == "nvse_config.ini") {
                if (!fs::exists(nvse_dir_path)) fs::create_directory(nvse_dir_path);
                fs::rename(nvse_path, NVSE_path);
            }

            if (filename == "nvse_loader.exe") {
                std::cout << "\n Backing up: " + game_exe_path + " --> " + backup_game_exe_path;
                fs::rename(game_exe_path, backup_game_exe_path);
                std::cout << "\n Moving: " + nvse_exe_path + " --> " + game_exe_path;
                fs::rename(nvse_exe_path, game_exe_path);
            }
        }
        // Handle other files
        else {
            if (fs::is_directory(file.path())) continue;

            if (std::regex_search(file_path, textures)) {
                dataMove(file_path, gamedir + "Textures/", textures);
            }
            else if (std::regex_search(file_path, meshes)) {
                dataMove(file_path, gamedir + "Meshes/", meshes);
            }
            else if (std::regex_search(file_path, plugins)) {
                dataMove(file_path, gamedir + "NVSE/Plugins/", plugins);
            }
            else if (std::regex_search(file_path, config)) {
                dataMove(file_path, gamedir + "Config/", config);
            }
            else if (std::regex_search(file_path, menu)) {
                dataMove(file_path, gamedir + "menus/", menu);
            }
            else if (std::regex_search(file_path, uio)) {
                dataMove(file_path, gamedir + "uio/", uio);
            }
            else {

                fs::path full_source = file.path(); //mods/SomeModName/SomeModName/fomod/script.cs"
                fs::path source_root = full_source.parent_path().parent_path(); // "mods/"
                fs::path relative_path = full_source.lexically_relative(source_root); // "/fomod/script.cs"

                fs::path mod_name = source_root.filename(); // "SomeModName"
                fs::path inner_content = (relative_path.begin()->string() == mod_name.string())
                                             ? fs::path(relative_path.string().substr(mod_name.string().length() + 1))
                                             : relative_path;
                fs::path target_path = fs::path(gamedir) / inner_content; // "test/Data/fomod/script.cs"

                std::cout << "Copying to Data: " << full_source << " to " << target_path << "\n";

                if (!fs::exists(target_path.parent_path())) {
                    fs::create_directories(target_path.parent_path());
                }

                fs::copy(full_source, target_path, fs::copy_options::overwrite_existing);
            }
        }
    }
}

void link_mod(const std::string& modname, const Config &cfg) {
    std::filesystem::path gamedir_path(cfg.gamedir);
    modCopy(modname, cfg);
}

void dataMove(std::string file_path, std::string target_base_path, std::regex pattern) {
    if (std::regex_search(file_path, pattern)) {
        std::filesystem::path path_obj(file_path);
        std::string filename = path_obj.filename().string();
        std::string path_str = file_path;
        std::smatch matches;
        std::regex_search(path_str, matches, pattern);

        size_t pattern_pos = path_str.find(matches[0]);
        size_t subpath_start = pattern_pos + matches[0].length();

        std::string subpath = "";
        if (subpath_start < path_str.length()) {
            subpath = path_str.substr(subpath_start);
        }

        fs::path source_path(file_path);
        fs::path target_path;

        if (subpath.empty()) {
            target_path = fs::path(target_base_path) / source_path.filename();
        } else {
            target_path = fs::path(target_base_path) / subpath;
        }

        if (!fs::exists(target_path.parent_path())) {
            std::cout << "\nCreating directory: " << target_path.parent_path() << "\n";
            fs::create_directories(target_path.parent_path());
        }

        int counter = 0;
        counter++;
        std::cout << " \nLinking File:\n" << filename;
        fs::copy(source_path, target_path, fs::copy_options::overwrite_existing);
    }
}

}
