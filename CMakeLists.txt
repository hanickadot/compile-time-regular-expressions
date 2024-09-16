cmake_minimum_required(VERSION 3.14...3.29)

if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.29.20240416") 
	set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
	set(CMAKE_CXX_MODULE_STD 1)
endif()

# When updating to a newer version of CMake, see if we can use the following
project(ctre
  HOMEPAGE_URL "https://compile-time.re"
  VERSION 3.9.0
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

option(CTRE_MODULE "build C++ module" OFF)

if(CTRE_MODULE)
	if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.29.20240416") 
		add_library(${PROJECT_NAME})
		
		target_sources(${PROJECT_NAME} PUBLIC FILE_SET CXX_MODULES TYPE CXX_MODULES FILES ctre.cppm)
		target_sources(${PROJECT_NAME} PUBLIC FILE_SET HEADERS TYPE HEADERS
		    BASE_DIRS
			"${CMAKE_CURRENT_SOURCE_DIR}/include"
		    FILES 
			include/ctll.hpp
			include/ctre/functions.hpp
			include/ctre/utility.hpp
			include/ctre/utf8.hpp
			include/ctre/evaluation.hpp
			include/ctre/starts_with_anchor.hpp
			include/ctre/pcre_actions.hpp
			include/ctre/rotate.hpp
			include/ctre/iterators.hpp
			include/ctre/literals.hpp
			include/ctre/return_type.hpp
			include/ctre/find_captures.hpp
			include/ctre/id.hpp
			include/ctre/atoms_characters.hpp
			include/ctre/actions/mode.inc.hpp
			include/ctre/actions/characters.inc.hpp
			include/ctre/actions/class.inc.hpp
			include/ctre/actions/look.inc.hpp
			include/ctre/actions/sequence.inc.hpp
			include/ctre/actions/fusion.inc.hpp
			include/ctre/actions/asserts.inc.hpp
			include/ctre/actions/capture.inc.hpp
			include/ctre/actions/named_class.inc.hpp
			include/ctre/actions/backreference.inc.hpp
			include/ctre/actions/options.inc.hpp
			include/ctre/actions/atomic_group.inc.hpp
			include/ctre/actions/set.inc.hpp
			include/ctre/actions/hexdec.inc.hpp
			include/ctre/actions/repeat.inc.hpp
			include/ctre/actions/properties.inc.hpp
			include/ctre/actions/boundaries.inc.hpp
			include/ctre/operators.hpp
			include/ctre/pcre.hpp
			include/ctre/atoms_unicode.hpp
			include/ctre/range.hpp
			include/ctre/wrapper.hpp
			include/ctre/first.hpp
			include/ctre/flags_and_modes.hpp
			include/ctre/atoms.hpp
			include/unicode-db.hpp
			include/unicode-db/unicode_interface.hpp
			include/unicode-db/unicode-db.hpp
			include/ctll/parser.hpp
			include/ctll/actions.hpp
			include/ctll/fixed_string.hpp
			include/ctll/list.hpp
			include/ctll/utilities.hpp
			include/ctll/grammars.hpp
			include/ctre.hpp
			include/ctre-unicode.hpp
		)
		
		# we are using `import std;`
		if (NOT DEFINED CTRE_CXX_STANDARD OR CTRE_CXX_STANDARD LESS 23)
			set(CTRE_CXX_STANDARD 23)
		endif()

		target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_${CTRE_CXX_STANDARD})

		install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}-targets
			FILE_SET CXX_MODULES DESTINATION "${CMAKE_INSTALL_LIBDIR}/cxx/${PROJECT_NAME}"
			FILE_SET HEADERS DESTINATION "include")
	else()
		message(FATAL_ERROR "unsupported cmake for c++ modules")
	endif()
else()
	add_library(${PROJECT_NAME} INTERFACE)
	
	target_include_directories(${PROJECT_NAME} INTERFACE
	  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
	  $<INSTALL_INTERFACE:include>)

	if (NOT CTRE_CXX_STANDARD)
	  set(CTRE_CXX_STANDARD 20)
	endif()

	target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_${CTRE_CXX_STANDARD})
	set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_SCAN_FOR_MODULES 0)
	
	install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}-targets)
	install(DIRECTORY include/ DESTINATION include
	    FILES_MATCHING PATTERN *.hpp)
endif()

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

if (NOT EXISTS "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake.in")
  file(WRITE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake.in [[
    @PACKAGE_INIT@
    include("${CMAKE_CURRENT_LIST_DIR}/@PROJECT_NAME@-targets.cmake")
  ]])
endif()

configure_package_config_file(
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake.in"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file(ctre-config-version.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

install(EXPORT ${PROJECT_NAME}-targets 
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
  NAMESPACE ${PROJECT_NAME}::)
install(
  FILES
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

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
	DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
)

list(APPEND CPACK_SOURCE_IGNORE_FILES /.git/ /build/ .gitignore .DS_Store)

include(CPack)
