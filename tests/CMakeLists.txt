cmake_minimum_required(VERSION 3.12)

enable_language(CXX)

# CONFIGURE_DEPENDS was added in 3.12, so tests require at least that version
file(GLOB test-sources CONFIGURE_DEPENDS *.cpp)

# TODO: remove once nothing depends on this (replaced by CTest tests)
add_custom_target(ctre-test)

foreach(source IN LISTS test-sources)
  get_filename_component(test "${source}" NAME_WE)
  add_library("ctre-test-${test}" STATIC EXCLUDE_FROM_ALL "${source}")
  target_link_libraries("ctre-test-${test}" PRIVATE ctre::ctre)
  add_dependencies(ctre-test "ctre-test-${test}")
  # The tests passes only if the above target compiles successfully
  add_test(NAME "ctre.${test}" COMMAND
    "${CMAKE_COMMAND}" --build "${ctre_BINARY_DIR}" --config "$<CONFIG>"
    --target "ctre-test-${test}")
endforeach()
