file(GLOB_RECURSE SRC CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(FrogITApp ${SRC})

# Help Visual Studio with project settings
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT FrogITApp)

target_link_libraries(
  FrogITApp
  PRIVATE 
		FrogIT::FrogIT_options
    FrogIT::FrogIT_warnings
		SFML::Graphics 
		SFML::Audio
		ImGui-SFML::ImGui-SFML
		fmt::fmt)
		
if(APPLE)
  target_link_libraries(FrogITApp PRIVATE "-framework Cocoa")
endif()
					
target_include_directories(FrogITApp PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")

# Coppy assets for github releases
install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/assets
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Copy assets to build/bin folder after build
add_custom_command(
    TARGET FrogITApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:FrogITApp>/assets
)

# Set Visual Studio debugger working directory to the executable folder
if(MSVC)
    set_target_properties(FrogITApp PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:FrogITApp>"
    )
endif()
