#ifndef CONFIG_UTILS_H // Include guard to prevent multiple inclusion
#define CONFIG_UTILS_H
#include <filesystem>

#include <string>
namespace fs = std::filesystem;

struct Config {
    std::string gamedir = "~/";
    std::string steamdir = "~/";
    int init = 0;
};

bool writeConfigToIni(const Config& cfg, const std::string& filePath);
bool readConfigFromIni(Config& cfg, const std::string& filePath); // New function
void setWelcomeInit(Config& cfg);
void welcomeConfig(Config& cfg);

#endif // CONFIG_UTILS_H
