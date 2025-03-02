cmake_minimum_required(VERSION 3.16)

project(Learning LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)
add_executable(Learning main.cpp
    config_utils.h config_utils.cpp
    mod_utils.hpp mod_utils.cpp)

include(GNUInstallDirs)
target_include_directories(Learning PRIVATE ${LIBGIT2_INCLUDE_DIR})
target_include_directories(Learning PUBLIC .)
target_link_libraries(Learning PRIVATE CURL::libcurl)
add_subdirectory(${CMAKE_SOURCE_DIR}/bit7z/) # Path to bit7z's repository

target_link_libraries(Learning PRIVATE bit7z)
install(TARGETS Learning
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
