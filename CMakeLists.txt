cmake_minimum_required(VERSION 3.8.0)
# When updating to a newer version of CMake, see if we can use the following
project(ctre
  HOMEPAGE_URL "https://compile-time.re"
  VERSION 3.8.1
  LANGUAGES CXX)
set(PROJECT_DESCRIPTION "Fast compile-time regular expressions with support for matching/searching/capturing during compile-time or runtime.")

include(CMakePackageConfigHelpers)
include(CMakeDependentOption)
include(GNUInstallDirs)
include(CTest)

find_program(CTRE_DPKG_BUILDPACKAGE_FOUND dpkg-buildpackage)
find_program(CTRE_RPMBUILD_FOUND rpmbuild)

cmake_dependent_option(CTRE_BUILD_TESTS "Build ctre Tests" ON
  "BUILD_TESTING;CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR" OFF)
cmake_dependent_option(CTRE_BUILD_PACKAGE "Build ctre Packages" ON
  "CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR" OFF)
cmake_dependent_option(CTRE_BUILD_PACKAGE_DEB
  "Create DEB Package (${PROJECT_NAME})" ON
  "CTRE_BUILD_PACKAGE;CTRE_DPKG_BUILDPACKAGE_FOUND" OFF)
cmake_dependent_option(CTRE_BUILD_PACKAGE_RPM
  "Create RPM Package (${PROJECT_NAME})" ON
  "CTRE_BUILD_PACKAGE;CTRE_RPMBUILD_FOUND" OFF)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

if (NOT CTRE_CXX_STANDARD)
  set(CTRE_CXX_STANDARD 20)
endif()

target_compile_features(ctre INTERFACE cxx_std_${CTRE_CXX_STANDARD})

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}-targets)

if (NOT EXISTS "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake.in")
  file(WRITE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake.in [[
    @PACKAGE_INIT@
    include("${CMAKE_CURRENT_LIST_DIR}/@PROJECT_NAME@-targets.cmake")
  ]])
endif()

configure_package_config_file(
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake.in"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}"
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file(ctre-config-version.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

install(EXPORT ${PROJECT_NAME}-targets 
  DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}"
  NAMESPACE ${PROJECT_NAME}::)
install(
  FILES
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  DESTINATION ${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include
    FILES_MATCHING PATTERN *.hpp)

if(CTRE_BUILD_TESTS)
  add_subdirectory(tests)
endif()

if (NOT CTRE_BUILD_PACKAGE)
  return()
endif()

list(APPEND source-generators TBZ2 TGZ TXZ ZIP)

if (CTRE_BUILD_PACKAGE_DEB)
  list(APPEND binary-generators "DEB")
endif()

if (CTRE_BUILD_PACKAGE_RPM)
  list(APPEND binary-generators "RPM")
endif()

set(CPACK_SOURCE_GENERATOR ${source-generators})
set(CPACK_GENERATOR ${binary-generators})

set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}")

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Hana Dusíková")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")
set(CPACK_DEBIAN_PACKAGE_NAME "lib${PROJECT_NAME}-dev")

set(CPACK_RPM_PACKAGE_NAME "lib${PROJECT_NAME}-devel")

set(PKG_CONFIG_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/packaging/pkgconfig.pc.in" "${PKG_CONFIG_FILE_NAME}" @ONLY)
install(FILES "${PKG_CONFIG_FILE_NAME}"
	DESTINATION "${CMAKE_INSTALL_DATADIR}/pkgconfig"
)

list(APPEND CPACK_SOURCE_IGNORE_FILES /.git/ /build/ .gitignore .DS_Store)

include(CPack)
