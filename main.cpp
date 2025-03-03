#include <iostream>
#include <bit7z/bitfileextractor.hpp>
#include <string>
#include <fstream>
#include <filesystem>
#include <regex>
#include "config_utils.h"
#include "mod_utils.hpp"
namespace fs = std::filesystem;



void extractMod(Config& cfg, std::string mod, std::string type)
{
    std::string modname =  mod;
    size_t pos = modname.find('.');
    std::string cleanname = modname.substr(0, pos);

    try {
        using namespace bit7z;
        Bit7zLibrary lib{"./7z.so"};

        if(type == "zip") {
            BitFileExtractor extractor{ lib, BitFormat::Zip };

            extractor.extract( mod, "mods/" + cleanname );
            mod_utils::link_mod(cleanname, cfg);

        } else if (type=="7z") {
            BitFileExtractor extractor{ lib, BitFormat::SevenZip };
            extractor.extract( mod, "mods/" + cleanname );
            mod_utils::link_mod(cleanname, cfg);


        } else if (type=="rar") {
            BitFileExtractor extractor{ lib, BitFormat::Rar };
            extractor.extract( mod, "mods/" + cleanname );
            mod_utils::link_mod(cleanname, cfg);

        }

    } catch ( const bit7z::BitException& ex ) {std::cout << "error" << ex.what();}


    std::cout << "\n\n" + cleanname + " Has been added \n";
}



void setDirectory(Config& cfg, const std::string& dir, const std::string type = "") {
    if (fs::exists(dir)) {
        if(type == "steamdir") {
            cfg.steamdir= dir;
            std::cout << "Steam Dir set to: " << cfg.steamdir << "\n";

        } else if(type == "") {
            cfg.gamedir = dir; // Update the existing cfg
            std::cout << "Game Dir set to: " << cfg.gamedir << "\n";
        }
        if (writeConfigToIni(cfg, "config.ini")) {
            if(type == "") std::cout << "Game Dir Updated in config.ini\n"; else std::cout << "Steam Dir Updated in config.ini\n";
        } else {
            std::cerr << "Failed to write to file\n";
        }
    } else {
        std::cout << "Directory does not exist\n";
    }
}

void download(Config& cfg, const std::string& key) {
    if (fs::exists(key)) {
        std::cout << "\nValid Archive...\n";

        std::string filepath = key;
        std::regex filetype(R"(\.(zip|7z|tar\.gz)$)");
        std::smatch matches;

        if (std::regex_search(key, matches, filetype)) {
            std::cout << "\n Extracting...\n";

            std::string type = matches[1];
            extractMod(cfg, key, type);

        } else {
            std::cout << "File is not an Archive\n";
        }
    } else {
        std::cout << "\nFile Does Not Exist\n";
    }

}

int main(int argc, char* argv[]) {
    // Load config at startup
    Config cfg;
    if (fs::exists("config.ini")) {
        readConfigFromIni(cfg, "config.ini");
        std::cout << "Loaded config: gamedir=" << cfg.gamedir << "\nLoaded config: steamdir=" << cfg.steamdir << "\n\n";
    } else {
        std::cout << "No config.ini found, using defaults\n";
    }



    // Check arguments
    if (argc > 1 && argv[1] != nullptr) {
        std::string flag = argv[1];

        if (flag == "--install" && argc > 2 && argv[2] != nullptr) {
            std::cout << "Checking... " << argv[2] << "\n";
            download(cfg, argv[2]);
        } else if (flag == "--install") {
            std::cout << "Please enter link\n";
        }
        if (flag == "--steam-dir" && argc > 2 && argv[2] != nullptr) {
            setDirectory(cfg, argv[2], "steamdir");
        }
        if (flag == "--game-dir" && argc > 2 && argv[2] != nullptr) {
            setDirectory(cfg, argv[2]); // Pass cfg by reference
        } else if (flag == "--config") {
            std::cout << "\n Welcome to 23 Mods\n We have just a couple of steps...";
            welcomeConfig(cfg);
        }
    } else {
        std::cout << "Usage: --install <file> or --game_dir <dir>\n";
    }

    return 0;
}
