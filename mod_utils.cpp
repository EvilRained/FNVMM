#include "mod_utils.hpp"
#include "config_utils.h"
#include <iostream>
#include <fstream>
#include <regex>

namespace mod_utils {




void create_symlinks_recursive(const fs::path& source_dir,
                               const fs::path& target_dir,
                               bool overwrite) {
    try {
        fs::create_directories(target_dir);

        for (const auto& entry : fs::directory_iterator(source_dir)) {
            const fs::path& src_path = entry.path();
            fs::path dest_path = target_dir / src_path.filename();

            if (fs::is_directory(src_path)) {
                create_symlinks_recursive(src_path, dest_path, overwrite);
            } else if (fs::is_regular_file(src_path)) {
                try {
                    if (overwrite && fs::exists(dest_path)) {
                        fs::remove(dest_path);
                    }

                    fs::create_symlink(src_path, dest_path);
                    std::cout << "Created symlink: " << dest_path
                              << " -> " << src_path << std::endl;
                } catch (const fs::filesystem_error& e) {
                    std::cerr << "Error creating symlink " << dest_path
                              << ": " << e.what() << std::endl;
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}


void modCopy(const std::string& modname, const Config &cfg) {
    std::filesystem::path gamedir_path(cfg.gamedir);

    for (auto& file : std::filesystem::recursive_directory_iterator("mods/" + modname))
    {
        std::string filename = file.path().filename().string();
        std::string completepath = (std::filesystem::current_path() / file).string(); //mod folder & file
        std::string targetpath = (gamedir_path / filename).string(); //game_folder

        std::filesystem::copy(completepath,targetpath, fs::copy_options::skip_existing | fs::copy_options::recursive);
        std::cout << "Linking: " + completepath + " --> " + targetpath + "\n";

    }

    if(fs::exists(gamedir_path.string() + "/Plugins"))
    {
        for(auto& file : std::filesystem::recursive_directory_iterator(gamedir_path.string() + "Plugins"))
        {
            std::string filename = file.path().filename().string();
            std::string plugin_new_path = gamedir_path.string() + "/Data/NVSE/Plugins/" + filename;
            std::cout << "\n  Moving: " + file.path().string() + " --> " +  gamedir_path.string() ;
            fs::rename(file, plugin_new_path);
        }
    }

    std::cout << "\n \nLinking Complete!\n";
}


void link_mod(const std::string& modname, const Config &cfg) {


    std::regex pattern("nvse");
    std::filesystem::path gamedir_path(cfg.gamedir);

    if(std::regex_search(modname, pattern)) {
        std::cout << "\nSetting up Nvse..\n";
        nvse_mod(modname, cfg);
    } else {

        modCopy(modname, cfg);


        }
    }


void nvse_mod(const std::string& modname, const Config &cfg) {

    std::filesystem::path gamedir_path(cfg.gamedir);
    std::string game_exe_path = gamedir_path.string() + "FalloutNVLauncher.exe";
    std::string backup_game_exe_path = gamedir_path.string() + "FalloutNVLauncher_old.exe";
    std::string nvse_exe_path = gamedir_path.string() + "nvse_loader.exe";


    modCopy(modname, cfg);


    if(std::filesystem::exists(game_exe_path)) {
        std::cout << "\nBacking up: " + game_exe_path + " --> " + backup_game_exe_path;
        fs::rename(game_exe_path, backup_game_exe_path);
        std::cout << "\nMoving up: " + nvse_exe_path + " --> " + game_exe_path;
        fs::rename(nvse_exe_path, game_exe_path);
    }
}







}
