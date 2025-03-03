#include "config_utils.h"
#include <fstream>
#include <iostream>
#include <sstream>

bool writeConfigToIni(const Config& cfg, const std::string& filePath) {
    std::ofstream outFile(filePath);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << " for writing.\n";
        return false;
    }
    outFile << "gamedir=" << cfg.gamedir << "\n";
    outFile << "steamdir=" << cfg.steamdir << "\n";
    outFile << "init=" << cfg.init << "\n";
    outFile.close();
    return true;
}

bool readConfigFromIni(Config& cfg, const std::string& filePath) {
    std::ifstream inFile(filePath);
    if (!inFile.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << " for reading.\n";
        return false;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            if(key == "gamedir") cfg.gamedir = value;
            if(key == "steamdir") cfg.steamdir = value;
            if(key == "init") cfg.init = std::stoi(value);
        }
    }
    inFile.close();
    return true;
}

void welcomeConfig(Config& cfg) {
    bool validExe = false;
    std::string exe_path;
    std::string path;
    do {
    std::cout << "\n \n What is path to your Directory \n";
    std::getline(std::cin, path);

    exe_path = path + "FalloutNVLauncher.exe";
    if(!std::filesystem::exists(exe_path)) {
        std::cout << "Path does not contain FalloutNVLauncher.exe \n";
    } else {
        std::cout << "\nFalloutNVLauncher Found...\n";
        cfg.gamedir = path;
        validExe = true;
    }
    } while (!validExe);
    std::string data_path = path + "Data/";
    std::string data_path_backup = path + ".Data_Old/";
    std::string completepath = std::filesystem::current_path().string() + "/Data/";
    std::string data_bu = path + "Data";


    cfg.init = 1;
    writeConfigToIni(cfg, "config.ini");
    std::cout << "\nCreating Virtual Drive...\n";
    std::filesystem::create_directories("./Data");
    std::filesystem::copy(data_path,"./Data", fs::copy_options::skip_existing | fs::copy_options::recursive);
    std::filesystem::rename(data_path,data_path_backup);
    std::filesystem::rename("./Data", data_path);
    std::cout << completepath;

}
