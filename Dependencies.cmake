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

  if(NOT TARGET spdlog::spdlog)
    cpmaddpackage(
      NAME
      spdlog
      VERSION
      1.15.2
      GITHUB_REPOSITORY
      "gabime/spdlog"
      OPTIONS
      "SPDLOG_FMT_EXTERNAL ON")
  endif()

  if(NOT TARGET Catch2::Catch2WithMain)
    cpmaddpackage("gh:catchorg/Catch2@3.8.1")
  endif()

  if(NOT TARGET CLI11::CLI11)
    cpmaddpackage("gh:CLIUtils/CLI11@2.5.0")
  endif()

  if(NOT TARGET ftxui::screen)
    cpmaddpackage("gh:ArthurSonzogni/FTXUI@6.0.2")
  endif()

  if(NOT TARGET tools::tools)
    cpmaddpackage("gh:lefticus/tools#update_build_system")
  endif()
	
	if(NOT TARGET SFML::SFML
		cpmaddpackage(
			NAME 
			SFML
			GIT_TAG 
			3.0.2
			GITHUB_REPOSITORY 
			SFML/SFML
			GIT_SHALLOW 
			TRUE
			SYSTEM 
			TRUE)
	endif()
	
	# Fetch Dear ImGui
	if(NOT TARGET ImGui)
		cpmaddpackage(
			NAME ImGui
			GITHUB_REPOSITORY ocornut/imgui
			GIT_TAG v1.91.1
			GIT_SHALLOW TRUE
			SYSTEM TRUE
		)
	endif()

	# Make ImGui's source directory available
	set(IMGUI_DIR ${ImGui_SOURCE_DIR})

	# Disable ImGui-SFML's internal find_package(SFML)
	set(IMGUI_SFML_FIND_SFML OFF)

	# Fetch ImGui-SFML binding
	if(NOT TARGET ImGui-SFML)
		cpmaddpackage(
			NAME ImGui-SFML
			GITHUB_REPOSITORY SFML/imgui-sfml
			GIT_TAG v3.0
			GIT_SHALLOW TRUE
			SYSTEM TRUE
		)
	endif()

endfunction()
