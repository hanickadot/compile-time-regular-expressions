file(GLOB test-sources CONFIGURE_DEPENDS *.cpp)

add_custom_target(ctre-test)

foreach (source IN LISTS test-sources)
  get_filename_component(test "${source}" NAME_WE)
  add_library(ctre-test-${test} STATIC EXCLUDE_FROM_ALL ${source})
  target_link_libraries(ctre-test-${test} PRIVATE ctre)
  add_dependencies(ctre-test ctre-test-${test})
  
  target_compile_options(ctre-test-${test} PUBLIC "$<$<C_COMPILER_ID:MSVC>:/utf-8>")
  target_compile_options(ctre-test-${test} PUBLIC "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endforeach()


