include(CMakeDependentOption)

option(BUILD_ADDONS "Build addons" OFF)
cmake_dependent_option(BUILD_ADDON_PARSER "Build parser addon" OFF "BUILD_ADDONS" OFF)
cmake_dependent_option(BUILD_ADDON_PYCARL "Build python binding addon" OFF "BUILD_ADDONS" OFF)

if(BUILD_ADDONS)

	add_custom_target(addons)
	add_dependencies(addons carl-arith-shared)
	set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/addons)
	
	if(BUILD_ADDON_PYCARL)
		set(BUILD_ADDON_PARSER ON)
	endif()

	if(BUILD_ADDON_PARSER)
		include(resources/addons/carl-parser.cmake)
	endif()
	if(BUILD_ADDON_PYCARL)
		include(resources/addons/pycarl.cmake)
	endif()

endif()
