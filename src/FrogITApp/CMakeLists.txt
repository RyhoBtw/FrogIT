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
		ImGui-SFML::ImGui-SFML
		fmt::fmt)
		
if(APPLE)
  target_link_libraries(FrogITApp PRIVATE "-framework Cocoa")
endif()
					
target_include_directories(FrogITApp PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
