cmake_minimum_required(VERSION 3.8.0)
project(compile-time-regular-expressions VERSION 2.6.4 LANGUAGES CXX)

add_library(ctre INTERFACE)
add_library(ctre::ctre ALIAS ctre)
target_include_directories(ctre INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
target_compile_features(ctre INTERFACE cxx_std_17)

install(TARGETS ctre EXPORT ctre-targets)

export(EXPORT ctre-targets NAMESPACE ctre::)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/ctre-config.cmake
    "include(\${CMAKE_CURRENT_LIST_DIR}/ctre-targets.cmake)")
include(CMakePackageConfigHelpers)
write_basic_package_version_file(ctre-config-version.cmake
    VERSION ${ctre_VERSION}
    COMPATIBILITY SameMajorVersion)

install(EXPORT ctre-targets DESTINATION lib/cmake/ctre
    NAMESPACE ctre::)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/ctre-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/ctre-config-version.cmake
    DESTINATION lib/cmake/ctre)
install(DIRECTORY include/ DESTINATION include
    FILES_MATCHING PATTERN *.hpp)

option(ENABLE_CTRE_SELF_TEST "build self tests" OFF)

if(ENABLE_CTRE_SELF_TEST)
add_subdirectory(tests)
endif()