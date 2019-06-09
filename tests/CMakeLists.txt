file(GLOB ctre_self_tests *.cpp)

add_custom_target(ctre_self_test)

foreach(ctre_self_test_file ${ctre_self_tests})
	get_filename_component(ctre_self_test_name ${ctre_self_test_file} NAME)
	message(STATUS ${ctre_self_test_name})
	
	add_library(ctre_self_test_${ctre_self_test_name} STATIC ${ctre_self_test_name})
	target_link_libraries(ctre_self_test_${ctre_self_test_name} ctre)
	target_compile_features(ctre_self_test_${ctre_self_test_name} PRIVATE cxx_std_17)
	
	add_dependencies(ctre_self_test ctre_self_test_${ctre_self_test_name})
endforeach()