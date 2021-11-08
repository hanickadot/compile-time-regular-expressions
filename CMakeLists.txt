cmake_minimum_required(VERSION 3.8)

project(ctre VERSION 3.0 LANGUAGES NONE)

# ---- project() variable shims ----

# Added in CMake 3.9
set(PROJECT_DESCRIPTION "Fast compile-time regular expressions with support for matching/searching/capturing during compile-time or runtime.")

# Added in CMake 3.12
set(PROJECT_HOMEPAGE_URL "https://compile-time.re")

# Added in CMake 3.21
set(PROJECT_IS_TOP_LEVEL NO)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(PROJECT_IS_TOP_LEVEL YES)
endif()

# ---- Warning guard for vendoring projects ----

set(warning_guard "")
if(NOT PROJECT_IS_TOP_LEVEL)
  option(ctre_INCLUDES_WITH_SYSTEM
    "Use SYSTEM modifier for ctre's includes, disabling warnings" ON)
  mark_as_advanced(ctre_INCLUDES_WITH_SYSTEM)
  if(ctre_INCLUDES_WITH_SYSTEM)
    set(warning_guard SYSTEM)
  endif()
endif()

# ---- Declare library ----

add_library(ctre_ctre INTERFACE)
add_library(ctre::ctre ALIAS ctre_ctre)

set_property(TARGET ctre_ctre PROPERTY EXPORT_NAME ctre)

target_include_directories(ctre_ctre ${warning_guard} INTERFACE
  "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>")
target_compile_features(ctre_ctre INTERFACE cxx_std_17)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  # For hygienic reasons, install headers to their own directory (issue #207)
  if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_INSTALL_INCLUDEDIR include/ctre CACHE PATH "")
  endif()

  # The project is configured without a language, so this needs to be set
  set(CMAKE_INSTALL_LIBDIR lib CACHE PATH "")

  include(CMakePackageConfigHelpers)
  include(GNUInstallDirs)

  # For header-only libraries, everything goes in the -dev package
  set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME ctre_Development)

  install(DIRECTORY include/
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING PATTERN *.hpp)

  install(TARGETS ctre_ctre
    EXPORT ctre-targets
    DESTINATION dummy # required until 3.14
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

  # Replace with ARCH_INDEPENDENT once updated to CMake 3.14
  set(tmp_CMAKE_SIZEOF_VOID_P "${CMAKE_SIZEOF_VOID_P}")
  set(CMAKE_SIZEOF_VOID_P "")

  write_basic_package_version_file(ctre-config-version.cmake
    COMPATIBILITY SameMajorVersion)

  set(CMAKE_SIZEOF_VOID_P "${tmp_CMAKE_SIZEOF_VOID_P}")

  # Allow package maintainers to freely override the path for the configs
  set(ctre_INSTALL_CMAKEDIR "${CMAKE_INSTALL_DATADIR}/cmake/ctre"
    CACHE PATH "CMake package config location relative to the install prefix")
  mark_as_advanced(ctre_INSTALL_CMAKEDIR)

  install(FILES "${PROJECT_BINARY_DIR}/ctre-config-version.cmake"
    DESTINATION "${ctre_INSTALL_CMAKEDIR}")

  install(EXPORT ctre-targets
    FILE ctre-config.cmake
    DESTINATION "${ctre_INSTALL_CMAKEDIR}"
    NAMESPACE ctre::)

  option(ctre_ENABLE_PKGCONFIG "Install a .pc file for the project" ON)
  if(ctre_ENABLE_PKGCONFIG)
    # This can only detect issues on first configure, but it's still better
    # than nothing
    if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
      message(WARNING "The value of CMAKE_INSTALL_PREFIX was not set, "
        "but ctre_ENABLE_PKGCONFIG was enabled. This is likely an error.")
    endif()

    set(ctre_INSTALL_PKGCONFIGDIR "${CMAKE_INSTALL_DATADIR}/pkgconfig"
      CACHE FILEPATH "pkgconfig file location relative to the install prefix")
    mark_as_advanced(ctre_INSTALL_PKGCONFIG)

    configure_file(packaging/pkgconfig.pc.in ctre.pc @ONLY)
    install(FILES "${PROJECT_BINARY_DIR}/ctre.pc"
      DESTINATION "${ctre_INSTALL_PKGCONFIGDIR}")
  endif()

  if(PROJECT_IS_TOP_LEVEL)
    foreach(cpack_type IN ITEMS Config SourceConfig)
      configure_file(packaging/CPackDefaults.cmake.in
        "CPack${cpack_type}-ctre.cmake" @ONLY)
    endforeach()

    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
    set(CPACK_PACKAGE_HOMEPAGE_URL "${PROJECT_HOMEPAGE_URL}")
    include(CPack)
  endif()
endif()

# ---- Testing ----

if(PROJECT_IS_TOP_LEVEL)
  include(CTest)
  if(BUILD_TESTING)
    add_subdirectory(tests)
  endif()
endif()
