file(GLOB test-sources CONFIGURE_DEPENDS *.cpp)

add_custom_target(ctre-test)

include(CheckCXXSourceCompiles)
check_cxx_source_compiles("
#if !defined(__cpp_impl_reflection) || __cpp_impl_reflection < 202603L
#  error no reflection
#endif
int main() {}" CTRE_HAVE_CPP26_REFLECTION)
if(CTRE_HAVE_CPP26_REFLECTION)
  add_subdirectory(reflect)
endif()

foreach (source IN LISTS test-sources)
  get_filename_component(test "${source}" NAME_WE)
  add_library(ctre-test-${test} STATIC EXCLUDE_FROM_ALL ${source})
  target_link_libraries(ctre-test-${test} PRIVATE ctre)
  add_dependencies(ctre-test ctre-test-${test})
  
  target_compile_options(ctre-test-${test} PUBLIC "$<$<C_COMPILER_ID:MSVC>:/utf-8>")
  target_compile_options(ctre-test-${test} PUBLIC "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endforeach()


