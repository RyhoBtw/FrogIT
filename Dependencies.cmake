include(cmake/CPM.cmake)

# Done as a function so that updates to variables like
# CMAKE_CXX_FLAGS don't propagate out to other
# targets
function(FrogIT_setup_dependencies)

	# For each dependency, see if it's
	# already been provided to us by a parent project

	if(NOT TARGET fmtlib::fmtlib)
	cpmaddpackage("gh:fmtlib/fmt#11.1.4")
	endif()

	if(NOT TARGET Catch2::Catch2WithMain)
	cpmaddpackage("gh:catchorg/Catch2@3.8.1")
	endif()

	# SFML and ImGui
	include(FetchContent)
	FetchContent_Declare(SFML
		GIT_REPOSITORY https://github.com/SFML/SFML.git
		GIT_TAG 3.0.1
		GIT_SHALLOW ON
		EXCLUDE_FROM_ALL
		SYSTEM)
	FetchContent_MakeAvailable(SFML)
	FetchContent_Declare(ImGui
		GIT_REPOSITORY https://github.com/ocornut/imgui
		GIT_TAG v1.91.1
		GIT_SHALLOW ON
		EXCLUDE_FROM_ALL
		SYSTEM)
	FetchContent_MakeAvailable(ImGui)
	FetchContent_GetProperties(ImGui SOURCE_DIR IMGUI_DIR)
	set(IMGUI_SFML_FIND_SFML OFF)
	FetchContent_Declare(ImGui-SFML
		GIT_REPOSITORY https://github.com/SFML/imgui-sfml
		GIT_TAG v3.0
		GIT_SHALLOW ON
		EXCLUDE_FROM_ALL
		SYSTEM)
	FetchContent_MakeAvailable(ImGui-SFML)

endfunction()
